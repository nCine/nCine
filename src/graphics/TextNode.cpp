#include "TextNode.h"
#include "FontGlyph.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextNode::TextNode(SceneNode *parent, Font *font, unsigned int maxStringLength)
	: DrawableNode(parent, 0, 0), string_(maxStringLength), dirtyDraw_(true), dirtyBoundaries_(true),
	  withKerning_(true), font_(font), interleavedVertices_(32),
	  xAdvance_(0.0f), xAdvanceSum_(0.0f), yAdvance_(0.0f), yAdvanceSum_(0.0f), lineLengths_(4), alignment_(ALIGN_LEFT)
{
	init(maxStringLength);
}

TextNode::TextNode(SceneNode *parent, Font *font)
	: DrawableNode(parent, 0, 0), string_(DefaultStringLength), dirtyDraw_(true), dirtyBoundaries_(true),
	  withKerning_(true), font_(font), interleavedVertices_(32),
	  xAdvance_(0.0f), xAdvanceSum_(0.0f), yAdvance_(0.0f), yAdvanceSum_(0.0f), lineLengths_(4), alignment_(ALIGN_LEFT)
{
	init(DefaultStringLength);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

float TextNode::width() const
{
	calculateBoundaries();

	return xAdvanceSum_ * currentAbsScale();
}

float TextNode::height() const
{
	calculateBoundaries();

	return yAdvanceSum_ * currentAbsScale();
}

void TextNode::enableKerning(bool withKerning)
{
	if (withKerning != withKerning_)
	{
		withKerning_ = withKerning;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::setAlignment(Alignment alignment)
{
	if (alignment != alignment_)
	{
		alignment_ = alignment;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::setString(const String &string)
{
	if (string_ != string)
	{
		string_ = string;
		dirtyDraw_ = true;
		dirtyBoundaries_ = true;
	}
}

void TextNode::draw(RenderQueue &renderQueue)
{
	// Precalculate boundaries for horizontal alignment
	calculateBoundaries();

	if (dirtyDraw_)
	{
		// Clear every previous quad before drawing again
		interleavedVertices_.clear();

		unsigned int currentLine = 0;
		xAdvance_ = calculateAlignment(currentLine);
		yAdvance_ = 0.0f;
		for (unsigned int i = 0; i < string_.length(); i++)
		{
			if (string_[i] == '\n')
			{
				currentLine++;
				xAdvance_ = calculateAlignment(currentLine);
				yAdvance_ += font_->base();;
			}
			else
			{
				const FontGlyph *glyph = font_->glyph(static_cast<unsigned int>(string_[i]));
				if (glyph)
				{
					processGlyph(glyph);
					if (withKerning_)
					{
						// font kerning
						if (i < string_.length() - 1)
						{
							xAdvance_ += glyph->kerning(int(string_[i + 1]));
						}
					}
				}
			}
		}

		// Uploading data to the VBO only if the string changes
		renderCommand_->geometry().setDrawParameters(GL_TRIANGLES, 0, interleavedVertices_.size() / 4);
		renderCommand_->geometry().updateVboData(0, interleavedVertices_.size(), interleavedVertices_.data());
	}

	DrawableNode::draw(renderQueue);
	dirtyDraw_ = false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextNode::init(unsigned int stringLength)
{
	type_ = TEXTNODE_TYPE;
	setLayer(DrawableNode::HUD_LAYER);
	renderCommand_->setType(RenderCommand::TEXT_TYPE);
	renderCommand_->material().setTransparent(true);
	renderCommand_->material().setShaderProgram(Material::TEXTNODE_PROGRAM);
	renderCommand_->material().setTexture(*font_->texture());
	// `stringLength` characters, each has 6 vertices with 2 components for position and 2 for texcoords
	renderCommand_->geometry().createCustomVbo(stringLength * 6 * 2 * 2, GL_DYNAMIC_DRAW);
}

void TextNode::calculateBoundaries() const
{
	if (dirtyBoundaries_)
	{
		lineLengths_.clear();
		float xAdvanceMax = 0.0f; // longest line
		xAdvance_ = 0.0f;
		yAdvance_ = 0.0f;
		for (unsigned int i = 0; i < string_.length(); i++)
		{
			if (string_[i] == '\n')
			{
				lineLengths_.pushBack(xAdvance_);
				if (xAdvance_ > xAdvanceMax)
				{
					xAdvanceMax = xAdvance_;
				}
				xAdvance_ = 0.0f;
				yAdvance_ += font_->base();
			}
			else
			{
				const FontGlyph *glyph = font_->glyph(static_cast<unsigned int>(string_[i]));
				if (glyph)
				{
					xAdvance_ += glyph->xAdvance();
					if (withKerning_)
					{
						// font kerning
						if (i < string_.length() - 1)
						{
							xAdvance_ += glyph->kerning(int(string_[i + 1]));
						}
					}
				}
			}
		}
		lineLengths_.pushBack(xAdvance_);
		if (xAdvance_ > xAdvanceMax)
		{
			xAdvanceMax = xAdvance_;
		}

		xAdvanceSum_ = xAdvanceMax;
		yAdvanceSum_ = yAdvance_;

		dirtyBoundaries_ = false;
	}
}

float TextNode::calculateAlignment(unsigned int lineIndex) const
{
	float alignOffset = 0.0f;

	switch (alignment_)
	{
		case ALIGN_LEFT:
			alignOffset = 0.0f;
			break;
		case ALIGN_CENTER:
			alignOffset = (xAdvanceSum_ - lineLengths_[lineIndex]) * 0.5f;
			break;
		case ALIGN_RIGHT:
			alignOffset = xAdvanceSum_ - lineLengths_[lineIndex];
			break;
	}

	return alignOffset;
}

float TextNode::currentAbsScale() const
{
	float absScaleFactor = scaleFactor_;
	if (parent_)
	{
		absScaleFactor *= parent_->absScale();
	}

	return absScaleFactor;
}

void TextNode::processGlyph(const FontGlyph *glyph)
{
	Vector2i size = glyph->size();
	Vector2i offset = glyph->offset();

	float leftPos = xAdvance_ + offset.x;
	float rightPos = leftPos + size.x;
	float topPos = -yAdvance_ - offset.y;
	float bottomPos = topPos - size.y;


	Vector2i texSize = font_->texture()->size();
	Recti texRect = glyph->texRect();

	float leftCoord = float(texRect.x) / float(texSize.x);
	float rightCoord = float(texRect.x + texRect.w) / float(texSize.x);
	float bottomCoord = float(texRect.y + texRect.h) / float(texSize.y);
	float topCoord = float(texRect.y) / float(texSize.y);


	interleavedVertices_.pushBack(leftPos);		interleavedVertices_.pushBack(bottomPos);
	interleavedVertices_.pushBack(leftCoord);	interleavedVertices_.pushBack(bottomCoord);
	interleavedVertices_.pushBack(leftPos);		interleavedVertices_.pushBack(topPos);
	interleavedVertices_.pushBack(leftCoord);	interleavedVertices_.pushBack(topCoord);
	interleavedVertices_.pushBack(rightPos);	interleavedVertices_.pushBack(bottomPos);
	interleavedVertices_.pushBack(rightCoord);	interleavedVertices_.pushBack(bottomCoord);

	interleavedVertices_.pushBack(rightPos);	interleavedVertices_.pushBack(bottomPos);
	interleavedVertices_.pushBack(rightCoord);	interleavedVertices_.pushBack(bottomCoord);
	interleavedVertices_.pushBack(rightPos);	interleavedVertices_.pushBack(topPos);
	interleavedVertices_.pushBack(rightCoord);	interleavedVertices_.pushBack(topCoord);
	interleavedVertices_.pushBack(leftPos);		interleavedVertices_.pushBack(topPos);
	interleavedVertices_.pushBack(leftCoord);	interleavedVertices_.pushBack(topCoord);

	xAdvance_ += glyph->xAdvance();
}

/*! \todo Only the transformation matrix should be updated per frame */
void TextNode::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;
	renderCommand_->material().uniform("color")->setFloatValue(absColor().fR(), absColor().fG(), absColor().fB(), absColor().fA());
}

}
