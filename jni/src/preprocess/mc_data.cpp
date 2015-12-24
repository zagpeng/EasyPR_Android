// ����Ԥ����ĳ�����Ҫ������������
// 1.��ȡԭ������ rawdata�����������������
// 2.���/ѡ���Ե�ѡȡ�������ݳ�Ϊlearndata��������ݲ������ã�һ������Ϊ1000��10000������1����
#include "easypr/preprocess/mc_data.h"
#include <ctime>
#include "easypr/plate_judge.h"
#include "easypr/plate_locate.h"
#include "easypr/util.h"

#ifdef OS_WINDOWS
#include <io.h>
#endif

namespace easypr {

namespace preprocess {

//std::map<std::string, std::string> mc_map = {
//        {"E00", "δʶ��"},
//        {"A01", "��"},
//        {"A02", "��"},
//        {"A03", "��"},
//        {"A04", "��"},
//        {"B01", "��"},
//        {"B02", "��"},
//        {"B03", "��"},
//        {"B04", "��"},
//        {"B05", "��"},
//        {"S01", "��"},
//        {"S02", "��"},
//        {"S03", "��"},
//        {"S04", "��"},
//        {"S05", "��"},
//        {"S06", "��"},
//        {"S07", "��"},
//        {"S08", "��"},
//        {"S09", "��"},
//        {"S10", "ԥ"},
//        {"S11", "��"},
//        {"S12", "��"},
//        {"S13", "��"},
//        {"S14", "��"},
//        {"S15", "��"},
//        {"S16", "��"},
//        {"S17", "��"},
//        {"S18", "³"},
//        {"S19", "��"},
//        {"S20", "��"},
//        {"S21", "��"},
//        {"S22", "��"},
//        {"J01", "��"},
//        {"J02", "��"},
//        {"J03", "��"},
//        {"J04", "��"},
//        {"J05", "��"},
//        {"J06", "��"},
//        {"J07", "��"},
//        {"J08", "��"},
//        {"J09", "��"},
//        {"J10", "��"}
//};

// ��ȥ�ϲ���ײ����ŵ�ϸ��
cv::Mat cut_top_bottom(const cv::Mat& img) {
  int width = img.size().width;
  int height = img.size().height;
  // TODO: it seems not correctly.
  cv::Rect rect(0, 0, width, int(height * 0.97));
  return img(rect);
}

//std::string code_to_province(const std::string& code) {
//  return (mc_map.find(code) != mc_map.end()) ? mc_map[code] : "��";
//}

//// ͨ��filepath��ȡ���ƺ���
//// �ļ�����ʽ��A01_00000
//std::string plate_from_path(const std::string& path) {
//  auto filename = Utils::getFileName(path);
//  auto code = filename.substr(0, 3);
//  return code_to_province(code) + filename.substr(3);
//}

// ��rawdata��ȡ�������ݵ�learndata��
void create_learn_data(const char* raw_data_folder, const char* out_data_folder,
                       const int how_many /* = 5000 */) {
  assert(raw_data_folder);
  assert(out_data_folder);

  auto files = Utils::getFiles(raw_data_folder);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << raw_data_folder << std::endl;
    return;
  }
  // �������rawdata
  srand(unsigned(time(NULL)));
  std::random_shuffle(files.begin(), files.end());

  int count = 0;
  for (auto f : files) {
    // ѡȡǰhow_many��rawdata������Ϊlearndata
    if (count++ >= how_many) {
      break;
    }
    //��ȡ���ݣ�����ͼƬ����Ԥ����
    cv::Mat img = cv::imread(f);
    img = cut_top_bottom(img);

    std::string save_to(out_data_folder);
    if (*(save_to.end() - 1) != '/') {
      save_to.push_back('/');
    }
    save_to.append(Utils::getFileName(f, true));

    utils::imwrite(save_to, img);
    std::cout << f << " -> " << save_to << std::endl;
  }
  std::cout << "Learn data created successfully!" << std::endl;
}

// ��λ���жϳ������ޣ�������ָ��λ��
void tag_data(const char* source_folder, const char* has_plate_folder,
              const char* no_plate_folder, const char* svm_model) {
  assert(source_folder);
  assert(has_plate_folder);
  assert(no_plate_folder);
  assert(svm_model);

  auto files = Utils::getFiles(source_folder);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << source_folder << std::endl;
    return;
  }

  CPlateLocate locator;
  CPlateJudge judger;

  judger.LoadModel(svm_model);

  for (auto f : files) {
    auto filename = Utils::getFileName(f);
    std::cout << "Tagging: " << f << std::endl;

    // auto plate_string = plate_from_path(f);
    cv::Mat image = cv::imread(f);
    assert(!image.empty());

    std::vector<cv::Mat> maybe_plates;
    locator.plateLocate(image, maybe_plates);

    int plate_index = 0;
    for (auto plate : maybe_plates) {
      char save_to[255] = {0};
      int result = 0;
      judger.plateJudge(plate, result);
      if (result == 1) {
        // it's a plate
        sprintf(save_to, "%s/%s_%d.jpg",
                has_plate_folder, filename.c_str(), plate_index);
        std::cout << "[Y] -> " << save_to << std::endl;
      } else {
        // no plate found
        sprintf(save_to, "%s/%s_%d.jpg",
                no_plate_folder, filename.c_str(), plate_index);
        std::cout << "[N] -> " << save_to << std::endl;
      }
      utils::imwrite(save_to, plate);
      ++plate_index;
    }
  }
}

} // namespace preprocess

} // namespace easypr
