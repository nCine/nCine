#ifndef CLASS_NCILOGGER
#define CLASS_NCILOGGER

class ncILogger
{
public:
	virtual void Write(int iLevel, const char *message, ...) = 0;
};

#endif

#ifndef CLASS_NCNULLLOGGER
#define CLASS_NCNULLLOGGER

class ncNullLogger : public ncILogger
{
public:
	virtual void Write(int iLevel, const char *message, ...) { }
};

#endif
