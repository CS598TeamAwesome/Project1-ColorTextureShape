#pragma once

namespace cv 
{
    class Mat;
}

namespace ColorTextureShape
{
    class HistogramFeature
    {
        public: 
            virtual double *Compute(cv::Mat &img) = 0;
    };
}
