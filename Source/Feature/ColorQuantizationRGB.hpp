#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class ColorQuantizationRGB
    {
        public:
            ColorQuantizationRGB(int rq, int gq, int bq);

            //takes a BGR256 color pixel and returns the bin#
            int binForColor(cv::Vec3b pixel); //Vec3b is BGR ordered

            //iterates over the pixels in a Mat and maps their bins into a 2D int array
            void quantize(cv::Mat src, int** dest);

            //takes an int < rq*gq*bq, and returns an array of color ranges in the following form
            //{b_lower, b_upper, g_lower, g_upper, r_lower, r_upper}, inclusive
            int* getColorsForBin(int bin);

            int getBinCt();

        private:
            int bq; //number of blue bins
            int gq; //number of green bins
            int rq; //number of red bins

            //by default we'll assume they are evenly divided, but eventually user may want to decide
            int* b_ranges;
            int* g_ranges;
            int* r_ranges;
    };
}
