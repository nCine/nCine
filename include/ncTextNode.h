#ifndef CLASS_NCTEXTNODE
#define CLASS_NCTEXTNODE

#include "ncDrawableNode.h"
#include "ncRenderCommand.h"
#include "ncFont.h"

class ncFontGlyph;

/// A scene node to draw a text label
class ncTextNode : public ncDrawableNode
{
private:
	static const unsigned int MAX_LENGHT = 256;

	/// The string to be rendered
	char m_vString[MAX_LENGHT];
	/// Dirty flag for vertices and texture coordinates
	bool m_bDirty;
	/// Kerning flag for rendering
	bool m_bWithKerning;
	/// The font class used to render text
	ncFont *m_pFont;
	/// Scale factor for glyph size
	float m_fScaleFactor;
	/// The array of vertices for every glyph in the batch
	ncArray<float> m_vVertices;
	/// The array of texture coordinates for every glyph in the batch
	ncArray<float> m_vTexCoords;

	/// Advance on the X-axis for the next processed glyph
	mutable float m_uXAdvance;
	/// Total advance on the X-axis for the entire string (its width)
	mutable unsigned int m_uXAdvanceSum;
	// Fills the batch draw command with data from a glyph
	void ProcessGlyph(const ncFontGlyph* pGlyph);

	virtual void UpdateRenderCommand();
public:
	ncTextNode(ncSceneNode* pParent, ncFont *pFont);
	virtual ~ncTextNode() { }

	// Returns the total sum of advances for a string (its width)
	unsigned int XAdvanceSum() const;
	/// Is kerning enabled for this node rendering?
	inline bool withKerning() const { return m_bWithKerning; }
	/// Sets the kerning flag for this node rendering
	inline void EnableKerning(bool bWithKerning) { m_bWithKerning = bWithKerning; }
	/// Gets the glyph scale factor
	inline float Scale() const { return m_fScaleFactor; }
	/// Scales the glyph size
	inline void SetScale(float fScaleFactor) {
		m_fScaleFactor = fScaleFactor;
		m_bDirty = true;
	}
	/// Gets the font base scaled by the scale factor
	inline float FontBase() const { return m_pFont->Base() * m_fScaleFactor; }
	/// Gets the font line height scaled by the scale factor
	inline float FontLineHeight() const { return m_pFont->LineHeight() * m_fScaleFactor; }
	// Sets the string to render
	void SetString(const char *pString);

	virtual void Visit(ncRenderQueue& rRenderQueue);
	virtual void Draw(ncRenderQueue& rRenderQueue);

	inline static eObjectType sType() { return TEXT_TYPE; }
	static ncTextNode* FromId(unsigned int uId);
};

#endif
