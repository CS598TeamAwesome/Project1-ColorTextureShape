#include "HistogramFeature.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorCorrelogram : public HistogramFeature
    {
        public:
            void Compute(cv::Mat image);
    };
}
