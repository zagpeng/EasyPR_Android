//	����GDTS���ļ�
//	�ھ������ݵ�GDTS���ݼ������������ķ�����ͼ�����Ԥ����
//	EasyPR��Դ��Ŀ�ǳ�ע�ر���ԭʼͼƬ�İ�Ȩ
//  ���еľ�������ͨ��GDSLЭ�����Լ������֤ʹ���˽����ڷ���ҵ��Ŀ��
#ifndef EASYPR_GDTS_H
#define EASYPR_GDTS_H

#include <opencv2/opencv.hpp>

namespace easypr {

namespace preprocess {

// EasyPR��ͼ��Ԥ������������ģ������ü�������
cv::Mat imageProcess(cv::Mat src);

int generate_gdts();

}

}

#endif //EASYPR_GDTS_H
