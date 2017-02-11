package com.example.jm.android_simple_triangle;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {
    private GLSurfaceView surfaceView;
    private long renderer_handler;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);

        // Example of a call to a native method
        //TextView tv = (TextView) findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI());
        renderer_handler = nativeCreateRenderer(
                getClass().getClassLoader(),
                this.getApplicationContext());

        surfaceView = new GLSurfaceView(this);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 0, 0, 0);
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setRenderer(
                new GLSurfaceView.Renderer() {
                    @Override
                    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                        nativeInitializeGl(renderer_handler);
                    }

                    @Override
                    public void onSurfaceChanged(GL10 gl, int width, int height) {}

                    @Override
                    public void onDrawFrame(GL10 gl) {
                        nativeDrawFrame(renderer_handler);
                    }
                });
        setContentView(surfaceView);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Destruction order is important; shutting down the GvrLayout will detach
        // the GLSurfaceView and stop the GL thread, allowing safe shutdown of
        // native resources from the UI thread.
        nativeDestroyRenderer(renderer_handler);
    }

        /**
         * A native method that is implemented by the 'native-lib' native library,
         * which is packaged with this application.
         */
    private native long nativeCreateRenderer(
            ClassLoader appClassLoader, Context context);


    private native void nativeInitializeGl(long renderer_handler);

    private native long nativeDrawFrame(long renderer_handler);

    private native void nativeDestroyRenderer(long renderer_handler);
}
