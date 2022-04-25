#include "apptest_crash.h"
#include <ncine/Application.h>
#include <ncine/IAppEventHandler.h>
#include <ncine/Crashpad.h>

#include <ncine/version.h>

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	//config.fileLogLevel = nc::ILogger::LogLevel::OFF;
	nc::Crashpad::addAnnotation("product", "apptest_crash");

	nctl::String auxString(64);
	//nc::Crashpad::addAnnotation("sentry_key", "<public_key>");
	nc::Crashpad::addAnnotation("sentry_version", "7");
	auxString.format("apptest_crash/r%s", nc::VersionStrings::GitRevCount);
	nc::Crashpad::addAnnotation("sentry_client", auxString.data());
	auxString.format("apptest_crash/@r%s-%s", nc::VersionStrings::GitRevCount, nc::VersionStrings::GitShortHash);
	nc::Crashpad::addAnnotation("sentry_release", auxString.data());
#ifdef NCINE_DEBUG
	nc::Crashpad::addAnnotation("sentry_environment", "debug");
#else
	nc::Crashpad::addAnnotation("sentry_environment", "production");
#endif
}

void MyEventHandler::onInit()
{
	nc::Crashpad::simulateCrash();
	nc::theApplication().quit();
}
