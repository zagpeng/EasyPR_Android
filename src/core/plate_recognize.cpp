#include "easypr/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

CPlateRecognize::CPlateRecognize() {
  // cout << "CPlateRecognize" << endl;
  // m_plateDetect= new CPlateDetect();
  // m_charsRecognise = new CCharsRecognise();
}

// !����ʶ��ģ��
int CPlateRecognize::plateRecognize(Mat src, std::vector<string> &licenseVec, int index) {
  // ���Ʒ��鼯��
  vector<CPlate> plateVec;

  // ������ȶ�λ��ʹ����ɫ��Ϣ�����Sobel
  int resultPD = plateDetect(src, plateVec, getPDDebug(), index);

  if (resultPD == 0) {
    int num = plateVec.size();
    int i = 0;

    //����ʶ��ÿ�������ڵķ���
    for (int j = 0; j < num; j++) {
      CPlate item = plateVec[j];
      Mat plate = item.getPlateMat();

      //��ȡ������ɫ
      string plateType = getPlateColor(plate);

      //��ȡ���ƺ�
      string plateIdentify = "";
      int resultCR = charsRecognise(plate, plateIdentify, index);
      if (resultCR == 0) {
        string license = plateType + ":" + plateIdentify;
        licenseVec.push_back(license);
      }
    }
    //����ʶ����̵��˽���

    //�����Debugģʽ������Ҫ����λ��ͼƬ��ʾ��ԭͼ���Ͻ�
    if (getPDDebug() == true) {
      Mat result;
      src.copyTo(result);

      for (int j = 0; j < num; j++) {
        CPlate item = plateVec[j];
        Mat plate = item.getPlateMat();

        int height = 36;
        int width = 136;
        if (height * i + height < result.rows) {
          Mat imageRoi = result(Rect(0, 0 + height * i, width, height));
          addWeighted(imageRoi, 0, plate, 1, 0, imageRoi);
        }
        i++;

        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);

        Scalar lineColor = Scalar(255, 255, 255);

        if (item.getPlateLocateType() == SOBEL) lineColor = Scalar(255, 0, 0);

        if (item.getPlateLocateType() == COLOR) lineColor = Scalar(0, 255, 0);

        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], lineColor, 2,
               8);
      }

      //��ʾ��λ���ͼƬ
      showResult(result);
    }
  }

  return resultPD;
}

} /*! \namespace easypr*/
