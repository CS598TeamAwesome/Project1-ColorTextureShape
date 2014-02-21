#pragma once

#include "HistogramFeature.hpp"
#include <opencv2/opencv.hpp>
#include <functional>

// Defaults taken from optimal findings (Dalal & Triggs CVPR 2005 http://lear.inrialpes.fr/people/triggs/pubs/Dalal-cvpr05.pdf)

namespace ColorTextureShape
{
    typedef std::function<void(std::vector<double> &)> NormStrategy;

    void L2Norm(std::vector<double> &);
    void L1Norm(std::vector<double> &);
    void L1Sqrt(std::vector<double> &);

    class HistogramOfOrientedGradients : public HistogramFeature
    {
        public:
            HistogramOfOrientedGradients(void);
            
            std::vector<double> Compute(cv::Mat &img);
            
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
            
            NormStrategy Norm(void) const;
            void Norm(NormStrategy &n);
            
        private:            
            NormStrategy _Norm = L2Norm;
            
            cv::Size _CellSize = cv::Size(6, 6);
            cv::Size _BlockSize = cv::Size(3, 3);
            int _Bins = 9;
            
            // [-1 0 1] initialized in constructor
            cv::Mat _Mx, _My;
    };
}
