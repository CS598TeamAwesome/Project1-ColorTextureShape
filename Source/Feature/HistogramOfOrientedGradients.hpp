#pragma once

#include "HistogramFeature.hpp"
#include <opencv2/opencv.hpp>

namespace ColorTextureShape
{
    class HistogramOfOrientedGradients : public HistogramFeature
    {
        public:
            HistogramOfOrientedGradients(void);
            
            double *Compute(cv::Mat &img);
            
            cv::Size CellSize(void) const;
            void CellSize(const cv::Size &size);
            
            cv::Size BlockSize(void) const;
            void BlockSize(const cv::Size &size);
            
            int Bins(void) const;
            void Bins(int b);
            
            cv::Mat Mx(void) const;
            void Mx(const cv::Mat &m);
            
            cv::Mat My(void) const;
            void My(const cv::Mat &m);
            
        private:
            cv::Size _CellSize = cv::Size(3, 3);
            cv::Size _BlockSize = cv::Size(3, 3);
            int _Bins = 9;
            
            // [-1 0 1] initialized in constructor
            cv::Mat _Mx, _My;
    };
}
