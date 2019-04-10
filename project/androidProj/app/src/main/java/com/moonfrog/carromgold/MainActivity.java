package com.moonfrog.carromgold;

import com.moonfrog.carromgold.R;

import android.media.AudioManager;
import android.os.Bundle;
import android.os.Environment;
import android.app.NativeActivity;
import android.util.Log;
import android.view.Menu;
import android.content.Context;

import java.io.File;

public class MainActivity extends NativeActivity {

	public boolean firstlaunch=true;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
            File[] files = getExternalFilesDirs(null);
            for (int x=0;x<files.length;x++) {
                System.out.println("carromgold : writablepath - " + files[x].getAbsolutePath());
            }

            AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            System.out.println("carromgold : OpenSL Sample rate - " + myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE).toString());
            System.out.println("carromgold : OpenSL Sample buffer size - " + myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER).toString());
        }

        setContentView(R.layout.activity_main);
        System.out.println("getExternalStorageDirectory "+ Environment.getExternalStorageDirectory());
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
    
    public static native int mainlib();
    static{
        System.loadLibrary("carromgold");
    }
}
