#include <ncine/imgui.h>

#include <cmath>
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

#include <ncine/tracy.h>

#define LOG_TIMESTAMPS (0)

namespace {

int dummyIterations = 1024;
int numSpawnedJobs = 4;
nc::TimeStamp mainStartTime;
nc::TimeStamp mainEndTime;
nctl::StaticString<128> auxString;
bool autoResetStatistics = true;
int currentJobType = 1;
const char *jobTypeComboString = "Empty\0Dummy\0SpawnChildren\0\0";
bool waitRootJob = true;
int numJobsToQueue = 256;
int dataArraySize = 16 * 1024;
int countSplitterValue = 128;
int childrenCount = 10;
float maxTimeScale = 100.0f;
float maxFinishedJobsScale = 1024.0f;
bool showImGui = true;

class WorkerStatistics
{
  public:
	static const unsigned int TimestampCount = 32;

	struct TimestampStats
	{
		unsigned int timestampIndex = 0;
		nc::TimeStamp startTimestamps[TimestampCount];
		nc::TimeStamp endTimestamps[TimestampCount];
		float durations[TimestampCount];
	};

	explicit WorkerStatistics()
	    : numThreads_(0)
	{
	}

	explicit WorkerStatistics(unsigned char numThreads)
	    : numThreads_(numThreads)
	{
		setSize(numThreads);
	}

	void setSize(unsigned char numThreads)
	{
		numThreads_ = numThreads;

		timestampStats_.setSize(numThreads);
		totalFinishedJobs_.setSize(numThreads);
		timeAverages_.setSize(numThreads);

		reset();
	}

	void start(unsigned char threadIndex)
	{
		ASSERT(threadIndex < numThreads_);
		if (threadIndex < numThreads_)
		{
			TimestampStats &timestats = timestampStats_[threadIndex];
			nc::TimeStamp &startTime = timestats.startTimestamps[timestats.timestampIndex];
			startTime = nc::TimeStamp::now();
#if LOG_TIMESTAMPS
			LOGI_X("Thread %u started", threadIndex);
#endif
		}
	}

	void end(unsigned char threadIndex)
	{
		ASSERT(threadIndex < numThreads_);
		if (threadIndex < numThreads_)
		{
			TimestampStats &timestats = timestampStats_[threadIndex];
			const nc::TimeStamp &startTime = timestats.startTimestamps[timestats.timestampIndex];
			nc::TimeStamp &endTime = timestats.endTimestamps[timestats.timestampIndex];
			endTime = nc::TimeStamp::now();
#if LOG_TIMESTAMPS
			LOGI_X("Thread %u ended after %f ms", threadIndex, (endTime - startTime).milliseconds());
#endif

			timestats.durations[timestats.timestampIndex] = (endTime - startTime).milliseconds();
			totalFinishedJobs_[threadIndex]++;
			timestats.timestampIndex = (timestats.timestampIndex + 1) % TimestampCount;
		}
	}

	void reset()
	{
		const nc::TimeStamp now;
		for (unsigned char threadIndex = 0; threadIndex < numThreads_; threadIndex++)
		{
			TimestampStats &timestats = timestampStats_[threadIndex];
			timestats.timestampIndex = 0;
			for (unsigned int ts = 0; ts < TimestampCount; ts++)
			{
				timestats.startTimestamps[ts] = now;
				timestats.endTimestamps[ts] = now;
				timestats.durations[ts] = 0.0f;
			}

			timeAverages_[threadIndex] = 0.0f;
			totalFinishedJobs_[threadIndex] = 0;
		}
	}

	inline unsigned char numThreads() const { return numThreads_; }
	inline const float *timeDurations(unsigned char threadIndex) const { return timestampStats_[threadIndex].durations; }
	inline const nctl::Array<float> &totalFinishedJobs() const { return totalFinishedJobs_; }

	const nctl::Array<float> &calcAverages()
	{
		for (unsigned char threadIndex = 0; threadIndex < numThreads_; threadIndex++)
		{
			const TimestampStats &timestats = timestampStats_[threadIndex];
			timeAverages_[threadIndex] = 0.0f;

			unsigned int validTimestatCount = 0;
			for (unsigned int ts = 0; ts < TimestampCount; ts++)
			{
				if (timestats.durations[ts] > 0.0f)
				{
					timeAverages_[threadIndex] += timestats.durations[ts];
					validTimestatCount++;
				}
			}
			timeAverages_[threadIndex] /= validTimestatCount;
		}
		return timeAverages_;
	}

  private:
	unsigned char numThreads_;
	nctl::Array<TimestampStats> timestampStats_;
	nctl::Array<float> timeAverages_;
	nctl::Array<float> totalFinishedJobs_;
};

WorkerStatistics workerStats;
static volatile unsigned int dummySink = 0;

void emptyJobWithStatistics(nc::JobId, const void *)
{
	ZoneScoped;
	workerStats.start(nc::IJobSystem::threadIndex());
	workerStats.end(nc::IJobSystem::threadIndex());
}

void dummyJob(nc::JobId, const void *)
{
	ZoneScoped;
	static const unsigned int BUFFER_SIZE = 8192;
	static thread_local unsigned int buffer[BUFFER_SIZE];
	const unsigned int threadIndex = nc::IJobSystem::threadIndex();

	workerStats.start(nc::IJobSystem::threadIndex());

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
	workerStats.end(nc::IJobSystem::threadIndex());
}

void spawnChildrenJob(nc::JobId jobId, const void *)
{
	ZoneScoped;
	workerStats.start(nc::IJobSystem::threadIndex());
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();
	for (int i = 0; i < numSpawnedJobs; i++)
	{
		nc::JobId childJobId = jobSystem.createJobAsChild(jobId, dummyJob);
		jobSystem.submit(childJobId);
	}
	workerStats.end(nc::IJobSystem::threadIndex());
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
	workerStats.start(threadIndex);
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
	workerStats.end(nc::IJobSystem::threadIndex());
}

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
	workerStats.setSize(jobSystem.numThreads());
}

void MyEventHandler::onFrameStart()
{
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();

	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(580.0f, 640.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_jobsystem", &showImGui))
		{
			ImGui::Text("Number of threads: %u (main is #%u)", jobSystem.numThreads(), jobSystem.threadIndex());
			ImGui::SameLine();
			ImGui::Checkbox("Wait root", &waitRootJob);
			ImGui::SameLine();
			ImGui::Checkbox("Auto-reset stats", &autoResetStatistics);

			nc::JobFunction jobFn = &emptyJobWithStatistics;
			ImGui::Combo("Job's type", &currentJobType, jobTypeComboString);
			switch (currentJobType)
			{
				case 0: jobFn = emptyJobWithStatistics; break;
				case 1: jobFn = dummyJob; break;
				case 2: jobFn = spawnChildrenJob; break;
				default: jobFn = emptyJobWithStatistics; break;
			}
			ImGui::SliderInt("Jobs iterations", &dummyIterations, 0, 4096);
			if (dummyIterations < 0)
				dummyIterations = 0;

			static nc::JobId rootJobId = nc::InvalidJobId;
			ImGui::SliderInt("Number of jobs", &numJobsToQueue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::BeginDisabled(jobFn != spawnChildrenJob);
			ImGui::SliderInt("Number of spawned jobs", &numSpawnedJobs, 0, 16, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndDisabled();
			if (ImGui::Button("Enqueue jobs"))
			{
				if (autoResetStatistics)
					resetJobSystemStatistics();

				mainStartTime = nc::TimeStamp::now();

				rootJobId = jobSystem.createJob(nullptr);
				for (int i = 0; i < numJobsToQueue; i++)
				{
					nc::JobId jobId = jobSystem.createJobAsChild(rootJobId, jobFn);
					jobSystem.submit(jobId);
				}
				jobSystem.submit(rootJobId);
				if (waitRootJob)
					jobSystem.wait(rootJobId);

				mainEndTime = nc::TimeStamp::now();
			}

			ImGui::SameLine();
			if (ImGui::Button("Continuations"))
			{
				if (autoResetStatistics)
					resetJobSystemStatistics();

				mainStartTime = nc::TimeStamp::now();
				// Avoid an infinite spawning of jobs from continuations
				const nc::JobFunction contFn = (jobFn != spawnChildrenJob) ? jobFn : emptyJobWithStatistics;

				rootJobId = jobSystem.createJob(nullptr);
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
				jobSystem.submit(rootJobId);
				if (waitRootJob)
					jobSystem.wait(rootJobId);

				mainEndTime = nc::TimeStamp::now();
			}

			ImGui::SliderInt("Data array size", &dataArraySize, 1, MaxDataArraySize, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderInt("Count splitter", &countSplitterValue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Parallel for"))
			{
				if (autoResetStatistics)
					resetJobSystemStatistics();

				mainStartTime = nc::TimeStamp::now();

				rootJobId = nc::parallelFor(dataArray, dataArraySize, &myDataFunc, nc::CountSplitter(countSplitterValue));
				jobSystem.submit(rootJobId);
				if (waitRootJob)
					jobSystem.wait(rootJobId);

				mainEndTime = nc::TimeStamp::now();
			}

			ImGui::SliderInt("Children count", &childrenCount, 1, 32, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Parent / children"))
			{
				if (autoResetStatistics)
					resetJobSystemStatistics();

				mainStartTime = nc::TimeStamp::now();

				rootJobId = jobSystem.createJob(nullptr);
				for (int i = 0; i < childrenCount; i++)
				{
					nc::JobId grandparentId = jobSystem.createJobAsChild(rootJobId, nullptr);
					for (int j = 0; j < childrenCount; j++)
					{
						nc::JobId parentId = jobSystem.createJobAsChild(grandparentId, nullptr);
						for (int k = 0; k < childrenCount; k++)
						{
							nc::JobId jobId = jobSystem.createJobAsChild(parentId, jobFn);
							jobSystem.submit(jobId);
						}
						jobSystem.submit(parentId);
					}
					jobSystem.submit(grandparentId);
				}
				jobSystem.submit(rootJobId);
				if (waitRootJob)
					jobSystem.wait(rootJobId);

				mainEndTime = nc::TimeStamp::now();
			}
			ImGui::SameLine();
			const unsigned int totalChildrenJobs = childrenCount * childrenCount * childrenCount;
			ImGui::Text("Total Jobs: %u (%u x %u x %u)", totalChildrenJobs, childrenCount, childrenCount, childrenCount);

			if (waitRootJob)
				ImGui::Text("Total time: %f ms", (mainEndTime - mainStartTime).milliseconds());
			else if (rootJobId != nc::InvalidJobId)
				ImGui::Text("Unfinished jobs: %u", jobSystem.unfinishedJobs(rootJobId));

			if (ImGui::TreeNode("Timestamp statistics"))
			{
				const nctl::Array<float> &totalFinishedJobs = workerStats.totalFinishedJobs();
				const nctl::Array<float> &timeAverages = workerStats.calcAverages();

				static bool autoScaleJobs = true;
				static bool autoScaleTime = true;

				if (autoScaleJobs == true)
				{
					float maxValue = 0.0f;
					for (unsigned int i = 0; i < totalFinishedJobs.size(); i++)
					{
						if (totalFinishedJobs[i] > maxValue)
							maxValue = totalFinishedJobs[i];
					}
					maxFinishedJobsScale = maxValue * 1.1f;
				}

				ImGui::BeginDisabled(autoScaleJobs);
				ImGui::SliderFloat("Jobs scale", &maxFinishedJobsScale, 16.0f, 4096.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::Checkbox("Auto##Jobs", &autoScaleJobs);
				ImGui::PlotHistogram("Total finished jobs", totalFinishedJobs.data(), totalFinishedJobs.size(), 0, nullptr, 0.0f, maxFinishedJobsScale);
				if (ImGui::Button("Reset"))
					workerStats.reset();

				if (autoScaleTime == true)
				{
					float maxValue = 0;
					for (unsigned int i = 0; i < workerStats.numThreads(); i++)
					{
						if (timeAverages[i] > maxValue)
							maxValue = timeAverages[i];
					}
					maxTimeScale = maxValue * 1.1f;
				}

				ImGui::NewLine();
				ImGui::BeginDisabled(autoScaleTime);
				ImGui::SliderFloat("Time scale", &maxTimeScale, 0.001f, 50.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::Checkbox("Auto##Time", &autoScaleTime);
				ImGui::PlotHistogram("Average timestamps", timeAverages.data(), workerStats.numThreads(), 0, nullptr, 0.0f, maxTimeScale);
				for (unsigned char threadIndex = 0; threadIndex < workerStats.numThreads(); threadIndex++)
				{
					auxString.format("Timestamps for T#%u", threadIndex);
					ImGui::PlotHistogram(auxString.data(), workerStats.timeDurations(threadIndex), WorkerStatistics::TimestampCount, 0, nullptr, 0.0f, maxTimeScale);
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}

		if (nc::theJobStatistics().isAvailable())
		{
			ImGui::SetNextWindowSize(ImVec2(580.0f, 640.0f), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowPos(ImVec2(660.0f, 40.0f), ImGuiCond_FirstUseEver);
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
