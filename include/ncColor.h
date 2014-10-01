#ifndef CLASS_NCCOLOR
#define CLASS_NCCOLOR

/// A 32 bit color
class ncColor
{
 public:
	/// Default constructor (white color)
	ncColor()
		{ Set(255, 255, 255, 255); }
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
	inline unsigned char R() const { return m_ubColor[0]; }
	/// Getter for the green component of the color
	inline unsigned char G() const { return m_ubColor[1]; }
	/// Getter for the blue component of the color
	inline unsigned char B() const { return m_ubColor[2]; }
	/// Getter for the alpha component of the color
	inline unsigned char A() const { return m_ubColor[3]; }
	/// Getter for the color vector
	/*! Note: It is useful with glColor4ub */
	inline const unsigned char* Vector() const { return m_ubColor; }
	/// Flaot getter for the red component of the color
	inline float fR() const { return m_ubColor[0]/255.0f; }
	/// Float getter for the green component of the color
	inline float fG() const { return m_ubColor[1]/255.0f; }
	/// Float getter for the blue component of the color
	inline float fB() const { return m_ubColor[2]/255.0f; }
	/// Float getter for the alpha component of the color
	inline float fA() const { return m_ubColor[3]/255.0f; }

	/// Ses four color components (unsigned char)
	void Set(unsigned char ubR, unsigned char ubG, unsigned char ubB, unsigned char ubA);
	// Sets three color components (unsigned char)
	void Set(unsigned char ubR, unsigned char ubG, unsigned char ubB);
	// Sets four color components (unsigned char vector)
	void Setv(unsigned char vColor[4]);

	// Sets four color components (normalized float)
	void SetF(float fR, float fG, float fB, float fA);
	// Sets three color components (normalized float)
	void SetF(float fR, float fG, float fB);
	// Sets four color components (float vector)
	void SetFv(float fvColor[4]);

	// Sets the alpha component (unsigned char)
	void SetAlpha(unsigned char ubA);
	// Sets the alpha component (normalized float)
	void SetAlphaF(float fA);

	// Equality operator
	bool operator==(const ncColor& color) const;
	ncColor operator*(const ncColor& col) const;
	// Multiplication by a constant scalar
	ncColor operator*(float fC) const;

 private:
	/// The four unsigned byte color components
	unsigned char m_ubColor[4];
};


/// Sets four color components (unsigned char)
inline void ncColor::Set(unsigned char ubR, unsigned char ubG, unsigned char ubB, unsigned char ubA)
{
	m_ubColor[0] = ubR;
	m_ubColor[1] = ubG;
	m_ubColor[2] = ubB;
	m_ubColor[3] = ubA;
}

/// Sets three color components (unsigned char)
inline void ncColor::Set(unsigned char ubR, unsigned char ubG, unsigned char ubB)
{
	Set(ubR, ubG, ubB, 255);
}

/// Sets four color components (unsigned char vector)
inline void ncColor::Setv(unsigned char vColor[4])
{
	Set(vColor[0], vColor[1], vColor[2], vColor[3]);
}

/// Sets four color components (normalized float)
inline void ncColor::SetF(float fR, float fG, float fB, float fA)
{
	// TODO: Clamp negative values too
	m_ubColor[0] = fR > 1.0f ? 255 : fR * 255;
	m_ubColor[1] = fG > 1.0f ? 255 : fG * 255;
	m_ubColor[2] = fB > 1.0f ? 255 : fB * 255;
	m_ubColor[3] = fA > 1.0f ? 255 : fA * 255;
}

/// Sets three color components (normalized float)
inline void ncColor::SetF(float fR, float fG, float fB)
{
	SetF(fR, fG, fB, 1.0f);
}

/// Sets four color components (float vector)
inline void ncColor::SetFv(float fvColor[4])
{
	SetF(fvColor[0], fvColor[1], fvColor[2], fvColor[3]);
}

/// Sets the alpha component (unsigned char)
inline void ncColor::SetAlpha(unsigned char ubA)
{
	m_ubColor[3] = ubA;
}

/// Sets the alpha component (normalized float)
inline void ncColor::SetAlphaF(float fA)
{
	// TODO: Clamp negative values too
	m_ubColor[3] = fA > 1.0f ? 255 : fA * 255;
}

/// Equality operator
inline bool ncColor::operator==(const ncColor& color) const
{
	return (R() == color.R() && G() == color.G() &&
			B() == color.B() && A() == color.A());
}

inline ncColor ncColor::operator*(const ncColor& col) const
{
	ncColor result;

	for (int i = 0; i < 4; i++)
	{
		float fMul = (col.m_ubColor[i] / 255.0f) * m_ubColor[i];
		// clamping
		if (fMul > 255.0f)
			fMul = 255.0f;
		else if (fMul < 0.0f)
			fMul = 0.0f;

		result.m_ubColor[i] = fMul;
	}

	return result;
}

/// Multiplication by a constant scalar
inline ncColor ncColor::operator*(float fC) const
{
	ncColor result;

	for (int i = 0; i < 4; i++)
	{
		float fMul = fC * m_ubColor[i];
		// clamping
		if (fMul > 255.0f)
			fMul = 255.0f;
		else if (fMul < 0.0f)
			fMul = 0.0f;

		result.m_ubColor[i] = fMul;
	}

	return result;
}

#endif
