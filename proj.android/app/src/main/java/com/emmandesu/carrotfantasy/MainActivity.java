package com.emmandesu.carrotfantasy;

import android.os.Bundle;
import android.view.WindowManager;
import org.cocos2dx.lib.Cocos2dxActivity;

/** The game renders directly through Cocos2d-x/OpenGL ES. */
public final class MainActivity extends Cocos2dxActivity {
    private static native boolean nativeBack();

    @Override protected void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override public void onBackPressed() {
        runOnGLThread(new Runnable() {
            @Override public void run() {
                if (!nativeBack()) runOnUiThread(new Runnable() {
                    @Override public void run() { finish(); }
                });
            }
        });
    }
}
