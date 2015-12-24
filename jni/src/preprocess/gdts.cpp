#include "easypr/preprocess/gdts.h"
#include "easypr/preprocess/deface.h"
#include "easypr/util.h"

namespace easypr {

namespace preprocess {

// TODO �������·���ĳ����ԭʼͼƬ·��
// ͼƬ��Ҫ�࣬10-30�ž��㹻�ˣ�EasyPR��GDTS���ݼ���ʹ�ò�����Ϊ��Ҫָ��
// ֻҪ��ЩͼƬ�㹻��ӳ�����ݼ�����Ҫ��������
const char* src_path = "F:/data/easypr-data/tmp-1";

// TODO �������·���ĳ���ϣ�����ɾ�����GDTS���ݴ�ŵ���·��
const char* dst_path = "F:/data/easypr-data/tmp-2";

int generate_gdts() {
  // ��ȡ����ʶ���ļ�
  cv::CascadeClassifier cascade;
  std::string cascadeName = "resources/model/haarcascade_frontalface_alt_tree.xml";

  ////��ȡ��·���µ������ļ�
  auto files = Utils::getFiles(src_path);
  size_t size = files.size();

  if (0 == size) {
    std::cout << "No File Found!" << std::endl;
    return 0;
  }

  std::cout << "Begin to prepare generate_gdts!" << std::endl;

  for (size_t i = 0; i < size; i++) {
    std::string filepath = files[i].c_str();
    std::cout << "------------------" << std::endl;
    std::cout << filepath << std::endl;

    // EasyPR��ȡԭͼƬ
    cv::Mat src = cv::imread(filepath);

    // EasyPR��ʼ��ͼƬ����ģ������ü�������
    cv::Mat img = imageProcess(src);

    // EasyPR��ʼ��ͼƬ��������ʶ����
    cv::Mat dst = detectAndMaskFace(img, cascade, 1.5);

    // ��ͼƬ��������·��
    std::vector<std::string> spilt_path = Utils::splitString(filepath, '\\');

    size_t spiltsize = spilt_path.size();
    std::string filename = "";

    if (spiltsize != 0)
      filename = spilt_path[spiltsize - 1];

    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << dst_path << "/" << filename;
    utils::imwrite(ss.str(), dst);
  }

  return 0;
}

// EasyPR��ͼ��Ԥ������������ģ������ü�������
cv::Mat imageProcess(cv::Mat img) {
  int width = img.size().width;
  int height = img.size().height;
  cv::Rect_<double> rect(width * 0.01, height * 0.01, width * 0.99, height * 0.99);

  cv::Mat dst = img(rect);
  //GaussianBlur( dst, dst, Size(1, 1), 0, 0, BORDER_DEFAULT );
  return dst;
}

}

}

