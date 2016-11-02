package io.github.ncine;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class LoadLibrariesTV extends NativeActivity {

	static {
		System.loadLibrary("openal");

		try {
			System.loadLibrary("ncine");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Caught UnsatisfiedLinkError: " + e.getMessage());
		}
	}

}
