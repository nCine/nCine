#include <ncine/imgui.h>

#include "apptest_simdbench.h"
#include <ncine/Application.h>
#include <ncine/LuaStateManager.h>
#include <ncine/LuaUtils.h>
#include <ncine/IFile.h>
#include <ncine/TimeStamp.h>
#include <ncine/Vector4.h>
#include <ncine/Quaternion.h>
#include "apptest_datapath.h"

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
const char *testMaxTime = "max_time";
const char *testMinTime = "min_time";
const char *testAverage = "average";
const char *testStdDeviation = "standard_deviation";
const char *testRelStdDeviation = "relative_standard_deviation";

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
	    : name(nctl::String(128)), func(nullptr), totalTime(0.0f),
	      maxTime(0.0f), minTime(0.0f), average(0.0f), stdDeviation(0.0f),
	      numRepetitions(0), numIterations(0) {}

	nctl::String name;
	TestFunction func;
	float times[MaxRepetitions];
	float totalTime;
	float maxTime;
	float minTime;
	float average;
	float stdDeviation;
	unsigned int numRepetitions;
	unsigned int numIterations;
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

void calculateStats(TestInfo &t)
{
	t.maxTime = t.times[0];
	t.minTime = t.times[0];
	for (unsigned int i = 1; i < t.numRepetitions; i++)
	{
		if (t.times[i] > t.maxTime)
			t.maxTime = t.times[i];
		else if (t.times[i] < t.minTime)
			t.minTime = t.times[i];
	}

	t.totalTime = 0.0f;
	for (unsigned int i = 0; i < t.numRepetitions; i++)
		t.totalTime += t.times[i];
	t.average = t.totalTime / static_cast<float>(t.numRepetitions);

	t.stdDeviation = 0.0f;
	if (t.numRepetitions > 1)
	{
		for (unsigned int i = 0; i < t.numRepetitions; i++)
			t.stdDeviation += (t.times[i] - t.average) * (t.times[i] - t.average);
		t.stdDeviation /= static_cast<float>(t.numRepetitions - 1);
		t.stdDeviation = sqrtf(t.stdDeviation);
	}
}

void runTest(TestInfo &t, unsigned int numRepetitions, unsigned int numIterations)
{
	ASSERT(t.numRepetitions > 0);
	ASSERT(t.numIterations > 0);
	FATAL_ASSERT(t.func != nullptr);

	t.numRepetitions = numRepetitions;
	t.numIterations = numIterations;
	for (unsigned int i = 0; i < t.numRepetitions; i++)
		t.times[i] = t.func(t.numIterations) * 1000;

	calculateStats(t);
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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

	return testStartTime.secondsSince();
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
	if (luaState.run(filename) == false)
#else
	if (luaState.runFromMemory(localFileLoad.data(), localFileLoad.size(), localFileLoad.filename()) == false)
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
		t.numRepetitions = nc::LuaUtils::rawLen(L, -1);
		for (unsigned int repIndex = 0; repIndex < t.numRepetitions; repIndex++)
		{
			nc::LuaUtils::rawGeti(L, -1, repIndex + 1);
			t.times[repIndex] = nc::LuaUtils::retrieve<float>(L, -1);
			nc::LuaUtils::pop(L);
		}
		nc::LuaUtils::pop(L);

		nc::LuaUtils::pop(L);
		calculateStats(t);
	}

	nc::LuaUtils::pop(L);

	return true;
}

void saveTestRun(const char *filename, bool includeStatistics)
{
	nc::LuaStateManager luaState(
	    nc::LuaStateManager::ApiType::NONE,
	    nc::LuaStateManager::StatisticsTracking::DISABLED,
	    nc::LuaStateManager::StandardLibraries::NOT_LOADED);

	nctl::String file(8192);
	int amount = 0;

	indent(file, amount).formatAppend("%s = {\n", Names::testSet);

	amount++;
	indent(file, amount).formatAppend("%s = \"%s\",\n", Names::testSystem, system());

	for (unsigned int testIndex = 0; testIndex < Tests::Count; testIndex++)
	{
		if (testInfos[testIndex].numRepetitions == 0)
			continue;

		const TestInfo &t = testInfos[testIndex];
		indent(file, amount).append("{\n");

		amount++;
		indent(file, amount).formatAppend("%s = \"%s\",\n", Names::testName, t.name.data());
		indent(file, amount).formatAppend("%s = %d,\n", Names::testIterations, t.numIterations);
		indent(file, amount).formatAppend("%s = {", Names::testTimings);
		for (unsigned int i = 0; i < t.numRepetitions; i++)
			file.formatAppend(" %f%s", t.times[i], (i < t.numRepetitions - 1) ? "," : " ");
		file.formatAppend("}%s\n", includeStatistics ? "," : "");

		if (includeStatistics)
		{
			indent(file, amount).formatAppend("%s = %f,\n", Names::testTotalTime, t.totalTime);
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMaxTime, t.maxTime);
			indent(file, amount).formatAppend("%s = %f,\n", Names::testMinTime, t.minTime);
			indent(file, amount).formatAppend("%s = %f,\n", Names::testAverage, t.average);
			indent(file, amount).formatAppend("%s = %f,\n", Names::testStdDeviation, t.stdDeviation);
			indent(file, amount).formatAppend("%s = %f\n", Names::testRelStdDeviation, t.average > 0.0f ? 100.0f * t.stdDeviation / t.average : 0.0f);
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
	config.withDebugOverlay = false;
	config.withThreads = false;
}

void MyEventHandler::onInit()
{
#ifdef __ANDROID__
	ImGuiIO &io = ImGui::GetIO();
	io.FontGlobalScale = 3.0f;
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

	ImGui::Begin("Benchmark");
	if (ImGui::CollapsingHeader("Load Test Runs"))
	{
		if (ImGui::ArrowButton("##LeftTestRun", ImGuiDir_Left))
			currentTestRun--;
		ImGui::SameLine();
		if (ImGui::ArrowButton("##RightTestRun", ImGuiDir_Right))
			currentTestRun++;
		if (currentTestRun < 0)
			currentTestRun = 0;
		else if (currentTestRun > MaxTestRuns - 1)
			currentTestRun = MaxTestRuns - 1;
		ImGui::SameLine();
		ImGui::Text("Index: %d", currentTestRun);

#ifndef __EMSCRIPTEN__
		ImGui::InputText("##Loading", loadingFilename, MaxStringLength);
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			nctl::String filepath = nc::fs::joinPath(nc::fs::dataPath(), loadingFilename);
			if (nc::fs::isReadableFile(filepath.data()))
				loadTestRun(filepath.data(), currentTestRun);
			else
				LOGW_X("Cannot load file \"%s\" for index %u", filepath.data(), currentTestRun);
		}
#else
		if (localFileLoad.isLoading())
			ImGui::Text("Loading file...");
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
				nctl::String filepath = nc::fs::joinPath(nc::fs::dataPath(), savingFilename);
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
		const bool canCompare = (tri.totalTime > 0.0f && t.totalTime > 0.0f);

		ImGui::Text("Iterations: %u", t.numIterations);
		if (canCompare && tri.numIterations != t.numIterations)
		{
			ImGui::SameLine();

			if (t.numIterations != 0)
				ImGui::TextColored(Red, "(Comparing with %u iterations)", tri.numIterations);
			else
				ImGui::Text("(Test run loaded with %u iterations)", tri.numIterations);
		}

		ImGui::Text("Total Time: %f ms", t.totalTime);
		if (canCompare)
		{
			ImGui::SameLine();
			const ImVec4 color = (tri.totalTime > t.totalTime) ? Green : Red;
			ImGui::TextColored(color, "(%f ms, %.2fx)", tri.totalTime, t.totalTime / tri.totalTime);
		}
		ImGui::PlotHistogram("Times", t.times, t.numRepetitions, 0, nullptr, 0.0f, FLT_MAX, ImVec2(0.0f, 100.0f));
		ImGui::Text("Max Time: %f ms", t.maxTime);
		if (canCompare)
		{
			ImGui::SameLine();
			const ImVec4 color = (tri.maxTime > t.maxTime) ? Green : Red;
			ImGui::TextColored(color, "(%f ms, %.2fx)", tri.maxTime, t.maxTime / tri.maxTime);
		}
		ImGui::Text("Min Time: %f ms", t.minTime);
		if (canCompare)
		{
			ImGui::SameLine();
			const ImVec4 color = (tri.minTime > t.minTime) ? Green : Red;
			ImGui::TextColored(color, "(%f ms, %.2fx)", tri.minTime, t.minTime / tri.minTime);
		}
		ImGui::Text("Average: %f ms", t.average);
		if (canCompare)
		{
			ImGui::SameLine();
			const ImVec4 color = (tri.average > t.average) ? Green : Red;
			ImGui::TextColored(color, "(%f ms, %.2fx)", tri.average, t.average / tri.average);
		}
		ImGui::Text("Std. Deviation: %f ms", t.stdDeviation);
		if (canCompare)
		{
			ImGui::SameLine();
			ImGui::Text("(%f ms)", tri.stdDeviation);
		}
		ImGui::Text("Relative S.D.: %.2f %%", t.average > 0.0f ? 100.0f * t.stdDeviation / t.average : 0.0f);
		if (canCompare)
		{
			ImGui::SameLine();
			ImGui::Text("(%.2f %%)", tri.average > 0.0f ? 100.0f * tri.stdDeviation / tri.average : 0.0f);
		}
	}

	ImGui::End();
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
}
