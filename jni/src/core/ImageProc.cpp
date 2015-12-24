#include<com_example_carplate_CarPlateDetection.h>
#include "../include/easypr/plate_locate.h"
#include "../include/easypr/plate_judge.h"
#include "../include/easypr/chars_segment.h"
#include "../include/easypr/chars_identify.h"
#include "../include/easypr/plate_detect.h"
#include "../include/easypr/chars_recognise.h"
#include "../include/easypr/plate_recognize.h"
using namespace easypr;
#include <iostream>
#include <fstream>
#include <android/log.h>
#include <string>
#define LOG_TAG "System.out"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

char* jstring2str(JNIEnv* env, jstring jstr) {
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("GB2312");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0) {
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
}

JNIEXPORT jbyteArray JNICALL Java_com_example_carplate_CarPlateDetection_ImageProc(
		JNIEnv *env, jclass obj, jstring imgpath, jstring svmpath,
		jstring annpath) {
	CPlateRecognize pr;
//	const string *img = (*env)->GetStringUTFChars(env, imgpath, 0);
//	const string *svm = (*env)->GetStringUTFChars(env, svmpath, 0);
//	const string *ann = (*env)->GetStringUTFChars(env, annpath, 0);
	char* img = jstring2str(env,imgpath);
	char* svm = jstring2str(env,svmpath);
	char* ann = jstring2str(env,annpath);
	ofstream fout("/sdcard/easypr3/Debug.txt",ios::app);
	fout<<"=================="<<std::endl;
	fout<<"set params"<<std::endl;
	Mat src = imread(img);
	pr.LoadSVM(svm);
	pr.LoadANN(ann);

	pr.setGaussianBlurSize(5);
	pr.setMorphSizeWidth(17);

	pr.setVerifyMin(3);
	pr.setVerifyMax(20);

	pr.setLiuDingSize(7);
	pr.setColorThreshold(150);

	vector < string > plateVec;

	fout<<"call plateRecognize"<<std::endl;
	int count = pr.plateRecognize(src, plateVec, 0);
	fout<<"plateRecognize: "<<count<<std::endl;
	string str = "0";

	if (count == 0) {
		fout<<"vector size: "<<plateVec.size()<<std::endl;
		if ( ! plateVec.empty() ) {
			str = plateVec[0];
		}

	}

	char *result = new char[str.length() + 1];
	strcpy(result, str.c_str());
	jbyte *by = (jbyte*) result;
	jbyteArray jarray = env->NewByteArray(strlen(result));
	env->SetByteArrayRegion(jarray, 0, strlen(result), by);
	return jarray;
}
