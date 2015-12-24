#include "../include/easypr/plate_locate.h"
#include "../include/easypr/util.h"

using namespace std;

/*! \namespace easypr
        Namespace where all the C++ EasyPR functionality resides
        */
namespace easypr {

const float DEFAULT_ERROR = 0.9f;    // 0.6
const float DEFAULT_ASPECT = 3.75f;  // 3.75

CPlateLocate::CPlateLocate() {
  // cout << "CPlateLocate" << endl;
  m_GaussianBlurSize = DEFAULT_GAUSSIANBLUR_SIZE;
  m_MorphSizeWidth = DEFAULT_MORPH_SIZE_WIDTH;
  m_MorphSizeHeight = DEFAULT_MORPH_SIZE_HEIGHT;

  m_error = DEFAULT_ERROR;
  m_aspect = DEFAULT_ASPECT;
  m_verifyMin = DEFAULT_VERIFY_MIN;
  m_verifyMax = DEFAULT_VERIFY_MAX;

  m_angle = DEFAULT_ANGLE;

  m_debug = DEFAULT_DEBUG;
}

//! ����ģʽ�빤ҵģʽ�л�
//!
//���Ϊ�棬�����ø������Ϊ��λ�������Ƭ����ٶ�ͼƬ���Ĳ���������ָ�Ĭ��ֵ��
void CPlateLocate::setLifemode(bool param) {
  if (param == true) {
    setGaussianBlurSize(5);
    setMorphSizeWidth(10);
    setMorphSizeHeight(3);
    setVerifyError(0.75);
    setVerifyAspect(4.0);
    setVerifyMin(1);
    setVerifyMax(200);
  } else {
    setGaussianBlurSize(DEFAULT_GAUSSIANBLUR_SIZE);
    setMorphSizeWidth(DEFAULT_MORPH_SIZE_WIDTH);
    setMorphSizeHeight(DEFAULT_MORPH_SIZE_HEIGHT);
    setVerifyError(DEFAULT_ERROR);
    setVerifyAspect(DEFAULT_ASPECT);
    setVerifyMin(DEFAULT_VERIFY_MIN);
    setVerifyMax(DEFAULT_VERIFY_MAX);
  }
}

//! ��minAreaRect��õ���С��Ӿ��Σ����ݺ�Ƚ����ж�
bool CPlateLocate::verifySizes(RotatedRect mr) {
  float error = m_error;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm��aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = m_aspect;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  int min = 34 * 8 * m_verifyMin;  // minimum area
  int max = 34 * 8 * m_verifyMax;  // maximum area

  // Get only patchs that match to a respect ratio.
  float rmin = aspect - aspect * error;
  float rmax = aspect + aspect * error;

  float area = mr.size.height * mr.size.width;
  float r = (float)mr.size.width / (float)mr.size.height;
  if (r < 1) r = (float)mr.size.height / (float)mr.size.width;

  // cout << "area:" << area << endl;
  // cout << "r:" << r << endl;

  if ((area < min || area > max) || (r < rmin || r > rmax))
    return false;
  else
    return true;
}

// !����HSV�ռ����ɫ��������
int CPlateLocate::colorSearch(const Mat& src, const Color r, Mat& out,
                              vector<RotatedRect>& outRects, int index) {
  Mat match_grey;

  // widthֵ�����ս��Ӱ��ܴ󣬿��Կ��ǽ��ж��colorSerch��ÿ�β�ͬ��ֵ
  // ��һ�ֽ�����������ڽ�������SVM֮ǰ����������ǵ��پ���
  const int color_morph_width = 10;
  const int color_morph_height = 2;

  // ������ɫ����
  colorMatch(src, match_grey, r, false);

  if (m_debug) {
    imwrite("resources/image/tmp/match_grey.jpg", match_grey);
  }

  Mat src_threshold;
  threshold(match_grey, src_threshold, 0, 255,
            CV_THRESH_OTSU + CV_THRESH_BINARY);

  Mat element = getStructuringElement(
      MORPH_RECT, Size(color_morph_width, color_morph_height));
  morphologyEx(src_threshold, src_threshold, MORPH_CLOSE, element);

  if (m_debug) {
    imwrite("resources/image/tmp/color.jpg", src_threshold);
  }

  src_threshold.copyTo(out);

  // ��������
  vector<vector<Point> > contours;

  // ע�⣬findContours��ı�src_threshold
  // ���Ҫ���src_threshold��������֮ǰʹ��copyTo����
  findContours(src_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // ��ȡ�ⲿ����
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));

    // ��Ҫ���д�С�ߴ��ж�
    if (!verifySizes(mr))
      itc = contours.erase(itc);
    else {
      ++itc;
      outRects.push_back(mr);
    }
  }

  return 0;
}

//! Sobel��һ������
//! �����ƴ�С����״����ȡ��BoundRect������һ��
int CPlateLocate::sobelFrtSearch(const Mat& src,
                                 vector<Rect_<float> >& outRects) {
  Mat src_threshold;
  // soble�������õ���ֵͼ��
  sobelOper(src, src_threshold, m_GaussianBlurSize, m_MorphSizeWidth,
            m_MorphSizeHeight);

  if (0) {
    imshow("sobelFrtSearch", src_threshold);
    waitKey(0);
    destroyWindow("sobelFrtSearch");
  }

  vector<vector<Point> > contours;
  findContours(src_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // ��ȡ�ⲿ����
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();

  vector<RotatedRect> first_rects;

  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));

    // ��Ҫ���д�С�ߴ��ж�
    if (verifySizes(mr)) {
      first_rects.push_back(mr);

      float area = mr.size.height * mr.size.width;
      float r = (float)mr.size.width / (float)mr.size.height;
      if (r < 1) r = (float)mr.size.height / (float)mr.size.width;

      /*cout << "area:" << area << endl;
      cout << "r:" << r << endl;*/
    }

    ++itc;
  }

  for (size_t i = 0; i < first_rects.size(); i++) {
    RotatedRect roi_rect = first_rects[i];

    Rect_<float> safeBoundRect;
    if (!calcSafeRect(roi_rect, src, safeBoundRect)) continue;

    outRects.push_back(safeBoundRect);
  }
  return 0;
}

//! Sobel�ڶ�������,�Զ��ѵĲ��ֽ����ٴεĴ���
//! �Դ�С����״�����ƣ����ɲο�����
int CPlateLocate::sobelSecSearchPart(Mat& bound, Point2f refpoint,
                                     vector<RotatedRect>& outRects) {
  Mat bound_threshold;

  ////!
  ///�ڶ��β�����һ�ξ�ϸ������Ե��ǵõ�����Ӿ���֮���ͼ����sobel�õ���ֵͼ��
  sobelOperT(bound, bound_threshold, 3, 6, 2);

  ////��ֵ��ȥ�����ߵı߽�

  // Mat mat_gray;
  // cvtColor(bound,mat_gray,CV_BGR2GRAY);

  // bound_threshold = mat_gray.clone();
  ////threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU +
  /// CV_THRESH_BINARY);
  // int w = mat_gray.cols;
  // int h = mat_gray.rows;
  // Mat tmp = mat_gray(Rect(w*0.15,h*0.2,w*0.6,h*0.6));
  // int threadHoldV = ThresholdOtsu(tmp);
  // threshold(mat_gray, bound_threshold,threadHoldV, 255, CV_THRESH_BINARY);

  Mat tempBoundThread = bound_threshold.clone();
  ////
  clearLiuDingOnly(tempBoundThread);

  int posLeft = 0, posRight = 0;
  if (bFindLeftRightBound(tempBoundThread, posLeft, posRight)) {
    //�ҵ������߽����������޲�����
    if (posRight != 0 && posLeft != 0 && posLeft < posRight) {
      int posY = int(bound_threshold.rows * 0.5);
      for (int i = posLeft + (int)(bound_threshold.rows * 0.1);
           i < posRight - 4; i++) {
        bound_threshold.data[posY * bound_threshold.cols + i] = 255;
      }
    }

    imwrite("resources/image/tmp/repaireimg1.jpg", bound_threshold);

    //���ߵ�����Ҫ
    for (int i = 0; i < bound_threshold.rows; i++) {
      bound_threshold.data[i * bound_threshold.cols + posLeft] = 0;
      bound_threshold.data[i * bound_threshold.cols + posRight] = 0;
    }
    imwrite("resources/image/tmp/repaireimg2.jpg", bound_threshold);
  }

  vector<vector<Point> > contours;
  findContours(bound_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // ��ȡ�ⲿ����
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();

  vector<RotatedRect> second_rects;
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));
    second_rects.push_back(mr);
    ++itc;
  }

  for (size_t i = 0; i < second_rects.size(); i++) {
    RotatedRect roi = second_rects[i];
    if (verifySizes(roi)) {
      Point2f refcenter = roi.center + refpoint;
      Size2f size = roi.size;
      float angle = roi.angle;

      RotatedRect refroi(refcenter, size, angle);
      outRects.push_back(refroi);
    }
  }

  return 0;
}

//! Sobel�ڶ�������
//! �Դ�С����״�����ƣ����ɲο�����
int CPlateLocate::sobelSecSearch(Mat& bound, Point2f refpoint,
                                 vector<RotatedRect>& outRects) {
  Mat bound_threshold;

  //!
  //�ڶ��β�����һ�ξ�ϸ������Ե��ǵõ�����Ӿ���֮���ͼ����sobel�õ���ֵͼ��
  sobelOper(bound, bound_threshold, 3, 10, 3);

  // Mat tempBoundThread = bound_threshold.clone();
  //////
  // tempBoundThread = clearLiuDing(tempBoundThread);

  // int posLeft = 0,posRight = 0;
  // if (bFindLeftRightBound2(tempBoundThread,posLeft,posRight))
  //{
  //	//�ҵ������߽����������޲�����
  //	if (posRight !=0 && posLeft != 0 && posLeft < posRight)
  //	{
  //		int posY = bound_threshold.rows*0.5;
  //		for (int i=posLeft+bound_threshold.rows*0.1;i<posRight-4;i++)
  //		{
  //			bound_threshold.data[posY*bound_threshold.step[0]+i] =
  // 255;
  //		}

  //	}

  //	imwrite("resources/image/tmp/repaireimg1.jpg",bound_threshold);

  //	//���ߵ�����Ҫ
  //	for (int i=0;i<bound_threshold.rows;i++)
  //	{
  //		bound_threshold.data[i*bound_threshold.step[0]+posLeft] = 0;
  //		bound_threshold.data[i*bound_threshold.step[0]+posRight] = 0;
  //	}
  //	imwrite("resources/image/tmp/repaireimg2.jpg",bound_threshold);
  //}

  imwrite("resources/image/tmp/sobelSecSearch.jpg", bound_threshold);

  vector<vector<Point> > contours;
  findContours(bound_threshold,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // ��ȡ�ⲿ����
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();

  vector<RotatedRect> second_rects;
  while (itc != contours.end()) {
    RotatedRect mr = minAreaRect(Mat(*itc));
    second_rects.push_back(mr);
    ++itc;
  }

  for (size_t i = 0; i < second_rects.size(); i++) {
    RotatedRect roi = second_rects[i];
    if (verifySizes(roi)) {
      Point2f refcenter = roi.center + refpoint;
      Size2f size = roi.size;
      float angle = roi.angle;

      RotatedRect refroi(refcenter, size, angle);
      outRects.push_back(refroi);
    }
  }

  return 0;
}

//! Sobel����//��ͼ��ָ��ʴ�����͵Ĳ���
//! �����ɫͼ�������ֵ��ͼ��
int CPlateLocate::sobelOper(const Mat& in, Mat& out, int blurSize, int morphW,
                            int morphH) {
  Mat mat_blur;
  mat_blur = in.clone();
  GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);

  Mat mat_gray;
  if (mat_blur.channels() == 3)
    cvtColor(mat_blur, mat_gray, CV_RGB2GRAY);
  else
    mat_gray = mat_blur;

  // equalizeHist(mat_gray, mat_gray);

  int scale = SOBEL_SCALE;
  int delta = SOBEL_DELTA;
  int ddepth = SOBEL_DDEPTH;

  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;
  // ��X  soble
  Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);
  // ��Y  soble
  // Sobel(mat_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  // convertScaleAbs(grad_y, abs_grad_y);
  // ������Ȩֵ���
  // ��ΪY�����Ȩ����0������ڴ˾Ͳ��ټ���Y�����sobel��
  Mat grad;
  addWeighted(abs_grad_x, SOBEL_X_WEIGHT, 0, 0, 0, grad);
  // �ָ�
  Mat mat_threshold;
  double otsu_thresh_val =
      threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
  // ��ʴ������
  Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
  morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

  out = mat_threshold;

  if (0) {
    imshow("sobelOper", out);
    waitKey(0);
    destroyWindow("sobelOper");
  }

  return 0;
}

void DeleteNotArea(Mat& inmat) {
  Mat input_grey;
  cvtColor(inmat, input_grey, CV_BGR2GRAY);

  int w = inmat.cols;
  int h = inmat.rows;

  Mat tmpMat = inmat(Rect_<double>(w * 0.15, h * 0.1, w * 0.7, h * 0.7));
  //�жϳ�����ɫ�Դ�ȷ��threshold����
  Color plateType = getPlateType(tmpMat, true);
  Mat img_threshold;
  if (BLUE == plateType) {
    img_threshold = input_grey.clone();
    Mat tmp = input_grey(Rect_<double>(w * 0.15, h * 0.15, w * 0.7, h * 0.7));
    int threadHoldV = ThresholdOtsu(tmp);

    threshold(input_grey, img_threshold, threadHoldV, 255, CV_THRESH_BINARY);
    // threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU +
    // CV_THRESH_BINARY);

    imwrite("resources/image/tmp/inputgray2.jpg", img_threshold);

  } else if (YELLOW == plateType) {
    img_threshold = input_grey.clone();
    Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
    int threadHoldV = ThresholdOtsu(tmp);

    threshold(input_grey, img_threshold, threadHoldV, 255,
              CV_THRESH_BINARY_INV);

    imwrite("resources/image/tmp/inputgray2.jpg", img_threshold);

    // threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU +
    // CV_THRESH_BINARY_INV);
  } else
    threshold(input_grey, img_threshold, 10, 255,
              CV_THRESH_OTSU + CV_THRESH_BINARY);

  int posLeft = 0;
  int posRight = 0;

  int top = 0;
  int bottom = img_threshold.rows - 1;
  clearLiuDing(img_threshold, top, bottom);
  if (bFindLeftRightBound1(img_threshold, posLeft, posRight)) {
    inmat = inmat(Rect(posLeft, top, w - posLeft, bottom - top));

    /*int posY = inmat.rows*0.5*inmat.cols;
    for (int i=posLeft;i<posRight;i++)
    {

    inmat.data[posY+i] = 255;
    }
    */
    /*roiRect.x += posLeft;
    roiRect.width -=posLeft;*/
  }
}

//! ��Ťб����
int CPlateLocate::deskew(const Mat& src, const Mat& src_b,
                         vector<RotatedRect>& inRects,
                         vector<CPlate>& outPlates) {
  Mat mat_debug;
  src.copyTo(mat_debug);

  for (size_t i = 0; i < inRects.size(); i++) {
    RotatedRect roi_rect = inRects[i];

    float r = (float)roi_rect.size.width / (float)roi_rect.size.height;
    float roi_angle = roi_rect.angle;

    Size roi_rect_size = roi_rect.size;
    if (r < 1) {
      roi_angle = 90 + roi_angle;
      swap(roi_rect_size.width, roi_rect_size.height);
    }

    if (m_debug) {
      Point2f rect_points[4];
      roi_rect.points(rect_points);
      for (int j = 0; j < 4; j++)
        line(mat_debug, rect_points[j], rect_points[(j + 1) % 4],
             Scalar(0, 255, 255), 1, 8);
    }

    // m_angle=60
    if (roi_angle - m_angle < 0 && roi_angle + m_angle > 0) {
      Rect_<float> safeBoundRect;
      bool isFormRect = calcSafeRect(roi_rect, src, safeBoundRect);
      if (!isFormRect) continue;

      Mat bound_mat = src(safeBoundRect);
      Mat bound_mat_b = src_b(safeBoundRect);

      Point2f roi_ref_center = roi_rect.center - safeBoundRect.tl();

      Mat deskew_mat;
      if ((roi_angle - 3 < 0 && roi_angle + 3 > 0) || 90.0 == roi_angle ||
          -90.0 == roi_angle) {
        deskew_mat = bound_mat;
      } else {
        // �Ƕ���5��60��֮��ģ�������Ҫ��ת rotation
        Mat rotated_mat;
        Mat rotated_mat_b;

        if (!rotation(bound_mat, rotated_mat, roi_rect_size, roi_ref_center,
                      roi_angle))
          continue;

        if (!rotation(bound_mat_b, rotated_mat_b, roi_rect_size, roi_ref_center,
                      roi_angle))
          continue;

        // ���ͼƬƫб������Ҫ�ӽ�ת�� affine
        double roi_slope = 0;
        // imshow("1roated_mat",rotated_mat);
        // imshow("rotated_mat_b",rotated_mat_b);
        if (isdeflection(rotated_mat_b, roi_angle, roi_slope)) {

          affine(rotated_mat, deskew_mat, roi_slope);
        } else
          deskew_mat = rotated_mat;
      }

      Mat plate_mat;
      plate_mat.create(HEIGHT, WIDTH, TYPE);

      // haitungaga���ӣ�ɾ���������������Ӱ����25%��������λ��
      DeleteNotArea(deskew_mat);

      // �����deskew_mat������һ��ɸѡ
      // ʹ���˾�����ֵ��2.3��6

      if (0) {
        imshow("deskew_mat", deskew_mat);
        waitKey(0);
        destroyWindow("deskew_mat");
      }

      //
      if (deskew_mat.cols * 1.0 / deskew_mat.rows > 2.3 &&
          deskew_mat.cols * 1.0 / deskew_mat.rows < 6) {
        //���ͼ�����������Ҫ���ͼ�񣬶�ͼ�����һ����С���
        if (deskew_mat.cols >= WIDTH || deskew_mat.rows >= HEIGHT)
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
        else
          resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_CUBIC);

        if (0) {
          imshow("plate_mat", plate_mat);
          waitKey(0);
          destroyWindow("plate_mat");
        }

        CPlate plate;
        plate.setPlatePos(roi_rect);
        plate.setPlateMat(plate_mat);
        outPlates.push_back(plate);
      }
    }
  }

  if (0) {
    imshow("mat_debug", mat_debug);
    waitKey(0);
    destroyWindow("mat_debug");
  }

  return 0;
}

//! ��ת����
bool CPlateLocate::rotation(Mat& in, Mat& out, const Size rect_size,
                            const Point2f center, const double angle) {
  Mat in_large;
  in_large.create(int(in.rows * 1.5), int(in.cols * 1.5), in.type());

  float x = in_large.cols / 2 - center.x > 0 ? in_large.cols / 2 - center.x : 0;
  float y = in_large.rows / 2 - center.y > 0 ? in_large.rows / 2 - center.y : 0;

  float width = x + in.cols < in_large.cols ? in.cols : in_large.cols - x;
  float height = y + in.rows < in_large.rows ? in.rows : in_large.rows - y;

  /*assert(width == in.cols);
  assert(height == in.rows);*/

  if (width != in.cols || height != in.rows) return false;

  Mat imageRoi = in_large(Rect_<float>(x, y, width, height));
  addWeighted(imageRoi, 0, in, 1, 0, imageRoi);

  Point2f center_diff(in.cols / 2.f, in.rows / 2.f);
  Point2f new_center(in_large.cols / 2.f, in_large.rows / 2.f);

  Mat rot_mat = getRotationMatrix2D(new_center, angle, 1);

  /*imshow("in_copy", in_large);
  waitKey(0);*/

  Mat mat_rotated;
  warpAffine(in_large, mat_rotated, rot_mat, Size(in_large.cols, in_large.rows),
             CV_INTER_CUBIC);

  /*imshow("mat_rotated", mat_rotated);
  waitKey(0);*/

  Mat img_crop;
  getRectSubPix(mat_rotated, Size(rect_size.width, rect_size.height),
                new_center, img_crop);

  out = img_crop;

  /*imshow("img_crop", img_crop);
  waitKey(0);*/

  return true;
}

//! �Ƿ�ƫб
//! �����ֵ��ͼ������жϽ��
bool CPlateLocate::isdeflection(const Mat& in, const double angle,
                                double& slope) { /*imshow("in",in);
                                                waitKey(0);*/
  int nRows = in.rows;
  int nCols = in.cols;

  assert(in.channels() == 1);

  int comp_index[3];
  int len[3];

  comp_index[0] = nRows / 4;
  comp_index[1] = nRows / 4 * 2;
  comp_index[2] = nRows / 4 * 3;

  const uchar* p;

  for (int i = 0; i < 3; i++) {
    int index = comp_index[i];
    p = in.ptr<uchar>(index);

    int j = 0;
    int value = 0;
    while (0 == value && j < nCols) value = int(p[j++]);

    len[i] = j;
  }

  // cout << "len[0]:" << len[0] << endl;
  // cout << "len[1]:" << len[1] << endl;
  // cout << "len[2]:" << len[2] << endl;
  // len[0]/len[1]/len[2]������Ӧ����ȡ���Ʊ��ߵ�ֵ�������㳵�Ʊ��ߵ�б��
  double maxlen = max(len[2], len[0]);
  double minlen = min(len[2], len[0]);
  double difflen = abs(len[2] - len[0]);
  // cout << "nCols:" << nCols << endl;

  double PI = 3.14159265;
  // angle�Ǹ���ˮƽ�Ǹ�ֱ�ߵ�б��ת�������ĽǶ�
  double g = tan(angle * PI / 180.0);

  if (maxlen - len[1] > nCols / 32 || len[1] - minlen > nCols / 32) {
    // ���б��Ϊ������ײ����£���֮����
    double slope_can_1 =
        double(len[2] - len[0]) / double(comp_index[1]);  //��ֱ�ߵ�б��
    double slope_can_2 = double(len[1] - len[0]) / double(comp_index[0]);
    double slope_can_3 = double(len[2] - len[1]) / double(comp_index[0]);
    // cout<<"angle:"<<angle<<endl;
    // cout<<"g:"<<g<<endl;
    // cout << "slope_can_1:" << slope_can_1 << endl;
    // cout << "slope_can_2:" << slope_can_2 << endl;
    // cout << "slope_can_3:" << slope_can_3 << endl;
    // if(g>=0)
    slope = abs(slope_can_1 - g) <= abs(slope_can_2 - g) ? slope_can_1
                                                         : slope_can_2;
    // cout << "slope:" << slope << endl;
    return true;
  } else {
    slope = 0;
  }

  return false;
}

//! Ť�����//ͨ��opencv�ķ���任
void CPlateLocate::affine(const Mat& in, Mat& out, const double slope) {
  // imshow("in", in);
  // waitKey(0);
  //�����slope��ͨ���ж��Ƿ���б�ó�������б��
  Point2f dstTri[3];
  Point2f plTri[3];

  float height = (float)in.rows;  //��
  float width = (float)in.cols;   //��
  float xiff = (float)abs(slope) * height;

  if (slope > 0) {
    //��ƫ�ͣ����������ϵ��xiff/2λ��
    plTri[0] = Point2f(0, 0);
    plTri[1] = Point2f(width - xiff - 1, 0);
    plTri[2] = Point2f(0 + xiff, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  } else {
    //��ƫ�ͣ����������ϵ�� -xiff/2λ��
    plTri[0] = Point2f(0 + xiff, 0);
    plTri[1] = Point2f(width - 1, 0);
    plTri[2] = Point2f(0, height - 1);

    dstTri[0] = Point2f(xiff / 2, 0);
    dstTri[1] = Point2f(width - 1 - xiff + xiff / 2, 0);
    dstTri[2] = Point2f(xiff / 2, height - 1);
  }

  Mat warp_mat = getAffineTransform(plTri, dstTri);

  Mat affine_mat;
  affine_mat.create((int)height, (int)width, TYPE);

  if (in.rows > HEIGHT || in.cols > WIDTH)
    warpAffine(in, affine_mat, warp_mat, affine_mat.size(),
               CV_INTER_AREA);  //����任
  else
    warpAffine(in, affine_mat, warp_mat, affine_mat.size(), CV_INTER_CUBIC);

  out = affine_mat;

  // imshow("out", out);
  // waitKey(0);
}

//! ����һ����ȫ��Rect
//! ��������ڣ�����false
bool CPlateLocate::calcSafeRect(const RotatedRect& roi_rect, const Mat& src,
                                Rect_<float>& safeBoundRect) {
  Rect_<float> boudRect = roi_rect.boundingRect();

  // boudRect�����ϵ�x��y�п���С��0
  float tl_x = boudRect.x > 0 ? boudRect.x : 0;
  float tl_y = boudRect.y > 0 ? boudRect.y : 0;
  // boudRect�����µ�x��y�п��ܴ���src�ķ�Χ
  float br_x = boudRect.x + boudRect.width < src.cols
                   ? boudRect.x + boudRect.width - 1
                   : src.cols - 1;
  float br_y = boudRect.y + boudRect.height < src.rows
                   ? boudRect.y + boudRect.height - 1
                   : src.rows - 1;

  float roi_width = br_x - tl_x;
  float roi_height = br_y - tl_y;

  if (roi_width <= 0 || roi_height <= 0) return false;

  // �½�һ��mat��ȷ����ַ��Խ�磬�Է�mat��λroiʱ���쳣
  safeBoundRect = Rect_<float>(tl_x, tl_y, roi_width, roi_height);

  return true;
}

// !������ɫ��Ϣ�ĳ��ƶ�λ
int CPlateLocate::plateColorLocate(Mat src, vector<CPlate>& candPlates,
                                   int index) {
  vector<RotatedRect> rects_color_blue;
  vector<RotatedRect> rects_color_yellow;
  vector<CPlate> plates;
  Mat src_b;

  // ������ɫ����
  // ������ɫƥ�䳵��
  colorSearch(src, BLUE, src_b, rects_color_blue, index);
  // ���п�Ťб����
  deskew(src, src_b, rects_color_blue, plates);

  // ���һ�ɫ����
  colorSearch(src, YELLOW, src_b, rects_color_yellow, index);
  deskew(src, src_b, rects_color_yellow, plates);

  for (size_t i = 0; i < plates.size(); i++) {
    candPlates.push_back(plates[i]);
  }
  return 0;
}

//! Sobel����
//! �����ɫͼ�������ֵ��ͼ��
int CPlateLocate::sobelOperT(const Mat& in, Mat& out, int blurSize, int morphW,
                             int morphH) {
  Mat mat_blur;
  mat_blur = in.clone();
  GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);

  Mat mat_gray;
  if (mat_blur.channels() == 3)
    cvtColor(mat_blur, mat_gray, CV_BGR2GRAY);
  else
    mat_gray = mat_blur;

  imwrite("resources/image/tmp/grayblure.jpg", mat_gray);

  // equalizeHist(mat_gray, mat_gray);

  int scale = SOBEL_SCALE;
  int delta = SOBEL_DELTA;
  int ddepth = SOBEL_DDEPTH;

  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(grad_x, abs_grad_x);

  //��ΪY�����Ȩ����0������ڴ˾Ͳ��ټ���Y�����sobel��
  Mat grad;
  addWeighted(abs_grad_x, 1, 0, 0, 0, grad);

  imwrite("resources/image/tmp/graygrad.jpg", grad);

  Mat mat_threshold;
  double otsu_thresh_val =
      threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

  imwrite("resources/image/tmp/grayBINARY.jpg", mat_threshold);

  Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
  morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

  imwrite("resources/image/tmp/phologyEx.jpg", mat_threshold);

  out = mat_threshold;

  return 0;
}

// !���ڴ�ֱ�����ĳ��ƶ�λ
int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate>& candPlates,
                                   int index) {
  vector<RotatedRect> rects_sobel;
  vector<RotatedRect> rects_sobel_sel;
  vector<CPlate> plates;

  vector<Rect_<float> > bound_rects;

  // Sobel��һ�δ�������
  sobelFrtSearch(src, bound_rects);

  vector<Rect_<float> > bound_rects_part;

  //�Բ�����Ҫ������������չ
  for (size_t i = 0; i < bound_rects.size(); i++) {
    float fRatio = bound_rects[i].width * 1.0f / bound_rects[i].height;
    if (fRatio < 3.0 && fRatio > 1.0 && bound_rects[i].height < 120) {
      Rect_<float> itemRect = bound_rects[i];
      //���ȹ�С��������չ
      itemRect.x = itemRect.x - itemRect.height * (4 - fRatio);
      if (itemRect.x < 0) {
        itemRect.x = 0;
      }
      itemRect.width = itemRect.width + itemRect.height * 2 * (4 - fRatio);
      if (itemRect.width + itemRect.x >= src.cols) {
        itemRect.width = src.cols - itemRect.x;
      }

      itemRect.y = itemRect.y - itemRect.height * 0.08f;
      itemRect.height = itemRect.height * 1.16f;

      bound_rects_part.push_back(itemRect);
    }
  }
  //�Զ��ѵĲ��ֽ��ж��δ���
  for (size_t i = 0; i < bound_rects_part.size(); i++) {
    Rect_<float> bound_rect = bound_rects_part[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    // Sobel�ڶ��ξ�ϸ����(����)
    sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
  }

  for (size_t i = 0; i < bound_rects.size(); i++) {
    Rect_<float> bound_rect = bound_rects[i];
    Point2f refpoint(bound_rect.x, bound_rect.y);

    float x = bound_rect.x > 0 ? bound_rect.x : 0;
    float y = bound_rect.y > 0 ? bound_rect.y : 0;

    float width =
        x + bound_rect.width < src.cols ? bound_rect.width : src.cols - x;
    float height =
        y + bound_rect.height < src.rows ? bound_rect.height : src.rows - y;

    Rect_<float> safe_bound_rect(x, y, width, height);
    Mat bound_mat = src(safe_bound_rect);

    // Sobel�ڶ��ξ�ϸ����
    sobelSecSearch(bound_mat, refpoint, rects_sobel);
    // sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
  }

  Mat src_b;
  sobelOper(src, src_b, 3, 10, 3);

  // ���п�Ťб����
  deskew(src, src_b, rects_sobel, plates);

  for (size_t i = 0; i < plates.size(); i++) candPlates.push_back(plates[i]);

  return 0;
}

// !���ƶ�λ
// !����ɫ��Sobel��λ�ĳ���ȫ�����
int CPlateLocate::plateLocate(Mat src, vector<Mat>& resultVec, int index) {
  vector<CPlate> all_result_Plates;

  plateColorLocate(src, all_result_Plates, index);
  plateSobelLocate(src, all_result_Plates, index);

  for (size_t i = 0; i < all_result_Plates.size(); i++) {
    CPlate plate = all_result_Plates[i];
    resultVec.push_back(plate.getPlateMat());
  }

  return 0;

}



} /*! \namespace easypr*/