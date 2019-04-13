package com.moonfrog.carromgold;

import com.moonfrog.carromgold.R;

import android.app.ActionBar;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Environment;
import android.app.NativeActivity;
import android.util.Log;
import android.view.Menu;
import android.content.Context;
import android.content.res.AssetManager;
import android.view.WindowManager;
import android.os.Build;
import java.io.File;
import android.view.View;

public class MainActivity extends NativeActivity {

//	public boolean firstlaunch=true;
    private AssetManager assetManager;
    private static final String APP_NAME = "carromgold";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT < 16) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                    WindowManager.LayoutParams.FLAG_FULLSCREEN);
        } else {
            View decorView = getWindow().getDecorView();
            // Hide both the navigation bar and the status bar.
            // SYSTEM_UI_FLAG_FULLSCREEN is only available on Android 4.1 and higher, but as
            // a general rule, you should design your app to hide the status bar whenever you
            // hide the navigation bar.
            int uiOptions = /*View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |*/ View.SYSTEM_UI_FLAG_FULLSCREEN;
            decorView.setSystemUiVisibility(uiOptions);
            // Remember that you should never show the action bar if the
            // status bar is hidden, so hide that too if necessary.
            ActionBar actionBar = getActionBar();
            actionBar.hide();
        }

        Log.i(APP_NAME, "onCreate()");
        assetManager = getAssets();


        String writablePath = "";
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
            File[] files = getExternalFilesDirs(null);
            for (int x=0;x<files.length;x++) {
                Log.i(APP_NAME, "writablepath - " + files[x].getAbsolutePath());
                if (x==0) {
                    writablePath = files[x].getAbsolutePath();
                }
            }

            AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            Log.i(APP_NAME, "OpenSL Sample rate - " + myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE).toString());
            Log.i(APP_NAME, "OpenSL Sample buffer size - " + myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER).toString());
        }

        Log.i(APP_NAME, "Using writablepath - " + writablePath);
        mainlib(assetManager, writablePath);


        setContentView(R.layout.activity_main);
        Log.i(APP_NAME, "getExternalStorageDirectory "+ Environment.getExternalStorageDirectory());
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    }


    public static native void mainlib(AssetManager mgr, String writablePath);

    static{
        System.loadLibrary("carromgold");
    }
}
