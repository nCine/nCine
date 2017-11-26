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
