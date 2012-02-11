package com.encelo.ncine;

import android.app.NativeActivity;

public class LoadLibraries extends NativeActivity {

	static {
		System.loadLibrary("openal");
	}

} 
