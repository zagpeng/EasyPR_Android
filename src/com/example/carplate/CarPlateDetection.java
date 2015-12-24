package com.example.carplate;

import java.io.FileOutputStream;

public class CarPlateDetection {

	public static native byte[] ImageProc(String imgpath, String svmpath, String annpath);

	public static native byte[] ImageProc(byte[] imgdata, String svmpath, String annpath);

	public static byte[] ImageProc(byte[] imgdata, String tmpPath, String svmpath, String annpath) {
		String file = tmpPath + "/" + System.currentTimeMillis() + ".jpg";
		FileOutputStream out = null;
		try {
			out = new FileOutputStream(file);
			out.write(imgdata);
			out.flush();
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (out != null) {
				try {
					out.close();
				} catch (Exception e) {}
			}
		}

		return ImageProc(file, svmpath, annpath);
	}
}
