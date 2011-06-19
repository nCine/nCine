#ifndef CLASS_ILOGGER
#define CLASS_ILOGGER

class ILogger
{
public:
	virtual void Write(int iLevel, const char *message, ...) = 0;
};

#endif

#ifndef CLASS_NULLLOGGER
#define CLASS_NULLLOGGER

class NullLogger : public ILogger
{
public:
	virtual void Write(int iLevel, const char *message, ...) { }
};

#endif
