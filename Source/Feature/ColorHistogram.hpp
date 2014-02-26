#include "HistogramFeature.hpp"
#include "../Util/ColorQuantizationBGR.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorHistogram : public HistogramFeature
    {
        public:
            std::vector<double> Compute(cv::Mat &img);
            ColorHistogram();
            ColorHistogram(ColorQuantizationBGR space);
        private:
            ColorQuantizationBGR quantization = ColorQuantizationBGR(4,4,4); //default 64 bin quantization
    };
}
