#include "client/crashpad_client.h"
#include "client/crash_report_database.h"
#include "client/settings.h"
#include "client/simulate_crash.h"

#if defined(_WIN32)
	#undef ASSERT
	#undef min
	#undef max
#elif defined(__ANDROID__)
	#include "AndroidJniHelper.h"
	#include "IFile.h"
	#include "AssetFile.h"
#endif

#include "common_macros.h"
#include "Crashpad.h"
#include "FileSystem.h"
#include "Application.h"

#include "version.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool Crashpad::isInitialized_ = false;

bool Crashpad::shouldInitialize_ = true;
nctl::String Crashpad::handlerPath_(fs::MaxPathLength);

nctl::String Crashpad::handlerName_ =
#if defined(_WIN32)
	"crashpad_handler.exe";
#elif defined(__ANDROID__)
	"libcrashpad_handler.so";
#else
	"crashpad_handler";
#endif

nctl::String Crashpad::dataPath_(fs::MaxPathLength);
nctl::String Crashpad::dataDirectoryName_ = "crashpad";
nctl::String Crashpad::serverUrl_(256);
nctl::Array<nctl::String> Crashpad::annotations_;
nctl::Array<nctl::String> Crashpad::attachments_;

nctl::UniquePtr<crashpad::CrashpadClient> Crashpad::client_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Crashpad::initialize()
{
	ASSERT(isInitialized_ == false);
	if (isInitialized_ || shouldInitialize_ == false)
		return false;

	nctl::String exeDirString;
	nctl::String executablePathString;
#if !defined(__ANDROID__)
	// 1) Try with the provided data directory
	if (fs::isDirectory(handlerPath_.data()))
		exeDirString = handlerPath_;
	executablePathString = fs::joinPath(exeDirString, handlerName_);
	if (handlerPath_.isEmpty() || fs::isExecutable(executablePathString.data()) == false)
	{
		if (handlerPath_.isEmpty() == false)
			LOGW_X("Cannot find Crashpad handler executable in the directory provided: \"%s\"", executablePathString.data());

		// 2) Then try with the directory where the current executable resides
		exeDirString = fs::executableDir();
		executablePathString = fs::joinPath(exeDirString, handlerName_);
		if (fs::isExecutable(executablePathString.data()) == false)
		{
			LOGW_X("Cannot find Crashpad handler executable in current executable directory: \"%s\"", executablePathString.data());

			// 3) Finally fallback to the current working directory
			exeDirString = fs::currentDir();
			executablePathString = fs::joinPath(exeDirString, handlerName_);
			if (fs::isExecutable(executablePathString.data()) == false)
			{
				LOGW_X("Cannot find Crashpad handler executable in the working directory: \"%s\"", executablePathString.data());
				return false;
			}
		}
	}
#else
	AndroidJniClass_ApplicationInfo appInfo = AndroidJniWrap_Context::getApplicationInfo();
	char nativeLibDir[256];
	appInfo.nativeLibraryDir(nativeLibDir, sizeof(nativeLibDir));

	if (fs::isDirectory(nativeLibDir))
	{
		exeDirString = nativeLibDir;
		executablePathString = fs::joinPath(exeDirString, handlerName_);
		if (fs::isExecutable(executablePathString.data()) == false)
			LOGW_X("Cannot find Crashpad handler executable in the native library directory: \"%s\"", executablePathString.data());
	}
#endif

	nctl::String dataDirString;
#if !defined(__ANDROID__)
	// 1) Try with the provided data directory
	dataDirString = dataPath_;
	if (dataPath_.isEmpty() || fs::isDirectory(dataDirString.data()) == false || fs::isWritable(dataDirString.data()) == false)
	{
		if (dataPath_.isEmpty() == false)
			LOGW_X("Cannot write Crashpad data in the data directory provided: \"%s\"", dataDirString.data());

		// 2) Then try with the directory where the handler executable resides
		dataDirString = exeDirString;
		if (fs::isDirectory(dataDirString.data()) == false || fs::isWritable(dataDirString.data()) == false)
		{
			LOGW_X("Cannot write Crashpad data in the handler executable directory: \"%s\"", dataDirString.data());

			// 3) Finally fallback to the current working directory
			dataDirString = fs::currentDir();
			if (fs::isDirectory(dataDirString.data()) == false || fs::isWritable(dataDirString.data()) == false)
			{
				LOGW_X("Cannot write Crashpad data in the working directory: \"%s\"", dataDirString.data());
				return false;
			}
		}
	}
#else
	dataDirString = fs::savePath();
#endif
	nctl::String dataPathString = fs::joinPath(dataDirString, dataDirectoryName_);

#if defined(_WIN32)
	base::FilePath handler(std::wstring(executablePathString.data(), executablePathString.data() + executablePathString.length()));
	base::FilePath reportsDir(std::wstring(dataPathString.data(), dataPathString.data() + dataPathString.length()));
	base::FilePath metricsDir(std::wstring(dataPathString.data(), dataPathString.data() + dataPathString.length()));
#else
	base::FilePath handler(executablePathString.data());
	base::FilePath reportsDir(dataPathString.data());
	base::FilePath metricsDir(dataPathString.data());
#endif

	// Initialize Crashpad database
	std::unique_ptr<crashpad::CrashReportDatabase> database = crashpad::CrashReportDatabase::Initialize(reportsDir);
	if (database == nullptr)
		return false;

	std::string url = serverUrl_.data();

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	// Metadata that will be posted to the server with the crash report map
	std::map<std::string, std::string> annotations;
	annotations["format"] = "minidump";
	annotations["product"] = (appCfg.argc() && appCfg.argv(0)) ? fs::baseName(appCfg.argv(0)).data() : "nCine";
	annotations["ncine_version"] = VersionStrings::Version;
	annotations["ncine_git_rev_count"] = VersionStrings::GitRevCount;
	annotations["ncine_git_short_hash"] = VersionStrings::GitShortHash;
	annotations["ncine_git_last_commit_date"] = VersionStrings::GitLastCommitDate;
	annotations["ncine_git_branch"] = VersionStrings::GitBranch;
	annotations["ncine_git_tag"] = VersionStrings::GitTag;
	annotations["ncine_compilation_date"] = VersionStrings::CompilationDate;
	annotations["ncine_compilation_time"] = VersionStrings::CompilationTime;

	ASSERT(annotations_.size() % 2 == 0);
	for (unsigned int i = 0; i + 1 < annotations_.size(); i += 2)
	{
		const char *key = annotations_[i].data();
		const char *value = annotations_[i + 1].data();
		annotations[key] = value;
	}
	annotations_.clear();

	// Disable crashpad rate limiting so that all crashes have dmp files
	std::vector<std::string> arguments;
	arguments.push_back("--no-rate-limit");

	// Attach the log file if it exists
	std::vector<base::FilePath> attachments;
	if (appCfg.fileLogLevel != ILogger::LogLevel::OFF)
	{
		// Try to specify an absolute path for the log file attachment
		nctl::String logFilepath(512);
#if !defined(__ANDROID__)
		logFilepath = fs::joinPath(fs::currentDir(), appCfg.logFile);
		if (fs::isReadableFile(logFilepath.data()) == false)
		{
			logFilepath = fs::joinPath(fs::executableDir(), appCfg.logFile);
			if (fs::isReadableFile(logFilepath.data()) == false)
				logFilepath = appCfg.logFile;
		}
#else
		// On Android the `logFile` is already absolute, and uses the persistent files directory
		logFilepath = appCfg.logFile;
#endif

#if defined(_WIN32)
		base::FilePath attachment(std::wstring(logFilepath.data(), logFilepath.data() + logFilepath.length()));
#else
		base::FilePath attachment(logFilepath.data());
#endif
		attachments.push_back(attachment);
	}

	for (unsigned int i = 0; i < attachments_.size(); i++)
	{
#if defined(_WIN32)
		base::FilePath attachment(std::wstring(attachments_[i].data(), attachments_[i].data() + attachments_[i].length()));
#else
		base::FilePath attachment(attachments_[i].data());
#endif
		attachments.push_back(attachment);
	}
	attachments_.clear();

	// Set automated crash uploads if the server URL string is not empty
	crashpad::Settings *settings = database->GetSettings();
	if (settings == nullptr)
		return false;
	settings->SetUploadsEnabled(serverUrl_.isEmpty() == false);
	crashpad::UUID clientId;
	settings->GetClientID(&clientId);

	// Start crash handler
	client_ = nctl::makeUnique<crashpad::CrashpadClient>();
	isInitialized_ = client_->StartHandler(handler, reportsDir, metricsDir, url, annotations, arguments, true, true, attachments);

	if (isInitialized_ == false)
		LOGW("Cannot initialize the Crashpad client");
	else
		LOGI_X("The Crashpad client has been initialized correctly with ID: %s", clientId.ToString().c_str());

	return isInitialized_;
}

void Crashpad::simulateCrash()
{
	if (isInitialized_)
		CRASHPAD_SIMULATE_CRASH();
}

/*! \note Give the user a chance to skip Crashpad handler initialization in the `onInit()` callback */
void Crashpad::setShouldInitialize(bool shouldInitialize)
{
	if (isInitialized_ == false)
		shouldInitialize_ = shouldInitialize;
}

void Crashpad::setHandlerPath(const char *handlerPath)
{
	if (isInitialized_ == false)
		handlerPath_ = handlerPath;
}

void Crashpad::setHandlerName(const char *handlerName)
{
	if (isInitialized_ == false)
		handlerName_ = handlerName;
}

void Crashpad::setDataPath(const char *dataPath)
{
	if (isInitialized_ == false)
		dataPath_ = dataPath;
}

void Crashpad::setDataDirectoryName(const char *dataDirectoryName)
{
	if (isInitialized_ == false)
		dataDirectoryName_ = dataDirectoryName;
}

/*! \note Setting this to `nullptr` or an empty string disables the automatic upload of crash reports */
void Crashpad::setServerUrl(const char *serverUrl)
{
	if (isInitialized_)
		return;

	if (serverUrl == nullptr || serverUrl[0] == '\0')
		serverUrl_.clear();
	else
		serverUrl_ = serverUrl;
}

/*! \note It should be called before initialization */
bool Crashpad::addAnnotation(const char *key, const char *value)
{
	if (isInitialized_ || key == nullptr || value == nullptr || key[0] == '\0' || value[0] == '\0')
		return false;

	annotations_.pushBack(key);
	annotations_.pushBack(value);

	return true;
}

/*! \note It should be called before initialization */
bool Crashpad::addAttachment(const char *filename)
{
	if (isInitialized_ || filename == nullptr || filename[0] == '\0')
		return false;

	attachments_.pushBack(filename);

	return true;
}

}
