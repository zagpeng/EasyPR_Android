package com.example.carplate;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.opencv.android.OpenCVLoader;

import com.example.carplate.util.FileUtil;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener {

	static {
		if (!OpenCVLoader.initDebug()) {

		} else {
			System.loadLibrary("imageproc");
		}
	}

	private SurfaceView cameraPreview;
	private Camera camera = null;

	private String svmPath = null;
	private String annPath = null;
	private String resultPath = null;
	private String tmpPath = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);

		try {
			init();
		} catch (IOException e) {
			Toast.makeText(this, "初始化失败", Toast.LENGTH_LONG).show();
			finish();
		}

		cameraPreview = (SurfaceView) findViewById(R.id.cameraPreview);
		cameraPreview.getHolder().addCallback(cameraPreviewHolderCallback);

		findViewById(R.id.btnTakePic).setOnClickListener(this);
	}

	private void init() throws IOException {
		String dataPath = FileUtil.getDataPath(this);
		FileUtil.mkdirs(dataPath);
		annPath = FileUtil.copyAssets(this, "ann.xml", dataPath);
		svmPath = FileUtil.copyAssets(this, "svm.xml", dataPath);

		tmpPath = dataPath + "/tmp";
		FileUtil.mkdirs(tmpPath);

		resultPath = Environment.getExternalStorageDirectory().toString() + "/DCIM";
		FileUtil.mkdirs(resultPath);

		// TODO 初始化GPS
	}

	@Override
	public void onClick(View v) {
		camera.takePicture(null, null, new Camera.PictureCallback() {
			@Override
			public void onPictureTaken(final byte[] data, Camera camera) {
				try {
					byte[] resultByte = CarPlateDetection.ImageProc(data, tmpPath, svmPath, annPath);
					String result = new String(resultByte, "GBK");
					if (result.contains(";")) {
						result = result.substring(0, result.indexOf(";"));
					}
					if (result.contains(":")) {
						result = result.substring(result.indexOf(":") + 1);
					}
					final String result2 = result;

					new AlertDialog.Builder(MainActivity.this).setTitle("识别结果").setMessage(result)
							.setPositiveButton("保存", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							// TODO 保存GPS信息

							// 保存图片
							FileOutputStream out = null;
							try {
								out = new FileOutputStream(new File(resultPath, result2 + ".jpg"));
								out.write(data);
								out.flush();
								out.close();
							} catch (IOException e) {
								Toast.makeText(MainActivity.this, "保存失败", Toast.LENGTH_SHORT).show();
							} finally {
								if (out != null) {
									try {
										out.close();
									} catch (Exception e) {}
								}
							}
						}
					}).setNegativeButton("返回", null).show();

					camera.startPreview();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	private void startPreview() {
		camera = Camera.open();
		try {
			camera.setPreviewDisplay(cameraPreview.getHolder());
			camera.setDisplayOrientation(90);
			Parameters params = camera.getParameters();
			params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
			// params.setJpegQuality(90); // 设置照片的质量
			// params.setPictureFormat(256);
			// params.setRotation(90);
			// params.setPictureSize(1280, 960);

			camera.setParameters(params); // 将参数设置给相机
			camera.startPreview();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void stopPreview() {
		camera.stopPreview();
		camera.release();
	}

	private Callback cameraPreviewHolderCallback = new Callback() {
		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			stopPreview();
		}

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			startPreview();
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {}
	};

}
