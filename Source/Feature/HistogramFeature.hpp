#pragma once
#include <vector>

namespace cv 
{
    class Mat;
}

namespace ColorTextureShape
{
    class HistogramFeature
    {
        public: 
            virtual std::vector<double> Compute(cv::Mat &img) = 0;
    };
}
