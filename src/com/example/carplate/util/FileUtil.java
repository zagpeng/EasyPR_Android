package com.example.carplate.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.os.Environment;

public class FileUtil {

	public static void init(Context context, String[] fileNames) throws IOException {
		String dataPath = FileUtil.getDataPath(context);
		for (String fileName : fileNames) {
			copyAssets(context, fileName, dataPath);
		}
	}

	private static String dataPath = null;

	public static String getDataPath(Context context) {
		if (dataPath == null) {
			dataPath = Environment.getExternalStorageDirectory().toString() + "/Android/data/"
					+ context.getPackageName();
		}

		return dataPath;
	}

	public static boolean mkdirs(String path) {
		return new File(path).mkdirs();
	}

	public static String copyAssets(Context context, String assetsName, String targetFolder) throws IOException {
		String target = targetFolder + "/" + assetsName;
		InputStream in = null;
		OutputStream out = null;

		try {
			in = context.getResources().getAssets().open(assetsName);
			out = new FileOutputStream(target);
			byte[] buffer = new byte[10240];
			int count = 0;
			while ((count = in.read(buffer)) >= 0) {
				out.write(buffer, 0, count);
				out.flush();
			}
			return target;
		} finally {
			if (out != null) {
				try {
					out.close();
				} catch (Exception e) {}
			}

			if (in != null) {
				try {
					in.close();
				} catch (Exception e) {}
			}
		}
	}
}
