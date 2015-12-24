//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsIdentify
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CCharsIdentify {
 public:
  CCharsIdentify();

  //! �ַ��ָ�
  string charsIdentify(Mat, bool, bool);
  string charsIdentify(Mat input);

  //! �ַ�����
  int classify(Mat, bool, bool);
  int classify(Mat f);

  //! װ��ANNģ��
  void LoadModel();

  //! װ��ANNģ��
  void LoadModel(string s);

  //! �������ȡģ��·��
  inline void setModelPath(string path) { m_path = path; }
  inline string getModelPath() const { return m_path; }

 private:
  //��ʹ�õ�ANNģ��
  CvANN_MLP ann;

  //! ģ�ʹ洢·��
  string m_path;

  //! �����ߴ�
  int m_predictSize;

  //! ʡ�ݶ�Ӧmap
  std::map<string, string> m_map;
};

} /* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */