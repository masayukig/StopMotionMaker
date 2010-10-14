package org.orzlabs.android.stopmotionmaker;

import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

public class StopMotionMaker extends Activity {
	private static final int IN_SAMPLE_SIZE = 1;
	private static final String TAG = "StopMotionMaker";

	private Camera mCamera;
	private ImageView mImage;
	private boolean mInProgress;

	private SurfaceHolder.Callback mSurfaceListener =
		new SurfaceHolder.Callback() {

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			mCamera = Camera.open();
			Log.i(TAG, "Camera opened");
			try {
				mCamera.setPreviewDisplay(holder);
			}
			catch (Exception e) {
				e.printStackTrace();
			}
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			mCamera.release();
			mCamera = null;
			Log.i(TAG, "Camera released");
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			Camera.Parameters parameters = mCamera.getParameters();
			parameters.setPreviewSize(width, height);
			mCamera.setParameters(parameters);
			mCamera.startPreview();
			Log.i(TAG, "Camera preview started");
			Log.i(TAG, "width  : " + width);
			Log.i(TAG, "height : " + height);
		}
	};

	private Camera.AutoFocusCallback mAutoFocusListener =
		new Camera.AutoFocusCallback() {  
		@Override
		public void onAutoFocus(boolean success, Camera camera) {
			Log.i(TAG,"AutoFocus : " + success);
			camera.autoFocus(null);
			camera.takePicture(mShutterListener, null, mPictureListener);
			mInProgress = true;
		}
	};

	private Camera.ShutterCallback mShutterListener = 
		new Camera.ShutterCallback() {
		@Override
		public void onShutter() {
			Log.i(TAG, "onShutter");
		}
	};

	private View.OnClickListener mButtonListener = 
		new View.OnClickListener() {   
		@Override
		public void onClick(View v) {
			if(mCamera != null && mInProgress == false){
				mCamera.autoFocus(mAutoFocusListener);
			}
		}
	};

	private Camera.PictureCallback mPictureListener = 
		new Camera.PictureCallback() {   
		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
			Log.i(TAG, "Picture taken");
			if(data != null) {
				Log.i(TAG, "JPEG Picture Taken");
				BitmapFactory.Options options = new BitmapFactory.Options();
				options.inSampleSize = IN_SAMPLE_SIZE;
				Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length, options);

				mImage.setImageBitmap(bitmap);
				mImage.setVisibility(View.VISIBLE);
				try {
					data2sd(data, "test.jpg");
				} catch (IOException e) {
					Log.e(this.toString(), e.getMessage(), e);
				}

				camera.startPreview();
				mInProgress = false;
			}
		}
	};
	private void data2sd(byte[] data, String fileName)
	throws IOException {
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream("/sdcard/" + fileName);
			fos.write(data);
		} finally {
			if (fos != null) {
				fos.close();
			}
		}
	}
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		mImage = (ImageView) findViewById(R.id.handpreview);
		SurfaceView surface = (SurfaceView) findViewById(R.id.surfaceview);
		SurfaceHolder holder = surface.getHolder();

		holder.addCallback(mSurfaceListener);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

		Button button = (Button) findViewById(R.id.shutter);
		button.setOnClickListener(mButtonListener);
	} 

}