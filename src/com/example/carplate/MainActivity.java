package com.example.carplate;

import java.io.UnsupportedEncodingException;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;

import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.Window;
import android.widget.Toast;

public class MainActivity extends Activity {
	//private ImageView imageView = null;
	//private Bitmap bmp = null;
	//private TextView m_text = null;
	private String path = null;
	private String strCaptureFilePath = null ;  
	private static final String TAG = "CarPhoto";
	
	static {
	    if (!OpenCVLoader.initDebug()) {
	    } else {
	        System.loadLibrary("imageproc");
	    }
	}
	
	
	private SurfaceView cameraPreview;
	private Camera camera=null;
	private Callback cameraPreviewHolderCallback=new Callback() {
		
		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			stopPreview();
		}
		
		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			startPreview();
		}
		
		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
			// TODO Auto-generated method stub
			
		}
	};
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);
		//imageView = (ImageView) findViewById(R.id.image_view);
		//m_text = (TextView) findViewById(R.id.myshow);
		//bmp = BitmapFactory.decodeResource(getResources(), R.drawable.plate_locate);
		//imageView.setImageBitmap(bmp);
		path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/easypr3";
		//strCaptureFilePath = path+"/temp.jpg";
		//System.out.println(path);
		//System.out.println(strCaptureFilePath);
		/*String svmpath = path+"/easypr/svm.xml";
		String annpath = path+"/easypr/ann.xml";
		String imgpath = path+"/easypr/56.jpg";
		byte[] resultByte = CarPlateDetection.ImageProc(imgpath,svmpath,annpath);
		try {
			String result = new String(resultByte,"GBK");
			Log.i(TAG, result.toString());
		} catch (UnsupportedEncodingException e1) {
			e1.printStackTrace();
		}*/
		
		cameraPreview = (SurfaceView) findViewById(R.id.cameraPreview);
		cameraPreview.getHolder().addCallback(cameraPreviewHolderCallback);
		
		findViewById(R.id.btnTakePic).setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				camera.takePicture(null, null, new Camera.PictureCallback() {
					
					@Override
					public void onPictureTaken(byte[] data, Camera camera) {
						
						
						
						try {
							File file = new File(path+"/",System.currentTimeMillis()+".jpg");
							FileOutputStream fos = new FileOutputStream(file);
							Log.i(TAG, file.toString());
							fos.write(data);
							fos.close();
							// 在拍照的时候相机是被占用的,拍照之后需要重新预览
							strCaptureFilePath = file.getAbsolutePath();
							
							String svmpath = path+"/svm.xml";
							String annpath = path+"/ann.xml";
							String imgpath = strCaptureFilePath.toString();
							//String imgpath =  Environment.getExternalStorageDirectory()+ "/DCIM/OpenCamera/IMG_20151130_212315.jpg";
							
							Log.i(TAG, imgpath.toString());
							//System.out.println(imgpath);
							
							byte[] resultByte = CarPlateDetection.ImageProc(imgpath,svmpath,annpath);
							String result = new String(resultByte,"GBK");
							Log.i(TAG, result.toString());
							//System.out.println(result);
							//m_text.setText(result);
							Toast.makeText(MainActivity.this, result.toString(), Toast.LENGTH_LONG).show();
							
							//imageView.setImageURI(Uri.fromFile(file));
							camera.startPreview();
							} catch (Exception e) {
							 e.printStackTrace();
							}

						
						
						
						//String pathPhoto=null;
						//if ((pathPhoto =saveTempFile(data))!=null) {
							
							//Intent i = new Intent(MainActivity.this, ImagePreviewAty.class);
							//i.putExtra("path", path);
							//startActivity(i);
							//imageView.setImageURI(Uri.fromFile(new File(pathPhoto)));
							
							
						//}else{
						//	Toast.makeText(MainActivity.this, "保存照片失败", Toast.LENGTH_SHORT).show();
						//}
					}
				});
			}
		});
		
		
		
	}
	
	

	private void startPreview(){
		camera = Camera.open();
		try {
			camera.setPreviewDisplay(cameraPreview.getHolder());			
			camera.setDisplayOrientation(90);
			Parameters params = camera.getParameters();
			params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
			//params.setJpegQuality(90);  // 设置照片的质量
			//params.setPictureFormat(256);
			//params.setRotation(90);
			//params.setPictureSize(1280, 960);
			
			camera.setParameters(params); // 将参数设置给相机
			camera.startPreview();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void stopPreview(){
		camera.stopPreview();
		camera.release();
	}
	

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS: {
			}
				break;
			default: {
				super.onManagerConnected(status);
			}
				break;
			}
		}
	};

//	public void click(View view) throws UnsupportedEncodingException {
//		//System.out.println("entering the jni");
//		//System.out.println(path);
//		//System.out.println(strCaptureFilePath);
//		
//		String svmpath = path+"/easypr/svm.xml";
//		String annpath = path+"/easypr/ann.xml";
//		String imgpath = strCaptureFilePath.toString();
//		//String imgpath =  Environment.getExternalStorageDirectory()+ "/DCIM/OpenCamera/IMG_20151130_212315.jpg";
//		
//		Log.i(TAG, imgpath.toString());
//		//System.out.println(imgpath);
//		
//		byte[] resultByte = CarPlateDetection.ImageProc(imgpath,svmpath,annpath);
//		String result = new String(resultByte,"GBK");
//		Log.i(TAG, result.toString());
//		//System.out.println(result);
//		//m_text.setText(result);
//		Toast.makeText(MainActivity.this, result.toString(), Toast.LENGTH_LONG).show();
//	}

	@Override
	protected void onResume() {
		super.onResume();
		mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
	}
}
