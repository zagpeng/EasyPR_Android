#include "easypr/chars_recognise.h"
#include "easypr/util.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

CCharsRecognise::CCharsRecognise() {
  m_charsSegment = new CCharsSegment();
  m_charsIdentify = new CCharsIdentify();
}

CCharsRecognise::~CCharsRecognise() {
  SAFE_RELEASE(m_charsSegment);
  SAFE_RELEASE(m_charsIdentify);
}

void CCharsRecognise::LoadANN(string s) {
  m_charsIdentify->LoadModel(s.c_str());
}

string CCharsRecognise::charsRecognise(Mat plate) {
  return m_charsIdentify->charsIdentify(plate);
}

int CCharsRecognise::charsRecognise(Mat plate, string& plateLicense, int index) {
  //�����ַ����鼯��
  vector<Mat> matVec;

  string plateIdentify = "";

  int result = m_charsSegment->charsSegment(plate, matVec, index);
  if (result == 0) {
    int num = matVec.size();
    for (int j = 0; j < num; j++) {
      Mat charMat = matVec[j];
      bool isChinses = false;
      bool isSpeci = false;

      //Ĭ���׸��ַ����������ַ�
      if (j == 0) isChinses = true;
      if (j == 1) isSpeci = true;

      string charcater =
          m_charsIdentify->charsIdentify(charMat, isChinses, isSpeci);

      plateIdentify = plateIdentify + charcater;
    }
  }

  plateLicense = plateIdentify;

  if (plateLicense.size() < 7) {
    return -1;
  }

  return result;
}

} /*! \namespace easypr*/