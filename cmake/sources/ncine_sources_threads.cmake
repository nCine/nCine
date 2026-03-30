if(Threads_FOUND AND NCINE_WITH_THREADS)
	target_compile_definitions(ncine PRIVATE "WITH_THREADS")
	target_link_libraries(ncine PRIVATE Threads::Threads)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/Thread.h
		${NCINE_ROOT}/src/include/ThreadSync.h
	)

	if(WIN32)
		list(APPEND SOURCES
			${NCINE_ROOT}/src/threading/WindowsThread.cpp
			${NCINE_ROOT}/src/threading/WindowsThreadSync.cpp
		)
		target_link_libraries(ncine PRIVATE synchronization)
	else()
		list(APPEND SOURCES
			${NCINE_ROOT}/src/threading/PosixThread.cpp
			${NCINE_ROOT}/src/threading/PosixThreadSync.cpp
		)
	endif()

	if(NCINE_WITH_JOBSYSTEM)
		target_compile_definitions(ncine PRIVATE "WITH_JOBSYSTEM")

		# `IJobSystem.h` and `IJobSystem.cpp` have been already added to headers and sources
		list(APPEND HEADERS
			${NCINE_ROOT}/include/ncine/JobHandle.h
			${NCINE_ROOT}/include/ncine/ParallelForJob.h
			${NCINE_ROOT}/include/ncine/JobStatistics.h
		)
		list(APPEND PRIVATE_HEADERS
			${NCINE_ROOT}/src/include/jobsystem_debug.h
			${NCINE_ROOT}/src/include/Job.h
			${NCINE_ROOT}/src/include/JobPool.h
			${NCINE_ROOT}/src/include/JobQueue.h
			${NCINE_ROOT}/src/include/JobSystem.h
			${NCINE_ROOT}/src/include/SerialJobSystem.h
			${NCINE_ROOT}/src/include/LogEntryQueue.h
			${NCINE_ROOT}/src/include/CpuTopology.h
		)
		list(APPEND SOURCES
			${NCINE_ROOT}/src/threading/jobsystem_debug.cpp
			${NCINE_ROOT}/src/threading/JobHandle.cpp
			${NCINE_ROOT}/src/threading/JobPool.cpp
			${NCINE_ROOT}/src/threading/JobQueue.cpp
			${NCINE_ROOT}/src/threading/JobSystem.cpp
			${NCINE_ROOT}/src/threading/SerialJobSystem.cpp
			${NCINE_ROOT}/src/threading/LogEntryQueue.cpp
			${NCINE_ROOT}/src/threading/JobStatistics.cpp
			${NCINE_ROOT}/src/threading/CpuTopology.cpp
		)

		if(WIN32)
			list(APPEND SOURCES ${NCINE_ROOT}/src/threading/CpuTopologyWindows.cpp)
		elseif(APPLE)
			list(APPEND SOURCES ${NCINE_ROOT}/src/threading/CpuTopologyApple.cpp)
		elseif(NOT EMSCRIPTEN)
			list(APPEND SOURCES ${NCINE_ROOT}/src/threading/CpuTopologyLinux.cpp)
		endif()
	endif()
endif()
