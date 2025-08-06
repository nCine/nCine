#include <ncine/imgui.h>

#include "apptest_jobsystem.h"
#include <ncine/IJobSystem.h>
#include <ncine/ParallelForJob.h>
#include <ncine/JobStatistics.h>
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/Timer.h>
#include <ncine/TimeStamp.h>
#include <nctl/Array.h>
#include <nctl/StaticString.h>
#include "Statistics.h"

#include <ncine/tracy.h>

namespace {

int dummyIterations = 1024;
int numSpawnedJobs = 4;
int numRepetitions = 16;
nctl::StaticString<128> auxString;
bool autoResetStatistics = true;
bool totalIterationsLocked = false;
unsigned int totalIterations = 0;
int currentJobType = 1;
const char *jobTypeComboString = "Empty\0Dummy\0FastDummy\0SpawnChildren\0\0";
int numJobsToQueue = 256;
int dataArraySize = 16 * 1024;
int countSplitterValue = 128;
int childrenCount = 10;
bool showImGui = true;

// ----- Statistics -----
nc::TimeStamp totalTimestamp;
nc::TimeStamp partialTimestamp;

const unsigned int MaxNumRepetitions = 64;
Statistics submitStats(MaxNumRepetitions);
Statistics waitStats(MaxNumRepetitions);
Statistics totalStats(MaxNumRepetitions);

// ----- WorkerStatistics -----

class WorkerStatistics
{
  public:
	explicit WorkerStatistics(unsigned int capacity)
	    : statistics_(capacity)
	{
		clear();
	}

	void clear()
	{
		valueWriteIndex = 0;
		statistics_.clearValues();
		statistics_.resetStats();
		totalFinishedJobs_ = 0;
	}

	void start()
	{
		timestamp_.toNow();
	}

	void end()
	{
		statistics_.addValueWrap(timestamp_.millisecondsSince());
		totalFinishedJobs_++;
	}

	const Statistics &statistics() const { return statistics_; }
	unsigned int totalFinishedJobs() const { return totalFinishedJobs_; }

	void calculateStats()
	{
		statistics_.calculateStats();
	}

  private:
	unsigned int valueWriteIndex = 0;
	nc::TimeStamp timestamp_;
	Statistics statistics_;
	unsigned int totalFinishedJobs_ = 0;
};

const unsigned int MaxNumJobTimings = 128;
nctl::Array<WorkerStatistics> workerStats(MaxNumJobTimings);

// ----- JobSystemStatistics -----

void resetJobSystemStatistics()
{
	if (nc::theJobStatistics().isAvailable())
	{
		nc::theJobStatistics().resetAllJobSystemStats();
		nc::theJobStatistics().resetAllJobPoolStats();
		nc::theJobStatistics().resetAllJobQueueStats();
	}
}

void guiJobSystemStatsTable(const nc::JobStatistics::JobSystemStats &systemStats, const char *tableName)
{
	if (ImGui::BeginTable(tableName, 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
	                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Counter", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs created");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.jobsCreated);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Child jobs created");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.childJobsCreated);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs stolen");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.jobsStolen);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs executed");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.jobsExecuted);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs finished");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.jobsFinished);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Child jobs finished");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.childJobsFinished);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Continuation jobs pushed");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.continuationJobsPushed);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Parent jobs finished");
		ImGui::TableNextColumn(); ImGui::Text("%u", systemStats.parentJobsFinished);

		ImGui::EndTable();
	}
}

void guiJobPoolStatsTable(const nc::JobStatistics::JobPoolStats &poolStats, const char *tableName)
{
	if (ImGui::BeginTable(tableName, 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg |
	                      ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Counter", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs allocated");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.jobsAllocated);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs failed to be allocated");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.jobAllocationFails);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs freed");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.jobsFreed);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs retrieved");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.jobsRetrieved);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs failed to be retrieved");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.jobRetrievalFails);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Thread cache found empty");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.threadCacheEmpty);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Thread cache found full");
		ImGui::TableNextColumn(); ImGui::Text("%u", poolStats.threadCacheFull);

		ImGui::EndTable();
	}
}

void guiJobQueueStatsTable(const nc::JobStatistics::JobQueueStats &queueStats, const char *tableName)
{
	if (ImGui::BeginTable(tableName, 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
	                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Counter", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs pushed");
		ImGui::TableNextColumn(); ImGui::Text("%u", queueStats.pushes);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs popped");
		ImGui::TableNextColumn(); ImGui::Text("%u", queueStats.pops);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs failed to be popped");
		ImGui::TableNextColumn(); ImGui::Text("%u", queueStats.popFails);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs steals");
		ImGui::TableNextColumn(); ImGui::Text("%u", queueStats.steals);
		ImGui::TableNextColumn(); ImGui::TextUnformatted("Jobs failed to be stolen");
		ImGui::TableNextColumn(); ImGui::Text("%u", queueStats.stealFails);

		ImGui::EndTable();
	}
}

// ----- Helper statistics functions -----

void calculateStatistics()
{
	submitStats.calculateStats();
	waitStats.calculateStats();
	totalStats.calculateStats();

	const unsigned char numThreads = nc::theServiceLocator().jobSystem().numThreads();
	for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
		workerStats[threadIndex].calculateStats();
}

void resetTimingStatistics()
{
	submitStats.clearValues();
	submitStats.resetStats();
	waitStats.clearValues();
	waitStats.resetStats();
	totalStats.clearValues();
	totalStats.resetStats();
}

void resetStatistics()
{
	resetJobSystemStatistics();

	resetTimingStatistics();
	const unsigned char numThreads = nc::theServiceLocator().jobSystem().numThreads();
	for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
		workerStats[threadIndex].clear();
}

// ----- Jobs -----

static volatile unsigned int dummySink = 0;

void emptyJobWithStatistics(nc::JobId, const void *)
{
	ZoneScoped;
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats[threadIndex].start();
	workerStats[threadIndex].end();
}

void dummyJob(nc::JobId, const void *)
{
	ZoneScoped;
	static const unsigned int BUFFER_SIZE = 8192;
	static thread_local unsigned int buffer[BUFFER_SIZE];
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats[threadIndex].start();

	unsigned int accumulator = threadIndex + 1;
	for (unsigned int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = (i + threadIndex * 17) ^ 0xA5A5A5A5;

	for (int i = 0; i <= dummyIterations; i++)
	{
		const unsigned int k = i * i;
		const unsigned int q = (i + k) / (i | 1);
		const unsigned int l = (k * i) / (q | 1);
		const unsigned int w = (k / (l | 1)) * q;

		const float f = sinf(float(i) * 0.1f) * cosf(float(w) * 0.001f);
		const float g = sqrtf(fabsf(f) + 1.0f);

		const size_t index = (w + static_cast<unsigned int>(g * 1000.0f)) % BUFFER_SIZE;
		buffer[index] ^= accumulator;

		accumulator += w + static_cast<unsigned int>(g * 10.0f);
	}

	dummySink = accumulator;
	workerStats[threadIndex].end();
}

void fastDummyJob(nc::JobId, const void *)
{
	ZoneScoped;
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats[threadIndex].start();

	unsigned int accumulator = threadIndex + 1;
	for (int i = 0; i <= dummyIterations; i++)
	{
		const unsigned int k = i * i;
		const unsigned int q = (i + k) / (i | 1);
		const unsigned int l = (k * i) / (q | 1);
		const unsigned int w = (k / (l | 1)) * q;

		accumulator += w + static_cast<unsigned int>(w * 10.0f);
	}

	dummySink = accumulator;
	workerStats[threadIndex].end();
}

void spawnChildrenJob(nc::JobId jobId, const void *)
{
	ZoneScoped;
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats[threadIndex].start();
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();
	for (int i = 0; i < numSpawnedJobs; i++)
	{
		nc::JobId childJobId = jobSystem.createJobAsChild(jobId, fastDummyJob);
		jobSystem.submit(childJobId);
	}
	workerStats[threadIndex].end();
}

struct MyData
{
	uint32_t icalc0 = 0;
	uint32_t icalc1 = 0;
	float fcalc0 = 0.0f;
	float fcalc1 = 0.0f;
};

const unsigned int MaxDataArraySize = 64 * 1024;
MyData dataArray[MaxDataArraySize];

void myDataFunc(MyData *data, unsigned int count)
{
	ZoneScoped;
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats[threadIndex].start();
	for (unsigned int i = 0; i < count; i++)
	{
		const unsigned int baseCalc0 = (i + threadIndex) * (i ^ 0x5A5A5A5A);
		const unsigned int baseCalc1 = (baseCalc0 >> 3) + (i * 31);

		float fval0 = 0.0f;
		float fval1 = 0.0f;
		for (int j = 0; j < dummyIterations; j++)
		{
			const float f0 = sinf(float(j) * 0.07f) * 0.5f;
			fval0 += (baseCalc0 / (0.05f + f0)) + (baseCalc1 / (0.08f + f0));

			const float f1 = float((baseCalc0 ^ j) & 0xFF) / 255.0f;
			fval1 += f1 * (baseCalc1 * 0.01f);
		}

		data[i].icalc0 = baseCalc0;
		data[i].icalc1 = baseCalc1;
		data[i].fcalc0 = fval0;
		data[i].fcalc1 = fval1;
	}
	workerStats[threadIndex].end();
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.withJobSystem = true;
	config.numThreads = 0;
}

void MyEventHandler::onInit()
{
#ifdef __ANDROID__
	const float scalingFactor = nc::theApplication().gfxDevice().windowScalingFactor();
	ImGuiStyle &style = ImGui::GetStyle();
	style.FontScaleMain = scalingFactor;
	style.ScaleAllSizes(scalingFactor);
#endif

	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();
	for (unsigned char i = 0; i < jobSystem.numThreads(); i++)
		workerStats.emplaceBack(256);
}

void MyEventHandler::onFrameStart()
{
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();
	const unsigned char numThreads = jobSystem.numThreads();

	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(640.0f, 680.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_jobsystem", &showImGui))
		{
			ImGui::Text("Number of threads: %u (main is #%u)", numThreads, jobSystem.threadIndex());
			ImGui::SameLine();
			ImGui::Checkbox("Auto-reset stats", &autoResetStatistics);
			ImGui::SliderInt("Num repetitions", &numRepetitions, 1, totalStats.capacity(), "%d", ImGuiSliderFlags_AlwaysClamp);

			nc::JobFunction jobFn = &emptyJobWithStatistics;
			ImGui::Combo("Job's type", &currentJobType, jobTypeComboString);
			switch (currentJobType)
			{
				case 0: jobFn = emptyJobWithStatistics; break;
				case 1: jobFn = dummyJob; break;
				case 2: jobFn = fastDummyJob; break;
				case 3: jobFn = spawnChildrenJob; break;
				default: jobFn = emptyJobWithStatistics; break;
			}
			const bool numIterationsChanged = ImGui::SliderInt("Jobs iterations", &dummyIterations, 0, 4096);
			if (dummyIterations <= 0)
				dummyIterations = 1;
			if (totalIterationsLocked && numIterationsChanged)
				numJobsToQueue = totalIterations / dummyIterations;

			const bool numJobsChanged = ImGui::SliderInt("Number of jobs", &numJobsToQueue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (totalIterationsLocked && numJobsChanged)
				dummyIterations = totalIterations / numJobsToQueue;

			ImGui::BeginDisabled(jobFn != spawnChildrenJob);
			ImGui::SliderInt("Number of spawned jobs", &numSpawnedJobs, 0, 16, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndDisabled();
			ImGui::Text("Total iterations: %u", dummyIterations * numJobsToQueue);
			ImGui::SameLine();
			if (ImGui::Checkbox("Locked", &totalIterationsLocked) && totalIterationsLocked)
				totalIterations = dummyIterations * numJobsToQueue;

			if (ImGui::Button("Enqueue jobs"))
			{
				ZoneScopedN("Enqueue Jobs");
				if (autoResetStatistics)
					resetStatistics();

				for (int rep = 0; rep < numRepetitions; rep++)
				{
					totalTimestamp = nc::TimeStamp::now();

					nc::JobId rootJobId = jobSystem.createJob(nullptr);
					{
						ZoneScopedN("Job Submission");
						for (int i = 0; i < numJobsToQueue; i++)
						{
							nc::JobId jobId = jobSystem.createJobAsChild(rootJobId, jobFn);
							jobSystem.submit(jobId);
						}
					}
					jobSystem.submit(rootJobId);
					submitStats.addValueWrap(totalTimestamp.millisecondsSince());

					{
						ZoneScopedN("Job Wait");
						partialTimestamp = nc::TimeStamp::now();
						jobSystem.wait(rootJobId);
						waitStats.addValueWrap(partialTimestamp.millisecondsSince());
					}

					totalStats.addValueWrap(totalTimestamp.millisecondsSince());
				}
				calculateStatistics();

#ifdef NCINE_WITH_TRACY
				auxString.format("Repetitions: %d, Jobs: %d", numRepetitions, numJobsToQueue);
				ZoneText(auxString.data(), auxString.length());
				if (jobFn == dummyJob || jobFn == fastDummyJob)
				{
					auxString.format("Iterations: %d (Total: %d)", dummyIterations, numJobsToQueue * dummyIterations);
					ZoneText(auxString.data(), auxString.length());
				}
				auxString.format("\nTotal Time: %.3f ms (%.2f%%)\nSubmit Time: %.3f ms (%.2f%%)\nWait Time: %.3f ms (%.2f%%)",
				                 totalStats.mean(), totalStats.relativeSigma(), submitStats.mean(), submitStats.relativeSigma(), waitStats.mean(), waitStats.relativeSigma());
				ZoneText(auxString.data(), auxString.length());
#endif
			}

			ImGui::SameLine();
			if (ImGui::Button("Continuations"))
			{
				ZoneScopedN("Continuations");
				if (autoResetStatistics)
					resetStatistics();

				// Avoid an infinite spawning of jobs from continuations
				const nc::JobFunction contFn = (jobFn != spawnChildrenJob) ? jobFn : emptyJobWithStatistics;

				for (int rep = 0; rep < numRepetitions; rep++)
				{
					totalTimestamp = nc::TimeStamp::now();

					nc::JobId rootJobId = jobSystem.createJob(nullptr);
					{
						ZoneScopedN("Job Submission");
						for (int i = 0; i < numJobsToQueue; i++)
						{
							nc::JobId jobId = jobSystem.createJobAsChild(rootJobId, jobFn);
							for (unsigned int j = 0; j < nc::JobNumContinuations; j++)
							{
								nc::JobId contId = jobSystem.createJob(contFn);
								jobSystem.addContinuation(jobId, contId);
							}
							jobSystem.submit(jobId);
						}
					}
					jobSystem.submit(rootJobId);
					submitStats.addValueWrap(totalTimestamp.millisecondsSince());

					{
						ZoneScopedN("Job Wait");
						partialTimestamp = nc::TimeStamp::now();
						jobSystem.wait(rootJobId);
						waitStats.addValueWrap(partialTimestamp.millisecondsSince());
					}

					totalStats.addValueWrap(totalTimestamp.millisecondsSince());
				}
				calculateStatistics();

#ifdef NCINE_WITH_TRACY
				auxString.format("Repetitions: %d, Jobs: %d", numRepetitions, numJobsToQueue);
				ZoneText(auxString.data(), auxString.length());
				if (jobFn == dummyJob || jobFn == fastDummyJob)
				{
					auxString.format("Iterations: %d (Total: %d)", dummyIterations, numJobsToQueue * dummyIterations);
					ZoneText(auxString.data(), auxString.length());
				}
				auxString.format("\nTotal Time: %.3f ms (%.2f%%)\nSubmit Time: %.3f ms (%.2f%%)\nWait Time: %.3f ms (%.2f%%)",
				                 totalStats.mean(), totalStats.relativeSigma(), submitStats.mean(), submitStats.relativeSigma(), waitStats.mean(), waitStats.relativeSigma());
				ZoneText(auxString.data(), auxString.length());
#endif
			}

			ImGui::SliderInt("Data array size", &dataArraySize, 1, MaxDataArraySize, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderInt("Count splitter", &countSplitterValue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Parallel for"))
			{
				ZoneScopedN("Parallel For");
				if (autoResetStatistics)
					resetStatistics();

				for (int rep = 0; rep < numRepetitions; rep++)
				{
					nc::JobId rootJobId = nc::parallelFor(dataArray, dataArraySize, &myDataFunc, nc::CountSplitter(countSplitterValue));
					totalTimestamp = nc::TimeStamp::now();
					{
						ZoneScopedN("Job Submission");
						jobSystem.submit(rootJobId);
						submitStats.addValueWrap(totalTimestamp.millisecondsSince());
					}

					{
						ZoneScopedN("Job Wait");
						partialTimestamp = nc::TimeStamp::now();
						jobSystem.wait(rootJobId);
						waitStats.addValueWrap(partialTimestamp.millisecondsSince());
					}

					totalStats.addValueWrap(totalTimestamp.millisecondsSince());
				}
				calculateStatistics();

#ifdef NCINE_WITH_TRACY
				auxString.format("Repetitions: %d", numRepetitions);
				ZoneText(auxString.data(), auxString.length());
				auxString.format("Data Array Size: %d, Count Splitter: %d", dataArraySize, countSplitterValue);
				ZoneText(auxString.data(), auxString.length());
				auxString.format("\nTotal Time: %.3f ms (%.2f%%)\nSubmit Time: %.3f ms (%.2f%%)\nWait Time: %.3f ms (%.2f%%)",
				                 totalStats.mean(), totalStats.relativeSigma(), submitStats.mean(), submitStats.relativeSigma(), waitStats.mean(), waitStats.relativeSigma());
				ZoneText(auxString.data(), auxString.length());
#endif
			}

			ImGui::SliderInt("Children count", &childrenCount, 1, 32, "%d", ImGuiSliderFlags_AlwaysClamp);
			const unsigned int totalChildrenJobs = childrenCount * childrenCount * childrenCount;
			if (ImGui::Button("Parent / children"))
			{
				ZoneScopedN("Parent / Children");
				if (autoResetStatistics)
					resetStatistics();

				for (int rep = 0; rep < numRepetitions; rep++)
				{
					totalTimestamp = nc::TimeStamp::now();

					nc::JobId rootJobId = jobSystem.createJob(nullptr);
					{
						ZoneScopedN("Job Submission");
						for (int i = 0; i < childrenCount; i++)
						{
							nc::JobId grandparentJobId = jobSystem.createJobAsChild(rootJobId, nullptr);
							for (int j = 0; j < childrenCount; j++)
							{
								nc::JobId parentJobId = jobSystem.createJobAsChild(grandparentJobId, nullptr);
								for (int k = 0; k < childrenCount; k++)
								{
									nc::JobId jobId = jobSystem.createJobAsChild(parentJobId, jobFn);
									jobSystem.submit(jobId);
								}
								jobSystem.submit(parentJobId);
							}
							jobSystem.submit(grandparentJobId);
						}
					}
					jobSystem.submit(rootJobId);
					submitStats.addValueWrap(totalTimestamp.millisecondsSince());

					{
						ZoneScopedN("Job Wait");
						partialTimestamp = nc::TimeStamp::now();
						jobSystem.wait(rootJobId);
						waitStats.addValueWrap(partialTimestamp.millisecondsSince());
					}

					totalStats.addValueWrap(totalTimestamp.millisecondsSince());
				}
				calculateStatistics();

#ifdef NCINE_WITH_TRACY
				auxString.format("Repetitions: %d, Jobs: %d", numRepetitions, totalChildrenJobs);
				ZoneText(auxString.data(), auxString.length());
				if (jobFn == dummyJob || jobFn == fastDummyJob)
				{
					auxString.format("Iterations: %d (Total: %d)", dummyIterations, numJobsToQueue * dummyIterations);
					ZoneText(auxString.data(), auxString.length());
				}
				auxString.format("\nTotal Time: %.3f ms (%.2f%%)\nSubmit Time: %.3f ms (%.2f%%)\nWait Time: %.3f ms (%.2f%%)",
				                 totalStats.mean(), totalStats.relativeSigma(), submitStats.mean(), submitStats.relativeSigma(), waitStats.mean(), waitStats.relativeSigma());
				ZoneText(auxString.data(), auxString.length());
#endif
			}
			ImGui::SameLine();
			ImGui::Text("Total jobs: %u (%u x %u x %u)", totalChildrenJobs, childrenCount, childrenCount, childrenCount);

			if (ImGui::TreeNodeEx("Repetition timings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				static bool sorted = false;
				ImGui::PlotHistogram("Submit", submitStats.values(sorted), submitStats.size(), 0, nullptr, 0.0f, submitStats.maximum() * 1.1f);
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
					resetTimingStatistics();
				ImGui::SameLine();
				ImGui::Checkbox("Sorted", &sorted);
				ImGui::PlotHistogram("Wait", waitStats.values(sorted), waitStats.size(), 0, nullptr, 0.0f, waitStats.maximum() * 1.1f);
				ImGui::PlotHistogram("Total", totalStats.values(sorted), totalStats.size(), 0, nullptr, 0.0f, totalStats.maximum() * 1.1f);

				ImGui::NewLine();
				if (ImGui::BeginTable("RepetitionTimingsTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
				                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Submit");
					ImGui::TableSetupColumn("Wait");
					ImGui::TableSetupColumn("Total");
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Sum");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.sum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.sum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.sum());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mean");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.mean());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.mean());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.mean());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Median");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.median());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.median());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.median());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mode");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.mode());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.mode());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.mode());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P75");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.percentile(0.75f));
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.percentile(0.75f));
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.percentile(0.75f));
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P90");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.percentile(0.9f));
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.percentile(0.9f));
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.percentile(0.9f));
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Sigma");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.sigma());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.sigma());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.sigma());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Rel. Sigma");
					ImGui::TableNextColumn(); ImGui::Text("%.3f%%", submitStats.relativeSigma());
					ImGui::TableNextColumn(); ImGui::Text("%.3f%%", waitStats.relativeSigma());
					ImGui::TableNextColumn(); ImGui::Text("%.3f%%", totalStats.relativeSigma());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Min");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.minimum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.minimum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.minimum());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Max");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.maximum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.maximum());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.maximum());
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Range");
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", submitStats.range());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", waitStats.range());
					ImGui::TableNextColumn(); ImGui::Text("%.3f ms", totalStats.range());

					ImGui::EndTable();
				}

				if (ImGui::TreeNode("Values"))
				{
					if (ImGui::BeginTable("TimingValuesTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
					                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
					{
						ImGui::TableSetupScrollFreeze(0, 1);
						ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
						ImGui::TableSetupColumn("Submit");
						ImGui::TableSetupColumn("Wait");
						ImGui::TableSetupColumn("Total");
						ImGui::TableHeadersRow();

						ImGui::TableNextRow();
						for (unsigned int i = 0; i < totalStats.size(); i++)
						{
							ImGui::TableNextColumn(); ImGui::Text("#%u", i);
							ImGui::TableNextColumn(); ImGui::Text("%f ms", submitStats.values(sorted)[i]);
							ImGui::TableNextColumn(); ImGui::Text("%f ms", waitStats.values(sorted)[i]);
							ImGui::TableNextColumn(); ImGui::Text("%f ms", totalStats.values(sorted)[i]);
						}

						ImGui::EndTable();
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Per-thread job timings"))
			{
				static nctl::Array<float> totalFinishedJobs(numThreads);
				totalFinishedJobs.clear();
				for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					totalFinishedJobs.pushBack(static_cast<float>(workerStats[threadIndex].totalFinishedJobs()));

				static nctl::Array<float> timeAverages(numThreads);
				timeAverages.clear();
				for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					timeAverages.pushBack(workerStats[threadIndex].statistics().mean());

				float maxFinishedJobsScale = 0.0f;
				for (unsigned int i = 0; i < totalFinishedJobs.size(); i++)
				{
					if (totalFinishedJobs[i] > maxFinishedJobsScale)
						maxFinishedJobsScale = totalFinishedJobs[i];
				}
				maxFinishedJobsScale *= 1.1f;

				ImGui::PlotHistogram("Total finished jobs", totalFinishedJobs.data(), totalFinishedJobs.size(), 0, nullptr, 0.0f, maxFinishedJobsScale);
				if (ImGui::Button("Reset"))
				{
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
						workerStats[threadIndex].clear();
				}
				static bool sorted = false;
				ImGui::SameLine();
				ImGui::Checkbox("Sorted", &sorted);

				float maxTimeScale = 0;
				for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
				{
					if (timeAverages[threadIndex] > maxTimeScale)
						maxTimeScale = timeAverages[threadIndex];
				}
				maxTimeScale *= 1.1f;

				ImGui::NewLine();
				ImGui::PlotHistogram("Average timestamps", timeAverages.data(), numThreads, 0, nullptr, 0.0f, maxTimeScale);
				for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
				{
					auxString.format("Timestamps for T#%u", threadIndex);
					ImGui::PlotHistogram(auxString.data(), workerStats[threadIndex].statistics().values(sorted), workerStats[threadIndex].statistics().size(), 0, nullptr, 0.0f, maxTimeScale);
				}

				ImGui::NewLine();
				if (ImGui::BeginTable("JobTimingsTable", numThreads + 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
				                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						auxString.format("Th#%.2d", threadIndex);
						ImGui::TableSetupColumn(auxString.data());
					}
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Sum");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().sum());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mean");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().mean());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Median");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().median());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Mode");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().mode());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P75");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().percentile(0.75f));
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("P90");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().percentile(0.9f));
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Sigma");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().sigma());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Rel. Sigma");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f%%", workerStats[threadIndex].statistics().relativeSigma());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Min");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().minimum());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Max");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().maximum());
					}
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Range");
					for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
					{
						ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().range());
					}

					ImGui::EndTable();
				}

				if (ImGui::TreeNode("Values"))
				{
					if (ImGui::BeginTable("TimingValuesTable", numThreads + 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
					                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
					{
						ImGui::TableSetupScrollFreeze(0, 1);
						ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
						for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
						{
							auxString.format("Th#%.2d", threadIndex);
							ImGui::TableSetupColumn(auxString.data());
						}
						ImGui::TableHeadersRow();

						unsigned int maxSize = 0;
						for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
						{
							if (workerStats[threadIndex].statistics().size() > maxSize)
								maxSize = workerStats[threadIndex].statistics().size();
						}

						ImGui::TableNextRow();
						for (unsigned int i = 0; i < maxSize; i++)
						{
							ImGui::TableNextColumn(); ImGui::Text("#%u", i);
							for (unsigned char threadIndex = 0; threadIndex < numThreads; threadIndex++)
							{
								ImGui::TableNextColumn(); ImGui::Text("%.3f ms", workerStats[threadIndex].statistics().values(sorted)[i]);
							}
						}

						ImGui::EndTable();
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}

		if (nc::theJobStatistics().isAvailable())
		{
			ImGui::SetNextWindowSize(ImVec2(540.0f, 680.0f), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(720.0f, 20.0f), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Debug statistics", &showImGui))
			{
				if (ImGui::TreeNodeEx("JobSystem statistics", ImGuiTreeNodeFlags_DefaultOpen))
				{
					nc::JobStatistics::JobSystemStats systemStats;
					nc::theJobStatistics().collectAllJobSystemStats(systemStats);

					guiJobSystemStatsTable(systemStats, "JobSystemTable");

					if (ImGui::Button("Reset##JobSystem"))
						nc::theJobStatistics().resetAllJobSystemStats();

					if (ImGui::TreeNode("Per-thread statistics"))
					{
						for (unsigned char i = 0; i < jobSystem.numThreads(); i++)
						{
							auxString.format("JobSystemTable#%02d", i);
							guiJobSystemStatsTable(nc::theJobStatistics().jobSystemStats(i), auxString.data());

							auxString.format("Reset##JobSystem#%02d", i);
							if (ImGui::Button(auxString.data()))
								nc::theJobStatistics().resetJobSystemStats(i);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("JobPool statistics", ImGuiTreeNodeFlags_DefaultOpen))
				{
					nc::JobStatistics::JobPoolStats poolStats;
					nc::theJobStatistics().collectAllJobPoolStats(poolStats);

					guiJobPoolStatsTable(poolStats, "JobPoolTable");

					if (ImGui::Button("Reset##JobPool"))
						nc::theJobStatistics().resetAllJobPoolStats();

					if (ImGui::TreeNode("Per-thread statistics"))
					{
						for (unsigned char i = 0; i < jobSystem.numThreads(); i++)
						{
							auxString.format("JobPoolTable#%02d", i);
							guiJobPoolStatsTable(nc::theJobStatistics().jobPoolStats(i), auxString.data());

							auxString.format("Reset##JobPool#%02d", i);
							if (ImGui::Button(auxString.data()))
								nc::theJobStatistics().resetJobPoolStats(i);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("JobQueue statistics", ImGuiTreeNodeFlags_DefaultOpen))
				{
					nc::JobStatistics::JobQueueStats queueStats;
					nc::theJobStatistics().collectAllJobQueueStats(queueStats);

					guiJobQueueStatsTable(queueStats, "JobQueueTable");

					if (ImGui::Button("Reset##JobQueue"))
						nc::theJobStatistics().resetAllJobQueueStats();

					if (ImGui::TreeNode("Per-thread statistics"))
					{
						for (unsigned char i = 0; i < jobSystem.numThreads(); i++)
						{
							auxString.format("JobQueueTable#%02d", i);
							guiJobQueueStatsTable(nc::theJobStatistics().jobQueueStats(i), auxString.data());

							auxString.format("Reset##JobQueue#%02d", i);
							if (ImGui::Button(auxString.data()))
								nc::theJobStatistics().resetJobQueueStats(i);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				ImGui::End();
			}
		}
	}
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
}
