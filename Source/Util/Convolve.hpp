#pragma once
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    cv::Mat CenterConvolve(cv::Mat &img, cv::Mat &kernel);
}
