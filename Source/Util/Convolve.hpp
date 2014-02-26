#pragma once
#include <opencv2/opencv.hpp>
#include <exception>

namespace ColorTextureShape
{    
    template<class T>
    inline const T &coerce_value(const T &lower, const T &value, const T &upper)
    {
        return std::max(lower, std::min(value, upper));
    }

    void CenterConvolve(cv::Mat &img, cv::Mat &out, cv::Mat &kernel);
}
