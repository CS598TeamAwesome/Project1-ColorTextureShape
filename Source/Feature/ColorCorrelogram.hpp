#include "HistogramFeature.hpp"
#include "ColorQuantizationRGB.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorCorrelogram : public HistogramFeature
    {
        public:
            void Compute(cv::Mat image, int distance);
            void Compute(cv::Mat image, int distance, ColorQuantizationRGB quantization);
        private:
            int*** buildLambdaTable(int color, char direction, int** q_img, int rows, int columns, int d);
            int uGammaValue(int color1, int color2, int** q_img, int rows, int columns, int distance, int k, int**** lambda_tables);
    };
}
