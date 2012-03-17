#ifndef CLASS_NCCOLOR
#define CLASS_NCCOLOR

/// A 32 bit color
class ncColor
{
public:
	/// The four unsigned byte color components
	GLubyte m_ubColor[4];

	/// Default constructor (white color)
	ncColor() { Set(255, 255, 255, 255); }
	/// Four components constructor (unsigned char)
	ncColor(unsigned char ubR, unsigned char ubG, unsigned char ubB, unsigned char ubA)
		{ Set(ubR, ubG, ubB, ubA); }
	/// Three components constructor (unsigned char)
	ncColor(unsigned char ubR, unsigned char ubG, unsigned char ubB)
		{ Set(ubR, ubG, ubB); }
	/// Four components constructor (unsigned int)
	ncColor(unsigned int uR, unsigned int uG, unsigned int uB, unsigned int uA)
		{ Set(uR, uG, uB, uA); }
	/// Three components constructor (unsigned int)
	ncColor(unsigned int uR, unsigned int uG, unsigned int uB)
		{ Set(uR, uG, uB); }
	/// Four components constructor (normalized float)
	ncColor(float fR, float fG, float fB, float fA)
		{ SetF(fR, fG, fB, fA); }
	/// Three components constructor (normalized float)
	ncColor(float fR, float fG, float fB)
		{ SetF(fR, fG, fB); }

	/// Getter for the red component of the color
	inline GLubyte R() const { return m_ubColor[0]; }
	/// Getter for the green component of the color
	inline GLubyte G() const { return m_ubColor[1]; }
	/// Getter for the blue component of the color
	inline GLubyte B() const { return m_ubColor[2]; }
	/// Getter for the alpha component of the color
	inline GLubyte A() const { return m_ubColor[3]; }
	/// Getter for the color vector
	/*! Note: It is useful with glColor4ub */
	inline const GLubyte* Vector() const { return m_ubColor; }
	/// Flaot getter for the red component of the color
	inline float fR() const { return m_ubColor[0]/255.0f; }
	/// Float getter for the green component of the color
	inline float fG() const { return m_ubColor[1]/255.0f; }
	/// Float getter for the blue component of the color
	inline float fB() const { return m_ubColor[2]/255.0f; }
	/// Float getter for the alpha component of the color
	inline float fA() const { return m_ubColor[3]/255.0f; }

	/// Sets four color elements (unsigned char)
	inline void Set(unsigned char ubR, unsigned char ubG, unsigned char ubB, unsigned char ubA)
	{
		m_ubColor[0] = ubR;
		m_ubColor[1] = ubG;
		m_ubColor[2] = ubB;
		m_ubColor[3] = ubA;
	}

	/// Sets three color elements (unsigned char)
	inline void Set(unsigned char ucR, unsigned char ucG, unsigned char ucB)
	{
		Set(ucR, ucG, ucB, 255);
	}

	/// Sets four color elements (unsigned char vector)
	inline void Setv(unsigned char vColor[4])
	{
		Set(vColor[0], vColor[1], vColor[2], vColor[3]);
	}

	/// Sets four color elements (normalized float)
	inline void SetF(float fR, float fG, float fB, float fA)
	{
		m_ubColor[0] = fR > 1.0f ? 255 : fR * 255;
		m_ubColor[1] = fG > 1.0f ? 255 : fG * 255;
		m_ubColor[2] = fB > 1.0f ? 255 : fB * 255;
		m_ubColor[3] = fA > 1.0f ? 255 : fA * 255;
	}

	/// Sets three color elements (normalized float)
	inline void SetF(float fR, float fG, float fB) {
		SetF(fR, fG, fB, 1.0f);
	}

	/// Sets four color elements (float vector)
	inline void SetFv(float fvColor[4])
	{
		SetF(fvColor[0], fvColor[1], fvColor[2], fvColor[3]);
	}

	/// Equality operator
	inline bool operator==(const ncColor& color) const {
		return (R() == color.R() && G() == color.G() &&
				B() == color.B() && A() == color.A());
	}
};

#endif
