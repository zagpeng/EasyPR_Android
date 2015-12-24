// learn data_prepare :
// ����learn data��cpp��learn dataָ����train data��verify data�� test data���ܺͣ��������������ġ�
// learn dataӦ�������ñ�ǩ�����ݣ���˼���ͨ��EasyPR�Զ��������Ժ�ҲҪ��Ϊ������һ��.

// ��Ӧ�ð������µĲ����˳����֯learn data��
// 1.��EasyPR��not label data���д���ͨ��EasyPR�������Գ���ͼƬ���б�ǩ���ŵ���ͬ�ļ����£���
// 2.��ǩ�����֣�һ�����г��Ƶģ�һ�����޳��Ƶģ�
// 2.EasyPR���ɱ�ǩ�Ժ��㻹����Ҫ����ȷ���£���������ȷ��ǩ������ת�Ƶ����Ǹ�ȥ���ļ����£�
// 3.ͨ������Ĳ��裬��ȷ�ĳ���ͼƬ�ͷǳ���ͼƬ�Ѿ����ֱ�ŵ������ļ��£�������hasplate��noplate��
// 4.���������ļ��зŵ�EasyPRĿ¼train/data/plate_detect_svm/learn����
// 5.����EasyPR��ѡ��ѵ����Ŀ¼�µġ����Ƽ�⣨not divide���������������Զ���learn data�ֿ飬ѵ��������

#ifndef EASYPR_MC_DATA_H
#define EASYPR_MC_DATA_H

#include <string>

namespace easypr {

namespace preprocess {

void create_learn_data(const char* raw_data_folder, const char* out_data_folder,
                       const int how_many = 5000);

void tag_data(const char* source_folder, const char* has_plate_folder,
              const char* no_plate_folder, const char* svm_model);

}

}

#endif //EASYPR_MC_DATA_H
