#include "FontGlyph.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FontGlyph::FontGlyph()
	: FontGlyph(0, 0, 0, 0, 0, 0, 0)
{

}

FontGlyph::FontGlyph(unsigned int x, unsigned int y, unsigned int width, unsigned int height,
                     int xOffset, int yOffset, int xAdvance)
	: x_(x), y_(y), width_(width), height_(height),
	  xOffset_(xOffset), yOffset_(yOffset), xAdvance_(xAdvance),
	  kernings_(4)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

int FontGlyph::kerning(unsigned int secondGlyph) const
{
	int kerningAmount = 0;

	for (const Kerning &kerning : kernings_)
	{
		if (secondGlyph == kerning.secondGlyph_)
		{
			kerningAmount = kerning.amount_;
			break;
		}
	}

	return kerningAmount;
}

}
