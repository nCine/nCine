#include <ncine/config.h>

#include "apptest_jobsystem.h"
#include <ncine/IJobSystem.h>
#include <ncine/ParallelForJob.h>
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/Timer.h>
#include <ncine/TimeStamp.h>
#include <nctl/Array.h>

#if NCINE_WITH_IMGUI
	#include <ncine/imgui.h>
	#include <nctl/StaticString.h>
#endif

// TODO: Use config.h to check for threads? Exclude from compilation in `tests/CMakeLists.txt`? What if using a serial job system?

namespace {

#if NCINE_WITH_IMGUI
nc::TimeStamp start;
nc::TimeStamp end;
nctl::StaticString<128> auxString;
int currentJobType = 2;
const char *jobTypeComboString = "Empty\0Dummy Fast\0Dummy\0\0";
bool waitRootJob = true;
int numJobsToQueue = 256;
int countSplitterValue = 128;
int childrenCount = 10;
float maxTimeScale = 100.0f;
float maxFinishedJobsScale = 1024.0f;
bool showImGui = true;
#endif

// TODO: Make the jobs write statistics as timestamps and counters (plot counters data with ImGui)

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
			nc::TimeStamp &start = timestats.startTimestamps[timestats.timestampIndex];
			start = nc::TimeStamp::now();
		}
	}

	void end(unsigned char threadIndex)
	{
		ASSERT(threadIndex < numThreads_);
		if (threadIndex < numThreads_)
		{
			TimestampStats &timestats = timestampStats_[threadIndex];
			const nc::TimeStamp &start = timestats.startTimestamps[timestats.timestampIndex];
			nc::TimeStamp &end = timestats.endTimestamps[timestats.timestampIndex];
			end = nc::TimeStamp::now();

			timestats.durations[timestats.timestampIndex] = (end - start).milliseconds();
			totalFinishedJobs_[threadIndex]++;
			timestats.timestampIndex = (timestats.timestampIndex + 1) % TimestampCount;
		}
	}

	void reset()
	{
		const nc::TimeStamp now;
		for (unsigned int threadIndex = 0; threadIndex < numThreads_; threadIndex++)
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

	unsigned char numThreads() const { return numThreads_; }
	const float *timeDurations(unsigned char threadIndex) const {return timestampStats_[threadIndex].durations; }
	const nctl::Array<float> &totalFinishedJobs() const { return totalFinishedJobs_; }

	const nctl::Array<float> &calcAverages()
	{
		for (unsigned int threadIndex = 0; threadIndex < numThreads_; threadIndex++)
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

void emptyJob(nc::JobId, const void *)
{
}

void emptyJobWithStatistics(nc::JobId, const void *)
{
	workerStats.start(nc::IJobSystem::threadIndex());
	workerStats.end(nc::IJobSystem::threadIndex());
}

void dummyJob(nc::JobId, const void *)
{
	workerStats.start(nc::IJobSystem::threadIndex());
	for (unsigned int i = 0; i < 1024 * 1024; i++)
	{
		const unsigned int k = i * i;
		const unsigned int q = (i + k) / (i + 1);
		const unsigned int l = (k * i) / (q + 1);
		const unsigned int w = (k / (l + 1)) * q;
	}
	workerStats.end(nc::IJobSystem::threadIndex());
}

void dummyFastJob(nc::JobId, const void *)
{
	workerStats.start(nc::IJobSystem::threadIndex());
	for (unsigned int i = 0; i < 1024; i++)
	{
		const unsigned int k = i * i;
		const unsigned int q = (i + k) / (i + 1);
		const unsigned int l = (k * i) / (q + 1);
		const unsigned int w = (k / (l + 1)) * q;
	}
	workerStats.end(nc::IJobSystem::threadIndex());
}

struct MyData
{
	unsigned int max = 0;
	unsigned int calc = 0;
	float calc2 = 0;
};

const unsigned int DataArraySize = 4096;
MyData dataArray[DataArraySize]; // TODO: Make it dynamic?

void myDataFunc(MyData *data, unsigned int count)
{
	workerStats.start(nc::IJobSystem::threadIndex());
	for (unsigned int i = 0; i < count; i++)
	{
		data[i].max = (~0u);
		data[i].calc = i * i;
		for (unsigned int j = 0; j < 1024; j++)
			data[i].calc2 += data[i].calc / 0.025f;
	}
	workerStats.end(nc::IJobSystem::threadIndex());
}

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.withThreads = true;
}

void MyEventHandler::onInit()
{
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();
	workerStats.setSize(jobSystem.numThreads());

#if 0
	//LOGI("APPTEST_JOBSYSTEM: jobs are starting");
	start = nc::TimeStamp::now();

/*
	numJobsToQueue = 1000;
	nc::JobId rootId = jobSystem.createJob(&emptyJob);
	for (int i = 0; i < numJobsToQueue; i++)
	{
		nc::JobId jobId = jobSystem.createJobAsChild(rootId, &dummfyJob);
		jobSystem.run(jobId);
	}
	jobSystem.run(rootId);
	jobSystem.wait(rootId);
*/

	nc::JobId jobId = nc::parallelFor(dataArray, 4096, &myDataFunc, nc::CountSplitter(128));
	jobSystem.run(jobId);
	jobSystem.wait(jobId);

	end = nc::TimeStamp::now();

	LOGI_X("APPTEST_JOBSYSTEM: jobs have finished (%f ms)", (end - start).milliseconds());
	nc::theApplication().quit();
#endif

#ifdef __ANDROID__
	ImGui::GetIO().FontGlobalScale = 3.0f;
#endif
}

void MyEventHandler::onFrameStart()
{
	nc::IJobSystem &jobSystem = nc::theServiceLocator().jobSystem();

	// TODO: Add gui support to parent/children jobs and continuations in the GUI
#if NCINE_WITH_IMGUI
	if (showImGui)
	{
		ImGui::SetNextWindowSize(ImVec2(580.0f, 580.0f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(35.0f, 35.0f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("apptest_jobsystem", &showImGui))
		{
			ImGui::Text("Number of threads: %u (main is #%u)", jobSystem.numThreads(), jobSystem.threadIndex());

			nc::JobFunction jobFn = &emptyJobWithStatistics;
			ImGui::Combo("Job's type", &currentJobType, jobTypeComboString);
			switch (currentJobType)
			{
				case 0: jobFn = emptyJobWithStatistics; break;
				case 1: jobFn = dummyFastJob; break;
				case 2: jobFn = dummyJob; break;
				default: jobFn = emptyJobWithStatistics; break;
			}
			ImGui::SameLine();
			ImGui::Checkbox("Wait root", &waitRootJob);

			static nc::JobId rootId = nc::NullJobId;
			ImGui::SliderInt("Number of jobs", &numJobsToQueue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Enqueue jobs"))
			{
				start = nc::TimeStamp::now();

				rootId = jobSystem.createJob(&emptyJob);
				if (jobSystem.isValid(rootId))
				{
					for (int i = 0; i < numJobsToQueue; i++)
					{
						nc::JobId jobId = jobSystem.createJobAsChild(rootId, jobFn);
						if (jobSystem.isValid(jobId))
							jobSystem.run(jobId);
					}
					jobSystem.run(rootId);
					if (waitRootJob)
						jobSystem.wait(rootId);
				}

				end = nc::TimeStamp::now();
			}

			ImGui::SliderInt("Count splitter", &countSplitterValue, 1, 1024, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::Button("Parallel for"))
			{
				start = nc::TimeStamp::now();

				rootId = nc::parallelFor(dataArray, DataArraySize, &myDataFunc, nc::CountSplitter(countSplitterValue));
				if (jobSystem.isValid(rootId))
				{
					jobSystem.run(rootId);
					jobSystem.wait(rootId);
				}

				end = nc::TimeStamp::now();
			}

			ImGui::SliderInt("Children count", &childrenCount, 1, 32, "%d", ImGuiSliderFlags_AlwaysClamp);
			const unsigned int totalChildrenJobs = childrenCount * childrenCount * childrenCount;
			ImGui::Text("Generating %u total jobs (%u x %u x %u)", totalChildrenJobs, childrenCount, childrenCount, childrenCount);
			if (ImGui::Button("Parent / children"))
			{
				start = nc::TimeStamp::now();

				rootId = jobSystem.createJob(&emptyJob);
				if (jobSystem.isValid(rootId))
				{
					for (unsigned int i = 0; i < totalChildrenJobs; i++)
					{
						nc::JobId grandparentId = jobSystem.createJobAsChild(rootId, &emptyJob);
						if (jobSystem.isValid(grandparentId))
						{
							for (unsigned int j = 0; j < totalChildrenJobs; j++)
							{
								nc::JobId parentId = jobSystem.createJobAsChild(grandparentId, &emptyJob);
								if (jobSystem.isValid(parentId))
								{
									for (unsigned int k = 0; k < totalChildrenJobs; k++)
									{
										nc::JobId jobId = jobSystem.createJobAsChild(parentId, jobFn);
										if (jobSystem.isValid(jobId))
											jobSystem.run(jobId);
									}
									jobSystem.run(parentId);
								}
							}
							jobSystem.run(grandparentId);
						}
					}
					jobSystem.run(rootId);
					jobSystem.wait(rootId);
				}

				end = nc::TimeStamp::now();
			}

			if (ImGui::Button("Continuations"))
			{
				start = nc::TimeStamp::now();

				rootId = jobSystem.createJob(&emptyJob);
				if (jobSystem.isValid(rootId))
				{
					for (unsigned int i = 0; i < numJobsToQueue; i++)
					{
						nc::JobId jobId = jobSystem.createJobAsChild(rootId, jobFn);
						if (jobSystem.isValid(jobId))
						{
							for (unsigned int j = 0; j < nc::JobNumContinuations; j++)
							{
								nc::JobId contId = jobSystem.createJob(jobFn);
								if (jobSystem.isValid(contId))
									jobSystem.addContinuation(jobId, contId);
							}
							jobSystem.run(jobId);
						}
					}
					jobSystem.run(rootId);
					jobSystem.wait(rootId);
				}

				end = nc::TimeStamp::now();
			}

			if (waitRootJob)
				ImGui::Text("Total time: %f ms", (end - start).milliseconds());
			else if (jobSystem.isValid(rootId))
				ImGui::Text("Unfinished jobs: %u", jobSystem.unfinishedJobs(rootId));

			if (ImGui::TreeNode("Statistics"))
			{
				const nctl::Array<float> &totalFinishedJobs = workerStats.totalFinishedJobs();
				const nctl::Array<float> &timeAverages = workerStats.calcAverages();


				ImGui::SliderFloat("Max finished jobs scale", &maxFinishedJobsScale, 16.0f, 4096.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::PlotHistogram("Total finished jobs", totalFinishedJobs.data(), totalFinishedJobs.size(), 0, nullptr, 0.0f, maxFinishedJobsScale);

				ImGui::NewLine();
				ImGui::SliderFloat("Max time scale", &maxTimeScale, 0.001f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::PlotHistogram("Average timestamps", timeAverages.data(), workerStats.numThreads(), 0, nullptr, 0.0f, maxTimeScale);
				for (unsigned int threadIndex = 0; threadIndex < workerStats.numThreads(); threadIndex++)
				{
					auxString.format("Timestamps for T#%u", threadIndex);
					ImGui::PlotHistogram(auxString.data(), workerStats.timeDurations(threadIndex), WorkerStatistics::TimestampCount, 0, nullptr, 0.0f, maxTimeScale);
				}
				if (ImGui::Button("Reset"))
					workerStats.reset();

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
#endif
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
#if NCINE_WITH_IMGUI
	else if (event.mod & nc::KeyMod::CTRL && event.sym == nc::KeySym::H)
		showImGui = !showImGui;
#endif
}
