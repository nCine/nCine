package io.github.ncine;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class LoadLibrariesTV extends NativeActivity {

	static {
		try {
			System.loadLibrary("openal");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Caught UnsatisfiedLinkError: " + e.getMessage());
		}

		System.loadLibrary("ncine");
	}

}
