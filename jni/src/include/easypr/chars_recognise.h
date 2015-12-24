//////////////////////////////////////////////////////////////////////////
// Name:	    chars_recognise Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsRecognise
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_RECOGNISE_H__
#define __CHARS_RECOGNISE_H__

#include "easypr/chars_segment.h"
#include "easypr/chars_identify.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CCharsRecognise {
 public:
  CCharsRecognise();

  ~CCharsRecognise();

  //! �ַ��ָ���ʶ��
  int charsRecognise(Mat, String&, int index = 0);

  string charsRecognise(Mat plate);

  //! װ��ANNģ��
  void LoadANN(string s);

  //! �Ƿ�������ģʽ
  inline void setCRDebug(int param) { m_charsSegment->setDebug(param); }

  //! ��ȡ����ģʽ״̬
  inline int getCRDebug() { return m_charsSegment->getDebug(); }

  //! ��ó�����ɫ
  inline string getPlateColor(Mat input) const {
    string color = "δ֪";
    Color result = getPlateType(input, true);
    if (BLUE == result) color = "����";
    if (YELLOW == result) color = "����";
    return color;
  }

  //! ���ñ���
  inline void setLiuDingSize(int param) {
    m_charsSegment->setLiuDingSize(param);
  }
  inline void setColorThreshold(int param) {
    m_charsSegment->setColorThreshold(param);
  }
  inline void setBluePercent(float param) {
    m_charsSegment->setBluePercent(param);
  }
  inline float getBluePercent() const {
    return m_charsSegment->getBluePercent();
  }
  inline void setWhitePercent(float param) {
    m_charsSegment->setWhitePercent(param);
  }
  inline float getWhitePercent() const {
    return m_charsSegment->getWhitePercent();
  }

 private:
  //���ַ��ָ�
  CCharsSegment* m_charsSegment;

  //! �ַ�ʶ��
  CCharsIdentify* m_charsIdentify;
};

} /* \namespace easypr  */

#endif /* endif __CHARS_RECOGNISE_H__ */