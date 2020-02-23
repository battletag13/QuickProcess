#include <algorithm>
#include <iostream>
#include <node/node_api.h>
#include <string>
#include <vector>

#include "opencv2/opencv.hpp"

const double SATURATION_SLIDER_MAX = 100;

const double WARM_FACTOR = 1.22;
const double COOL_FACTOR = 1.42;
const double CREAMA_DESAT_FACTOR = 0.7;
const double CREAMA_MIN_SAT = 0.05;
const double LARK_BRIGHTNESS_FACTOR = 1.12;
const double LARK_INTENSITY_FACTOR = 1.14;
const double LARK_MAX_BRIGHT = 230;
const double RISE_YELLOW_FACTOR = 1.2;

napi_value Brightness(napi_env env, napi_callback_info info) {
  napi_value toreturn;

  napi_value args[3];
  size_t argc = 3;
  size_t pathLength;

  size_t bufSize(512);
  char *buf = new char[bufSize];

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_get_value_string_utf8(env, args[0], buf, bufSize, &pathLength);

  std::string fileName(buf);
  delete[] buf;
  buf = new char[bufSize];

  napi_get_value_string_utf8(env, args[1], buf, bufSize, &pathLength);
  std::string folder(buf);
  std::string newName("brightness-" + fileName);

  int relativeChange;
  napi_get_value_int32(env, args[2], &relativeChange);

  cv::Mat img(cv::imread(folder + "/" + fileName));

  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      pixel += cv::Vec3i(relativeChange, relativeChange, relativeChange);

      pixel[0] = std::min(std::max(static_cast<unsigned char>(0), pixel[0]),
                          static_cast<unsigned char>(255));
      pixel[1] = std::min(std::max(static_cast<unsigned char>(0), pixel[1]),
                          static_cast<unsigned char>(255));
      pixel[2] = std::min(std::max(static_cast<unsigned char>(0), pixel[2]),
                          static_cast<unsigned char>(255));
    }
  cv::imwrite(folder + "/" + newName, img);

  napi_create_string_utf8(env, newName.c_str(), newName.length(), &toreturn);
  delete[] buf;
  return toreturn;
}
napi_value Saturation(napi_env env, napi_callback_info info) {
  napi_value toreturn;

  napi_value args[3];
  size_t argc = 3;
  size_t pathLength;

  size_t bufSize(512);
  char *buf = new char[bufSize];

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_get_value_string_utf8(env, args[0], buf, bufSize, &pathLength);

  std::string fileName(buf);
  delete[] buf;
  buf = new char[bufSize];

  napi_get_value_string_utf8(env, args[1], buf, bufSize, &pathLength);
  std::string folder(buf);
  std::string newName("saturation-" + fileName);

  int relativeChange;
  napi_get_value_int32(env, args[2], &relativeChange);

  cv::Mat img(cv::imread(folder + "/" + fileName));
  std::vector<cv::Mat> hsv(3);
  cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
  cv::split(img, hsv);

  hsv[1] *= relativeChange / SATURATION_SLIDER_MAX + 1.0;

  cv::merge(hsv, img);
  cv::cvtColor(img, img, cv::COLOR_HSV2BGR);
  cv::imwrite(folder + "/" + newName, img);

  napi_create_string_utf8(env, newName.c_str(), newName.length(), &toreturn);
  delete[] buf;
  return toreturn;
}

struct ImageInfo {
  ImageInfo(napi_value toreturn, std::string fileName, std::string folder,
            std::string newName)
      : toreturn(toreturn), fileName(fileName), folder(folder),
        newName(newName) {}
  napi_value toreturn;
  std::string fileName;
  std::string folder;
  std::string newName;
};

ImageInfo getImageInfo(napi_env env, napi_callback_info info,
                       const char *prefix) {
  napi_value toreturn;

  napi_value args[2];
  size_t argc = 2;
  size_t pathLength;

  size_t bufSize(512);
  char *buf = new char[bufSize];

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_get_value_string_utf8(env, args[0], buf, bufSize, &pathLength);

  std::string fileName(buf);
  delete[] buf;
  buf = new char[bufSize];

  napi_get_value_string_utf8(env, args[1], buf, bufSize, &pathLength);
  std::string folder(buf);
  std::string newName(prefix + fileName);
  delete[] buf;

  return ImageInfo(toreturn, fileName, folder, newName);
}

void clampValue(double &val, double min, double max) {
  val = std::min(std::max(min, val), max);
}

napi_value Warm(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "warm-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));

  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double red = pixel[2] * WARM_FACTOR;
      clampValue(red, 0, 255);
      pixel[2] = red;
    }

  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}
napi_value Cool(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "cool-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));

  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double blue = pixel[0] * COOL_FACTOR;
      clampValue(blue, 0, 255);
      pixel[0] = blue;
    }

  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}
napi_value Grey(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "grey-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));
  cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}
napi_value Creama(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "creama-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));

  cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double saturation = pixel[1] * CREAMA_DESAT_FACTOR;
      clampValue(saturation, CREAMA_MIN_SAT, 255);
      pixel[1] = saturation;
    }
  cv::cvtColor(img, img, cv::COLOR_HSV2BGR);

  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}
napi_value Lark(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "lark-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));

  cv::cvtColor(img, img, cv::COLOR_BGR2YUV);
  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double brightness = pixel[0] * LARK_BRIGHTNESS_FACTOR;
      clampValue(brightness, 0, LARK_MAX_BRIGHT);
      pixel[0] = brightness;
    }
  cv::cvtColor(img, img, cv::COLOR_YUV2BGR);
  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double b = pixel[0] * LARK_INTENSITY_FACTOR;
      double g = pixel[1] * LARK_INTENSITY_FACTOR;
      clampValue(b, 0, 255);
      clampValue(g, 0, 255);
      pixel[0] = b;
      pixel[1] = g;
    }

  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}
napi_value Rise(napi_env env, napi_callback_info info) {
  ImageInfo imageInfo = getImageInfo(env, info, "rise-");
  napi_create_string_utf8(env, imageInfo.newName.c_str(),
                          imageInfo.newName.length(), &imageInfo.toreturn);

  cv::Mat img(cv::imread(imageInfo.folder + "/" + imageInfo.fileName));

  for (int row = 0; row < img.rows; ++row)
    for (int col = 0; col < img.cols; ++col) {
      cv::Vec3b &pixel = img.at<cv::Vec3b>(row, col);
      double g = pixel[1] * RISE_YELLOW_FACTOR;
      double r = pixel[2] * RISE_YELLOW_FACTOR;
      clampValue(g, 0, 255);
      clampValue(r, 0, 255);
      pixel[1] = g;
      pixel[2] = r;
    }

  cv::imwrite(imageInfo.folder + "/" + imageInfo.newName, img);

  return imageInfo.toreturn;
}

napi_value init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value brightness, saturation, warm, cool, grey, creama, lark, rise;

  status =
      napi_create_function(env, nullptr, 0, Brightness, nullptr, &brightness);
  status =
      napi_create_function(env, nullptr, 0, Saturation, nullptr, &saturation);
  status = napi_create_function(env, nullptr, 0, Warm, nullptr, &warm);
  status = napi_create_function(env, nullptr, 0, Cool, nullptr, &cool);
  status = napi_create_function(env, nullptr, 0, Grey, nullptr, &grey);
  status = napi_create_function(env, nullptr, 0, Creama, nullptr, &creama);
  status = napi_create_function(env, nullptr, 0, Lark, nullptr, &lark);
  status = napi_create_function(env, nullptr, 0, Rise, nullptr, &rise);
  if (status != napi_ok)
    return nullptr;

  status = napi_set_named_property(env, exports, "brightness", brightness);
  status = napi_set_named_property(env, exports, "saturation", saturation);
  status = napi_set_named_property(env, exports, "warm", warm);
  status = napi_set_named_property(env, exports, "cool", cool);
  status = napi_set_named_property(env, exports, "grey", grey);
  status = napi_set_named_property(env, exports, "creama", creama);
  status = napi_set_named_property(env, exports, "lark", lark);
  status = napi_set_named_property(env, exports, "rise", rise);

  if (status != napi_ok)
    return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

// int main() {

//   cv::Mat img(cv::imread("./mbhs.jpg"));
//   // cv::cvtColor(img, img, cv::COLOR_BGR2HSV);

//   // cv::inRange(img, cv::Vec3b(255, 255, 255), cv::Vec3b(255, 255, 255),
//   // img);
//   // cv::cvtColor(img, img, cv::COLOR_HSV2BGR);

//   cv::imshow("Test", img);
//   cv::waitKey(0);
//   cv::destroyAllWindows();
// }
