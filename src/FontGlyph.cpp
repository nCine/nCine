#include "FontGlyph.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FontGlyph::FontGlyph()
	: x_(0), y_(0), width_(0), height_(0), xOffset_(0),
	  yOffset_(0), xAdvance_(0), kernings_(4)
{

}

FontGlyph::FontGlyph(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
	int xOffset, int yOffset, unsigned int xAdvance)
	: x_(x), y_(y), width_(width), height_(height),
	  xOffset_(xOffset), yOffset_(yOffset), xAdvance_(xAdvance),
	  kernings_(4)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Return the kerning amount for a subsequent glyph
int FontGlyph::kerning(int secondGlyph) const
{
	int kerningAmount = 0;

	for (unsigned int i = 0; i < kernings_.size(); i++)
	{
		if (secondGlyph == kernings_[i].secondGlyph_)
		{
			kerningAmount = kernings_[i].amount_;
			break;
		}
	}

	return kerningAmount;
}

}
