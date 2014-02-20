#include "HistogramFeature.hpp"
#include "../Util/ColorQuantizationBGR.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorCorrelogram : public HistogramFeature
    {
        public:
            ColorCorrelogram(int d);
            ColorCorrelogram(int d, ColorQuantizationBGR space);
            ~ColorCorrelogram();

            double*** computeFullCorrelogram(cv::Mat img);
            double** computeAutoCorrelogram(cv::Mat img);

            void deleteCorrelogram(double*** correlogram);
            void deleteAutoCorrelogram(double** autocorrelogram);

            int getDistance();
            int getColorCt();
            int getAutoCorrelogramVectorSize();
            int getCorrelogramVectorSize(); //potentially need long if color space and distance are too big

        private:
            ColorQuantizationBGR quantization = ColorQuantizationBGR(4,4,4); //default 64 bin quantization
            bool default_space = true;
            int distance;

            int*** buildLambdaTable(int color, char direction, int** q_img, int rows, int columns);
            void deleteLambdaTable(int rows, int columns, int*** lambda);
            int uGammaValue(int color1, int color2, int** q_img, int rows, int columns, int k, int**** lambda_tables);
    };
}
