package io.github.ncine;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;

public class LoadLibraries extends NativeActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// Hide toolbar
		int SDK_INT = android.os.Build.VERSION.SDK_INT;
		if (SDK_INT >= 19)
		{
			setImmersiveSticky();

			View decorView = getWindow().getDecorView();
			decorView.setOnSystemUiVisibilityChangeListener(
				new View.OnSystemUiVisibilityChangeListener() {
					@Override
					public void onSystemUiVisibilityChange(int visibility) {
						setImmersiveSticky();
					}
				}
			);
		}
	}

	@Override
	protected void onResume() {
		super.onResume();

		// Hide toolbar
		int SDK_INT = android.os.Build.VERSION.SDK_INT;
		if (SDK_INT >= 11 && SDK_INT < 14)
		{
			getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
		}
		else if (SDK_INT >= 14 && SDK_INT < 19)
		{
			getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN |
			                                                 View.SYSTEM_UI_FLAG_LOW_PROFILE);
		}
		else if (SDK_INT >= 19)
		{
			setImmersiveSticky();
		}
	}

	void setImmersiveSticky() {
		View decorView = getWindow().getDecorView();
		decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN |
		                                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
		                                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
		                                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
		                                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
		                                View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
	}

	static {
		System.loadLibrary("openal");

		try {
			System.loadLibrary("ncine");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Caught UnsatisfiedLinkError: " + e.getMessage());
		}
	}

}
