package io.github.ncine;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class LoadLibrariesTV extends NativeActivity {

	static {
		System.loadLibrary("openal");
		System.loadLibrary("ncine");
	}

}
