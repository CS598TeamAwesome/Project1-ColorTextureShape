#include "HistogramFeature.hpp"

namespace ColorTextureShape
{
    class ColorHistogram : public HistogramFeature
    {
        public:
            ColorHistogram(int bq = 4, int gq = 4, int rq = 4);        
            
            std::vector<double> Compute(cv::Mat &img);
            
        private:
            int bq, gq, rq;
    };
}
