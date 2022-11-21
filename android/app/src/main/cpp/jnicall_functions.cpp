#include <jni.h>

extern "C"
{
	extern void updateMonitors(JNIEnv *env, jclass clazz);
	JNIEXPORT void JNICALL Java_io_github_ncine_LoadLibraries_updateMonitors(JNIEnv *env, jclass clazz)
	{
		updateMonitors(env, clazz);
	}
}
