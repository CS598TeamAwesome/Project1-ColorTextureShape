#include "HistogramFeature.hpp"
#include "../Util/ColorQuantizationBGR.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorCorrelogram : public HistogramFeature
    {
        public:
            std::vector<double> Compute(cv::Mat &img);

            ColorCorrelogram(int d, bool set_auto);
            ColorCorrelogram(int d, bool set_auto, ColorQuantizationBGR space);

            double*** computeFullCorrelogram(cv::Mat &img);
            double** computeAutoCorrelogram(cv::Mat &img);

            void deleteCorrelogram(double*** correlogram);
            void deleteAutoCorrelogram(double** autocorrelogram);

            int getDistance();
            int getColorCt();
            int getAutoCorrelogramVectorSize();
            int getCorrelogramVectorSize(); //potentially need long if color space and distance are too big

        private:
            ColorQuantizationBGR quantization = ColorQuantizationBGR(4,4,4); //default 64 bin quantization
            bool auto_flag = false;
            int distance;

            int*** buildLambdaTable(int color, char direction, int** q_img, int rows, int columns);
            void deleteLambdaTable(int rows, int columns, int*** lambda);
            int uGammaValue(int color1, int** q_img, int rows, int columns, int k, int*** lambda_h, int*** lambda_v);
    };
}
