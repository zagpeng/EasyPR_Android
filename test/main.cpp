#include "easypr.h"
#include "easypr/program_options.h"

#include "accuracy.hpp"
#include "chars.hpp"
#include "plate.hpp"

namespace easypr {

namespace demo {

// interactions

int accuracyTestMain() {
  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {"BatchTest Option:", "1. general_test;",
                             "2. native_test;", "3. ����;", NULL};
    Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          accuracyTest("resources/image/general_test");
          break;
        case 2:
          accuracyTest("resources/image/native_test");
          break;
        case 3:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }
  return 0;
}

// int svmMain() {
//  bool isExit = false;
//  while (!isExit) {
//    std::cout << "////////////////////////////////////" << std::endl;
//    const char* options[] = {
//            "SvmTrain Option:",
//            "1. ����learndata(�������뵽��Ļ���������);",
//            "2. ��ǩlearndata;",
//            "3. ���Ƽ��(not divide and train);",
//            "4. ���Ƽ��(not train);",
//            "5. ���Ƽ��(not divide);",
//            "6. ���Ƽ��;",
//            "7. ����;",
//            NULL
//    };
//    Utils::print_str_lines(options);
//    std::cout << "////////////////////////////////////" << std::endl;
//    std::cout << "��ѡ��һ�����:";
//
//    int select = -1;
//    bool isRepeat = true;
//    while (isRepeat) {
//      std::cin >> select;
//      isRepeat = false;
//
//      //Svm svm;
//
//      switch (select) {
//        case 1:
//          // easypr::preprocess::getLearnData();
//          break;
//        case 2:
//          // easypr::preprocess::label_data();
//          break;
//        case 3:
//          svm.train(false, false);
//          break;
//        case 4:
//          svm.train(true, false);
//          break;
//        case 5:
//          svm.train(false, true);
//          break;
//        case 6:
//          svm.train();
//          break;
//        case 7:
//          isExit = true;
//          break;
//        default:
//          std::cout << "�����������������:";
//          isRepeat = true;
//          break;
//      }
//    }
//  }
//  return 0;
//}

int testMain() {
  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {
        "EasyPR Test:", "1. test plate_locate(���ƶ�λ);" /* ���ƶ�λ */,
        "2. test plate_judge(�����ж�);" /* �����ж� */,
        "3. test plate_detect(���Ƽ��);" /* ���Ƽ�⣨�������ƶ�λ�복���жϣ� */,
        "4. test chars_segment(�ַ��ָ�);" /* �ַ��ָ� */,
        "5. test chars_identify(�ַ�����);" /* �ַ����� */,
        "6. test chars_recognise(�ַ�ʶ��);" /* �ַ�ʶ�𣨰����ַ��ָ����ַ����� */,
        "7. test plate_recognize(����ʶ��);" /* ����ʶ�� */,
        "8. test all(����ȫ��);" /* ����ȫ�� */, "9. ����;" /* �˳� */, NULL};
    Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          assert(test_plate_locate() == 0);
          break;
        case 2:
          assert(test_plate_judge() == 0);
          break;
        case 3:
          assert(test_plate_detect() == 0);
          break;
        case 4:
          assert(test_chars_segment() == 0);
          break;
        case 5:
          assert(test_chars_identify() == 0);
          break;
        case 6:
          assert(test_chars_recognise() == 0);
          break;
        case 7:
          assert(test_plate_recognize() == 0);
          break;
        case 8:
          assert(test_plate_locate() == 0);
          assert(test_plate_judge() == 0);
          assert(test_plate_detect() == 0);

          assert(test_chars_segment() == 0);
          assert(test_chars_identify() == 0);
          assert(test_chars_recognise() == 0);

          assert(test_plate_recognize() == 0);
          break;
        case 9:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }

  return 0;
}

}  // namespace demo

}  // namespace easypr

void command_line_handler(int argc, const char* argv[]) {
  program_options::Generator options;

  options.add_subroutine("svm", "svm operations").make_usage("Usage:");
  options("h,help", "show help information");
  options(",svm", "resources/model/svm.xml",
          "the svm model file,"
          " this option is used for '--tag'(required)"
          " and '--train'(save svm model to) functions");
  // create
  options(
      ",create",
      "create learn data, this function "
      "will intercept (--max) raw images (--in) and preprocess into (--out)");
  options("i,in", "", "where is the raw images");
  options("o,out", "", "where to put the preprocessed images");
  options("m,max", "5000", "how many learn data would you want to create");
  // tag
  options(",tag",
          "tag learn data, this function "
          "will find plate blocks in your images(--source) "
          "as well as classify them into (--has) and (--no)");
  options("s,source", "", "where is your images to be classified");
  options(",has", "", "put plates in this folder");
  options(",no", "", "put images without plate in this folder");
  // train
  options(",train",
          "train given data, "
          "including the forward(has plate) and the inverse(no plate).");
  options(",has-plate", "", "where is the forward data");
  options(",no-plate", "", "where is the inverse data");
  options(",divide",
          "whether divide train data into two parts by --percentage or not");
  options(",percentage", "0.7",
          "70% train data will be used for training,"
          " others will be used for testing");
  options(",not-train", "don't train again, run testing directly");

  options.add_subroutine("locate", "locate plates in an image")
      .make_usage("Usage:");
  options("h,help", "show help information");
  options("f,file", "", "the target picture which contains one or more plates");

  options.add_subroutine(
             "judge", "determine whether an image block is the license plate")
      .make_usage("Usage:");
  options("h,help", "show help information");
  options("f,file", "the target image block");
  options(",svm", "resources/model/svm.xml", "the svm model file");

  options.add_subroutine("recognize", "plate recognition").make_usage("Usage:");
  options("h,help", "show help information");
  options("p,path", "", "where is the target picture or target folder");
  options("b,batch", "do batch recognition, if set, --path means a folder");
  options("c,color", "returns the plate color, blue or yellow");
  options(",svm", "resources/model/svm.xml", "the svm model file");
  options(",ann", "resources/model/ann.xml", "the ann model file");

  auto parser = options.make_parser();

  try {
    parser->parse(argc, argv);
  } catch (const std::exception& err) {
    std::cout << err.what() << std::endl;
    return;
  }

  auto subname = parser->get_subroutine_name();

  if (subname == "svm") {
    if (parser->has("help") || argc <= 2) {
      std::cout << options("svm");
      return;
    }

    if (parser->has("create")) {
      assert(parser->has("in"));
      assert(parser->has("out"));
      assert(parser->has("max"));

      auto in = parser->get("in")->val();
      auto out = parser->get("out")->val();
      auto max = parser->get("max")->as<int>();
      easypr::preprocess::create_learn_data(in.c_str(), out.c_str(), max);
    }

    if (parser->has("tag")) {
      assert(parser->has("source"));
      assert(parser->has("has"));
      assert(parser->has("no"));
      assert(parser->has("svm"));

      auto source = parser->get("source")->val();
      auto has_path = parser->get("has")->val();
      auto no_path = parser->get("no")->val();
      auto svm = parser->get("svm")->val();
      easypr::preprocess::tag_data(source.c_str(), has_path.c_str(),
                                   no_path.c_str(), svm.c_str());
      std::cout << "Tagging finished, check out output images "
                << "and classify the wrong images manually." << std::endl;
    }

    if (parser->has("train")) {
      assert(parser->has("has-plate"));
      assert(parser->has("no-plate"));
      assert(parser->has("percentage"));
      assert(parser->has("svm"));

      auto forward_data_path = parser->get("has-plate")->val();
      auto inverse_data_path = parser->get("no-plate")->val();
      bool divide = parser->has("divide");
      auto percentage = parser->get("percentage")->as<float>();
      auto svm_model = parser->get("svm")->val();
      bool not_train = parser->has("not-train");

      easypr::Svm svm(forward_data_path.c_str(), inverse_data_path.c_str());

      svm.train(divide, percentage, !not_train, svm_model.c_str());
    }

    return;
  } else if (subname == "locate") {
    if (parser->has("help") || argc <= 2) {
      std::cout << options("locate");
      return;
    }

    if (parser->has("file")) {
      easypr::api::plate_locate(parser->get("file")->val().c_str());
      std::cout << "finished, results can be found in tmp/" << std::endl;
    }

    return;
  } else if (subname == "judge") {
    if (parser->has("help") || argc <= 2) {
      std::cout << options("judge");
      std::cout << "Note that the input image's size should "
                << "be the same as the one you gived to svm train."
                << std::endl;
      return;
    }

    if (parser->has("file")) {
      assert(parser->has("file"));
      assert(parser->has("svm"));

      auto image = parser->get("file")->val();
      auto svm = parser->get("svm")->val();

      const char* true_or_false[2] = {"false", "true"};

      std::cout
          << true_or_false[easypr::api::plate_judge(image.c_str(), svm.c_str())]
          << std::endl;
    }

    return;
  } else if (subname == "recognize") {
    if (parser->has("help") || argc <= 2) {
      std::cout << options("recognize");
      return;
    }

    if (parser->has("path")) {
      if (parser->has("batch")) {
        // batch testing
        auto folder = parser->get("path")->val();
        easypr::demo::accuracyTest(folder.c_str());
      } else {
        // single testing
        auto image = parser->get("path")->val();

        if (parser->has("color")) {
          // return plate color
          const char* colors[2] = {"blue", "yellow"};
          std::cout << colors[easypr::api::get_plate_color(image.c_str())]
                    << std::endl;
        } else {
          // return strings
          auto svm = parser->get("svm")->val();
          auto ann = parser->get("ann")->val();

          auto results = easypr::api::plate_recognize(image.c_str(),
                                                      svm.c_str(), ann.c_str());
          for (auto s : results) {
            std::cout << s << std::endl;
          }
        }
      }
    } else {
      std::cout << "option 'file' cannot be empty." << std::endl;
    }
    return;
  }

  // no case matched, print all commands.
  std::cout << "There are several sub commands listed below, "
            << "choose one by typing:\n\n"
            << "    " << easypr::Utils::getFileName(argv[0])
            << " command [options]\n\n"
            << "The commands are:\n" << std::endl;
  auto subs = options.get_subroutine_list();
  for (auto sub : subs) {
    fprintf(stdout, "%s    %s\n", sub.first.c_str(), sub.second.c_str());
  }
  std::cout << std::endl;
}

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    // handle command line execution.
    command_line_handler(argc, argv);
    return 0;
  }

  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {"EasyPR Option:", "1. ����;", "2. ��������;",
                             "3. SVMѵ��;", "4. ANNѵ��(δ����);",
                             "5. GDTS����;", "6. �����Ŷ�;", "7. ��л����;",
                             "8. �˳�;", NULL};
    easypr::Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          easypr::demo::testMain();
          break;
        case 2:
          easypr::demo::accuracyTestMain();
          break;
        case 3:
          // easypr::demo::svmMain();
          std::cout << "Run \"easypr_test svm\" for more usage." << std::endl;
          break;
        case 4:
          // TODO
          break;
        case 5:
          easypr::preprocess::generate_gdts();
          break;
        case 6: {
          // �����Ŷ�;
          std::cout << std::endl;
          const char* recruitment[] = {
              "����EasyPR�Ŷ�Ŀǰ��һ��5�����ҵ�С���ڽ���EasyPR�����汾�Ŀ���"
              "������",
              "�����Ա���Ŀ����Ȥ������Ը��Ϊ��Դ����һ�����������Ǻܻ�ӭ���"
              "���롣",
              "Ŀǰ��Ƹ����Ҫ�˲��ǣ����ƶ�λ��ͼ��ʶ�����ѧϰ����վ�������"
              "�����ţ�ˡ�",
              "���������Լ������������뷢�ʼ�����ַ(easypr_dev@163.com)"
              "���ڴ���ļ��룡",
              NULL};
          easypr::Utils::print_str_lines(recruitment);
          std::cout << std::endl;
          break;
        }
        case 7: {
          // ��л����
          std::cout << std::endl;
          const char* thanks[] = {
              "����Ŀ�ڽ�������У��ܵ��˺ܶ��˵İ��������������ǶԱ���Ŀ����ͻ"
              "�����׵�",
              "(���װ������潨�飬������ţ������ṩ�ȵ�,������ʱ��˳��)��",
              "taotao1233�����ɽ���ƴ�����jsxyhelu�������һ��(zhoushiwei)��",
              "ѧϰ�ܶ���Ԭ��־��ʥ��Сʯ����goldriver��Micooz������ʱ�⣬",
              "Rain Wang��ahccoms����ҹ�䳾������¸�",
              "���кܶ��ͬѧ�Ա���ĿҲ�����˹�����֧�֣��ڴ�Ҳһ����ʾ��ϵ�л"
              "�⣡",
              NULL};
          easypr::Utils::print_str_lines(thanks);
          std::cout << std::endl;
          break;
        }
        case 8:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }
  return 0;
}
