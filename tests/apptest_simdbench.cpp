#include <ncine/imgui.h>

#include "apptest_simdbench.h"
#include <nctl/StaticString.h>
#include <ncine/Application.h>
#include <ncine/LuaStateManager.h>
#include <ncine/LuaUtils.h>
#include <ncine/IFile.h>
#include <ncine/TimeStamp.h>
#include <ncine/Vector4.h>
#include <ncine/Quaternion.h>
#include "apptest_datapath.h"
#include "Statistics.h"

#ifdef __EMSCRIPTEN__
	#include <ncine/EmscriptenLocalFile.h>
#endif

namespace Names {

const char *testSet = "test_set";
const char *testSystem = "system";
const char *testName = "name";
const char *testIterations = "iterations";
const char *testTimings = "timings";
const char *testTotalTime = "total_time";
const char *testAverage = "average";
const char *testMedian = "median";
const char *testMode = "mode";
const char *testPercentile75 = "percentile_75";
const char *testPercentile90 = "percentile_90";
const char *testStdDeviation = "standard_deviation";
const char *testRelStdDeviation = "relative_standard_deviation";
const char *testMinTime = "min_time";
const char *testMaxTime = "max_time";
const char *testRangeTime = "range_time";

};

namespace {

enum Tests
{
	Vector4Add,
	Vector4Sub,
	Vector4Mul,
	Vector4Div,
	Vector4Length,
	Vector4SqrLength,
	Vector4Normalize,
	Vector4Dot,
	QuaternionMult,
	MatrixMult,
	MatrixTrans,
	MatrixVecMult,

	Count
};

typedef float (*TestFunction)(unsigned int);
const unsigned int MaxTestRuns = 3;
const unsigned int MaxRepetitions = 32;
const unsigned int MaxIterations = 10000000;
const unsigned int MaxDataElements = 512;
static_assert(MaxDataElements <= MaxIterations, "It's useless to have more elements than iterations");

struct TestInfo
{
	TestInfo()
	    : func(nullptr), numIterations(0), stats(MaxRepetitions) {}

	nctl::StaticString<128> name;
	TestFunction func;
	unsigned int numIterations;
	Statistics stats;
};

struct TestRun
{
	nctl::String filename = nctl::String(128);
	nctl::String system;
	TestInfo testInfos[Tests::Count];
};

TestInfo testInfos[Tests::Count];
const char *testNames[Tests::Count];
TestRun testRuns[MaxTestRuns];
int currentTestRun = 0;
const unsigned int MaxStringLength = 128;
char const * const SaveDirectory = "apptest_simdbench";
char loadingFilename[MaxStringLength] = "timings.lua";
char savingFilename[MaxStringLength] = "timings.lua";
bool includeStatsWhenSaving = false;
#ifdef __EMSCRIPTEN__
nc::EmscriptenLocalFile localFileLoad;
#endif

static nc::TimeStamp testStartTime;
nctl::UniquePtr<float[]> nums;
nctl::UniquePtr<nc::Vector4f[]> vecsA;
nctl::UniquePtr<nc::Vector4f[]> vecsB;
nctl::UniquePtr<nc::Vector4f[]> vecsC;
nctl::UniquePtr<nc::Quaternionf[]> quats;
nctl::UniquePtr<nc::Matrix4x4f[]> mats;

int currentTest = 0;
int numIterations = MaxIterations / 5;
int numRepetitions = MaxRepetitions / 2;

bool runningAllTests = false;
unsigned int allTestsIndex = 0;
nctl::String progressMsg;
const ImVec4 Green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 Red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

const char *system()
{
#if defined(_WIN32)
	return "Windows";
#elif defined(__APPLE__)
	return "macOS";
#elif defined(__ANDROID__)
	return "Android";
#elif defined(__EMSCRIPTEN__)
	return "Emscripten";
#else
	return "Linux";
#endif
}

void runTest(TestInfo &t, unsigned int numRepetitions, unsigned int numIterations)
{
	ASSERT(numRepetitions > 0);
	ASSERT(numIterations > 0);
	FATAL_ASSERT(t.func != nullptr);

	t.stats.clearValues();
	t.stats.resetStats();

	t.numIterations = numIterations;
	for (unsigned int i = 0; i < numRepetitions; i++)
		t.stats.addValue(t.func(t.numIterations));

	t.stats.calculateStats();
}

void resetVecs(unsigned int iterations)
{
	const unsigned int n = (iterations < MaxDataElements) ? iterations : MaxDataElements;

	for (unsigned int i = 0; i < n; i++)
	{
		const float f = static_cast<float>(i);
		nums[i] = f;
		vecsA[i].set(f, f, f, f);
		vecsB[i].set(f, f, f, f);
		vecsC[i].set(f, f, f, f);
	}
}

void resetQuats(unsigned int iterations)
{
	const unsigned int n = (iterations < MaxDataElements) ? iterations : MaxDataElements;

	for (unsigned int i = 0; i < n; i++)
	{
		const float f = static_cast<float>(i);
		quats[i].set(f, f, f, f);
	}
}

void resetMats(unsigned int iterations)
{
	const unsigned int n = (iterations < MaxDataElements) ? iterations : MaxDataElements;

	for (unsigned int i = 0; i < n; i++)
	{
		const float f = static_cast<float>(i);
		mats[i][0].set(f, f, f, f);
		mats[i][1].set(f, f, f, f);
		mats[i][2].set(f, f, f, f);
		mats[i][3].set(f, f, f, f);
	}
}

float benchVector4Add(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = vecsA[index] + vecsB[index];
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Sub(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = vecsA[index] - vecsB[index];
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Mul(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = vecsA[index] * vecsB[index];
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Div(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = vecsA[index] / vecsB[index];
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Length(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations; i++)
	{
		const unsigned int index = i % MaxDataElements;
		nums[index] = vecsA[index].length();
	}

	return testStartTime.millisecondsSince();
}

float benchVector4SqrLength(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations; i++)
	{
		const unsigned int index = i % MaxDataElements;
		nums[index] = vecsA[index].sqrLength();
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Normalize(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = vecsA[index].normalize();
	}

	return testStartTime.millisecondsSince();
}

float benchVector4Dot(unsigned int iterations)
{
	resetVecs(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		nums[index] = nc::dot(vecsA[index], vecsB[index]);
	}

	return testStartTime.millisecondsSince();
}

float benchQuaternionMult(unsigned int iterations)
{
	resetQuats(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		quats[index] = quats[index] * quats[index + 1];
	}

	return testStartTime.millisecondsSince();
}

float benchMatrixMult(unsigned int iterations)
{
	resetMats(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations - 1; i++)
	{
		const unsigned int index = i % MaxDataElements;
		mats[index] = mats[index] * mats[index + 1];
	}

	return testStartTime.millisecondsSince();
}

float benchMatrixTrans(unsigned int iterations)
{
	resetMats(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations; i++)
	{
		const unsigned int index = i % MaxDataElements;
		mats[index] = mats[index].transpose();
	}

	return testStartTime.millisecondsSince();
}

float benchMatrixVecMult(unsigned int iterations)
{
	resetVecs(iterations);
	resetMats(iterations);

	testStartTime = nc::TimeStamp::now();
	for (unsigned int i = 0; i < iterations; i++)
	{
		const unsigned int index = i % MaxDataElements;
		vecsC[index] = mats[index] * vecsA[index];
	}

	return testStartTime.millisecondsSince();
}

nctl::String &indent(nctl::String &string, int amount)
{
	FATAL_ASSERT(amount >= 0);
	for (int i = 0; i < amount; i++)
		string.append("\t");

	return string;
}

bool loadTestRun(const char *filename, unsigned int index)
{
	nc::LuaStateManager luaState(
	    nc::LuaStateManager::ApiType::NONE,
	    nc::LuaStateManager::StatisticsTracking::DISABLED,
	    nc::LuaStateManager::StandardLibraries::NOT_LOADED);

#ifndef __EMSCRIPTEN__
	if (luaState.runFromFile(filename) == false)
#else
	if (luaState.runFromMemory(localFileLoad.filename(), localFileLoad.data(), localFileLoad.size()) == false)
#endif
	{
		LOGW_X("Cannot run script \"%s\" for index %u", filename, index);
		return false;
	}
	lua_State *L = luaState.state();

	testRuns[index].filename = filename;
	nc::LuaUtils::retrieveGlobalTable(L, Names::testSet);
	testRuns[index].system = nc::LuaUtils::retrieveField<const char *>(L, -1, Names::testSystem);

	const unsigned int numTests = nc::LuaUtils::rawLen(L, -1);
	for (unsigned int testIndex = 0; testIndex < numTests; testIndex++)
	{
		TestInfo &t = testRuns[index].testInfos[testIndex];
		nc::LuaUtils::rawGeti(L, -1, testIndex + 1); // Lua arrays start from index 1

		t.numIterations = nc::LuaUtils::retrieveField<uint32_t>(L, -1, Names::testIterations);
		t.name = nc::LuaUtils::retrieveField<const char *>(L, -1, Names::testName);

		nc::LuaUtils::retrieveFieldTable(L, -1, Names::testTimings);
		const unsigned int numReps = nc::LuaUtils::rawLen(L, -1);
		for (unsigned int repIndex = 0; repIndex < numReps; repIndex++)
		{
			nc::LuaUtils::rawGeti(L, -1, repIndex + 1);
			const float value = nc::LuaUtils::retrieve<float>(L, -1);
			t.stats.addValue(value);
			nc::LuaUtils::pop(L);
		}
		nc::LuaUtils::pop(L);

		nc::LuaUtils::pop(L);
		t.stats.calculateStats();
	}

	nc::LuaUtils::pop(L);

	LOGI_X("Loaded file \"%s\" for index %u", filename, index);
	return true;
}

void saveTestRun(const char *filename, bool includeStatistics)
{
	nctl::String file(8192);
	int amount = 0;

	indent(file, amount).formatAppend("%s = {\n", Names::testSet);

	amount++;
	indent(file, amount).formatAppend("%s = \"%s\",\n", Names::testSystem, system());

	for (unsigned int testIndex = 0; testIndex < Tests::Count; testIndex++)
	{
		if (testInfos[testIndex].stats.isEmpty())
			continue;

		const TestInfo &t = testInfos[testIndex];
		indent(file, amount).append("{\n");

		amount++;
		indent(file, amount).formatAppend("%s = \"%s\",\n", Names::testName, t.name.data());
		indent(file, amount).formatAppend("%s = %d,\n", Names::testIterations, t.numIterations);
		indent(file, amount).formatAppend("%s = {", Names::testTimings);
		for (unsigned int i = 0; i < t.stats.size(); i++)
			file.formatAppend(" %f%s", t.stats.value(i), (i < t.stats.size() - 1) ? "," : " ");
		file.formatAppend("}%s\n", includeStatistics ? "," : "");

		if (includeStatistics)
		{
			indent(file, amount).formatAppend("%s = %f,\n", Names::testTotalTime, t.stats.sum());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testAverage, t.stats.mean());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMedian, t.stats.median());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMode, t.stats.mode());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testPercentile75, t.stats.percentile(0.75f));
			indent(file, amount).formatAppend("%s = %f,\n", Names::testPercentile90, t.stats.percentile(0.9f));
			indent(file, amount).formatAppend("%s = %f,\n", Names::testStdDeviation, t.stats.sigma());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testRelStdDeviation, t.stats.relativeSigma());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMinTime, t.stats.minimum());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMaxTime, t.stats.maximum());
			indent(file, amount).formatAppend("%s = %f,\n", Names::testRangeTime, t.stats.range());
		}

		amount--;
		indent(file, amount).formatAppend("}%s\n", (testIndex < Tests::Count - 1) ? "," : "");
	}

	amount--;
	indent(file, amount).append("}\n");

#ifndef __EMSCRIPTEN__
	nctl::UniquePtr<nc::IFile> fileHandle = nc::IFile::createFileHandle(filename);
	fileHandle->open(nc::IFile::OpenMode::WRITE | nc::IFile::OpenMode::BINARY);
	fileHandle->write(file.data(), file.length());
	fileHandle->close();
#else
	nc::EmscriptenLocalFile localFileSave;
	localFileSave.write(file.data(), file.length());
	localFileSave.save(filename);
#endif
	LOGI_X("Saved file \"%s\"", filename);
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);

	config.withAudio = false;
	config.withJobSystem = false;
}

void MyEventHandler::onInit()
{
#ifdef __ANDROID__
	const float scalingFactor = nc::theApplication().gfxDevice().windowScalingFactor();
	ImGuiStyle &style = ImGui::GetStyle();
	style.FontScaleMain = scalingFactor;
	style.ScaleAllSizes(scalingFactor);
#endif

	nums = nctl::makeUnique<float[]>(MaxDataElements);
	vecsA = nctl::makeUnique<nc::Vector4f[]>(MaxDataElements);
	vecsB = nctl::makeUnique<nc::Vector4f[]>(MaxDataElements);
	vecsC = nctl::makeUnique<nc::Vector4f[]>(MaxDataElements);
	quats = nctl::makeUnique<nc::Quaternionf[]>(MaxDataElements);
	mats = nctl::makeUnique<nc::Matrix4x4f[]>(MaxDataElements);

	testInfos[Tests::Vector4Add].func = benchVector4Add;
	testInfos[Tests::Vector4Add].name = "Vector4 Add";
	testInfos[Tests::Vector4Sub].func = benchVector4Sub;
	testInfos[Tests::Vector4Sub].name = "Vector4 Sub";
	testInfos[Tests::Vector4Mul].func = benchVector4Mul;
	testInfos[Tests::Vector4Mul].name = "Vector4 Mul";
	testInfos[Tests::Vector4Div].func = benchVector4Div;
	testInfos[Tests::Vector4Div].name = "Vector4 Div";

	testInfos[Tests::Vector4Length].func = benchVector4Length;
	testInfos[Tests::Vector4Length].name = "Vector4 Length";
	testInfos[Tests::Vector4SqrLength].func = benchVector4SqrLength;
	testInfos[Tests::Vector4SqrLength].name = "Vector4 SqrLength";
	testInfos[Tests::Vector4Normalize].func = benchVector4Normalize;
	testInfos[Tests::Vector4Normalize].name = "Vector4 Normalize";
	testInfos[Tests::Vector4Dot].func = benchVector4Dot;
	testInfos[Tests::Vector4Dot].name = "Vector4 Dot";

	testInfos[Tests::QuaternionMult].func = benchQuaternionMult;
	testInfos[Tests::QuaternionMult].name = "Quaternion Mul";

	testInfos[Tests::MatrixMult].func = benchMatrixMult;
	testInfos[Tests::MatrixMult].name = "Matrix Mul";
	testInfos[Tests::MatrixTrans].func = benchMatrixTrans;
	testInfos[Tests::MatrixTrans].name = "Matrix Trans";
	testInfos[Tests::MatrixVecMult].func = benchMatrixVecMult;
	testInfos[Tests::MatrixVecMult].name = "MatrixVec Mult";

	for (unsigned int i = 0; i < Tests::Count; i++)
		testNames[i] = testInfos[i].name.data();

#ifdef __EMSCRIPTEN__
	localFileLoad.setLoadedCallback([](const nc::EmscriptenLocalFile &localFile, void *userData) {
		if (localFile.size() > 0)
			loadTestRun(localFile.filename(), currentTestRun);
	});
#endif
}

void MyEventHandler::onFrameStart()
{
#ifdef __ANDROID__
	const float scalingFactor = nc::theApplication().gfxDevice().windowScalingFactor();
#else
	const float scalingFactor = 1.0f;
#endif

	if (runningAllTests)
	{
		if (allTestsIndex >= Tests::Count)
		{
			allTestsIndex = 0;
			runningAllTests = false;
		}
		else
		{
			runTest(testInfos[allTestsIndex], numRepetitions, numIterations);
			allTestsIndex++;
		}
	}

	TestInfo &t = testInfos[currentTest];

	const ImVec2 windowPos(ImGui::GetMainViewport()->Size.x * 0.5f, ImGui::GetMainViewport()->Size.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400.0f * scalingFactor, 560.0f * scalingFactor), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("apptest_simdbench"))
	{
		if (ImGui::CollapsingHeader("Load Test Runs"))
		{
			if (ImGui::ArrowButton("##LeftTestRun", ImGuiDir_Left))
				currentTestRun--;
			ImGui::SameLine();
			if (ImGui::ArrowButton("##RightTestRun", ImGuiDir_Right))
				currentTestRun++;
			if (currentTestRun < 0)
				currentTestRun = 0;
			else if (currentTestRun > static_cast<int>(MaxTestRuns - 1))
				currentTestRun = MaxTestRuns - 1;
			ImGui::SameLine();
			ImGui::Text("Index: %d", currentTestRun);

#ifndef __EMSCRIPTEN__
			ImGui::InputText("##Loading", loadingFilename, MaxStringLength);
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				nctl::String filepath =  nc::fs::joinPath(nc::fs::savePath(), SaveDirectory);
				filepath =  nc::fs::joinPath(filepath, loadingFilename);
				if (nc::fs::isReadableFile(filepath.data()))
					loadTestRun(filepath.data(), currentTestRun);
				else
					LOGW_X("Cannot load file \"%s\" for index %u", filepath.data(), currentTestRun);
			}
#else
			if (localFileLoad.isLoading())
				ImGui::TextUnformatted("Loading file...");
			else
			{
				if (ImGui::Button("Load"))
					localFileLoad.load(".lua");
			}
#endif

			const TestRun &tr = testRuns[currentTestRun];
			const bool notLoaded = tr.filename.isEmpty();
			ImGui::Text("Filename: %s", notLoaded ? "N/A" : tr.filename.data());
			ImGui::Text("System: %s", notLoaded ? "N/A" : tr.system.data());
		}

		if (ImGui::CollapsingHeader("Test Run", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("System: %s", system());
			ImGui::Combo("Test", &currentTest, testNames, Tests::Count);
			int thousandIterations = numIterations / 1000;
			ImGui::SliderInt("Iterations", &thousandIterations, 1, MaxIterations / 1000, "%d K");
			numIterations = thousandIterations * 1000;
			ImGui::SliderInt("Repetitions", &numRepetitions, 1, MaxRepetitions);

			if (runningAllTests)
			{
				const float progress = (allTestsIndex + 1) / static_cast<float>(Tests::Count);
				progressMsg.format("%u / %u", allTestsIndex + 1, Tests::Count);
				ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), progressMsg.data());
				ImGui::NewLine();
			}
			else
			{
				if (ImGui::Button("Run"))
					runTest(t, numRepetitions, numIterations);
				ImGui::SameLine();
				if (ImGui::Button("Run All"))
					runningAllTests = true;

				ImGui::NewLine();
				ImGui::InputText("##Saving", savingFilename, MaxStringLength);
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					nctl::String filepath =  nc::fs::joinPath(nc::fs::savePath(), SaveDirectory);
					if (nc::fs::isDirectory(filepath.data()) == false)
						nc::fs::createDir(filepath.data());
					filepath =  nc::fs::joinPath(filepath, savingFilename);
					saveTestRun(filepath.data(), includeStatsWhenSaving);
				}
				ImGui::Checkbox("Include Statistics", &includeStatsWhenSaving);
			}
		}

		if (ImGui::CollapsingHeader("Results", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::ArrowButton("##LeftTest", ImGuiDir_Left))
				currentTest--;
			ImGui::SameLine();
			if (ImGui::ArrowButton("##RightTest", ImGuiDir_Right))
				currentTest++;
			if (currentTest < 0)
				currentTest = 0;
			else if (currentTest > Tests::Count - 1)
				currentTest = Tests::Count - 1;
			ImGui::SameLine();
			ImGui::Text("Test: %s", testNames[currentTest]);

			const TestRun &tr = testRuns[currentTestRun];
			const TestInfo &tri = tr.testInfos[currentTest];
			const bool canCompare = (tri.stats.sum() > 0.0f && t.stats.sum() > 0.0f);

			ImGui::Text("Iterations: %u", t.numIterations);
			if (canCompare && tri.numIterations != t.numIterations)
			{
				ImGui::SameLine();

				if (t.numIterations != 0)
					ImGui::TextColored(Red, "(Comparing with %u iterations)", tri.numIterations);
				else
					ImGui::Text("(Test run loaded with %u iterations)", tri.numIterations);
			}

			ImGui::Text("Total Time: %.3f ms", t.stats.sum());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.sum() > t.stats.sum()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.sum(), t.stats.sum() / tri.stats.sum());
			}
			static bool sorted = false;
			ImGui::PlotHistogram("Times", t.stats.values(sorted), t.stats.size(), 0, nullptr, 0.0f, t.stats.maximum() * 1.1f, ImVec2(0.0f, 50.0f));
			ImGui::SameLine();
			ImGui::Checkbox("Sorted", &sorted);
			ImGui::Text("Average: %.3f ms", t.stats.mean());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.mean() > t.stats.mean()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.mean(), t.stats.mean() / tri.stats.mean());
			}
			ImGui::Text("Median: %.3f ms", t.stats.mean());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.median() > t.stats.median()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.median(), t.stats.median() / tri.stats.median());
			}
			ImGui::Text("Mode: %.3f ms", t.stats.mode());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.mode() > t.stats.mode()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.mode(), t.stats.mode() / tri.stats.mode());
			}
			ImGui::Text("P75: %.3f ms", t.stats.percentile(0.75f));
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.percentile(0.75f) > t.stats.percentile(0.75f)) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.percentile(0.75f), t.stats.percentile(0.75f) / tri.stats.percentile(0.75f));
			}
			ImGui::Text("P90: %.3f ms", t.stats.percentile(0.9f));
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.percentile(0.9f) > t.stats.percentile(0.9f)) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.percentile(0.9f), t.stats.percentile(0.9f) / tri.stats.percentile(0.9f));
			}
			ImGui::Text("Std. Deviation: %.3f ms", t.stats.sigma());
			if (canCompare)
			{
				ImGui::SameLine();
				ImGui::Text("(%.3f ms)", tri.stats.sigma());
			}
			ImGui::Text("Relative S.D.: %.2f %%", t.stats.relativeSigma());
			if (canCompare)
			{
				ImGui::SameLine();
				ImGui::Text("(%.2f %%)", tri.stats.relativeSigma());
			}
			ImGui::Text("Min Time: %.3f ms", t.stats.minimum());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.minimum() > t.stats.minimum()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.minimum(), t.stats.minimum() / tri.stats.minimum());
			}
			ImGui::Text("Max Time: %.3f ms", t.stats.maximum());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.maximum() > t.stats.maximum()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.maximum(), t.stats.maximum() / tri.stats.maximum());
			}
			ImGui::Text("Range Time: %.3f ms", t.stats.range());
			if (canCompare)
			{
				ImGui::SameLine();
				const ImVec4 color = (tri.stats.range() > t.stats.range()) ? Green : Red;
				ImGui::TextColored(color, "(%.3f ms, %.2fx)", tri.stats.range(), t.stats.range() / tri.stats.range());
			}
		}
	}

	ImGui::End();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}
