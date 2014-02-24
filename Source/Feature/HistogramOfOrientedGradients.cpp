#include "HistogramOfOrientedGradients.hpp"
#include "../Util/Convolve.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>

#ifndef M_PI
#   define M_PI 3.14159265358979323846     
#endif


using namespace ColorTextureShape;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(void)
{
    double gradData[3] = {-1, 0, 1};
    
    _My = cv::Mat(3, 1, CV_64F, gradData).clone();
    _Mx = cv::Mat(1, 3, CV_64F, gradData).clone();
}

std::vector<double> HistogramOfOrientedGradients::Compute(cv::Mat &img)
{
    // 1. Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, CV_RGB2GRAY);
    
    // 2. Convert grayscale to double 
    cv::Mat dblImg;
    gray.convertTo(dblImg, CV_64F);

    // 3. Compute the horizontal and vertical gradients    
    cv::Mat gradH, gradV;
    CenterConvolve(dblImg, gradH, _Mx); 
    CenterConvolve(dblImg, gradV, _My);
    
    // 4. Divide image in cells 
    std::vector<cv::Mat> cellsH;
    std::vector<cv::Mat> cellsV; 
    
    for(int j = 0; j < img.rows - _CellSize.height + 1; j+=_CellSize.height)
    {
        for(int i = 0; i < img.cols - _CellSize.width + 1; i+=_CellSize.width)
        {            
            cellsH.push_back(gradH(cv::Rect(cv::Point(i, j), _CellSize)));
            cellsV.push_back(gradV(cv::Rect(cv::Point(i, j), _CellSize)));
        }
    }
    
    // 5. Compute histogram for each cell 
    double binAngleSize = M_PI / _Bins;
    
    std::vector<std::vector<double>> cellHistograms(cellsH.size());
    std::transform(cellsH.begin(), cellsH.end(), cellsV.begin(), cellHistograms.begin(),
    [this, &binAngleSize](cv::Mat &gradH, cv::Mat &gradV)
    {
        std::vector<double> histogram(_Bins, 0);
        
        for(int j = 0; j < gradH.rows; j++)
        {
            for(int i = 0; i < gradH.cols; i++)
            {    
                double h = gradH.at<double>(j, i);
                double v = gradV.at<double>(j, i);
                
                if(h != 0 || v != 0)
                {
                    double angle = std::abs(std::atan2(v, h)); // using unsigned angles
                    double mag = std::sqrt((h * h) + (v * v));
                    
                    int binBottom = (int)std::min(std::floor(_Bins * angle / M_PI), 8.0);
                    int binTop = (int)std::min(binBottom + 1.0, 8.0);
                    
                    if(binBottom != binTop)
                    {
                        double ratio = ((binTop * binAngleSize) - angle) / binAngleSize;
                    
                        double magBottom = ratio * mag;
                        double magTop = (1.0 - ratio) * mag;
                    
                        histogram[binBottom] += magBottom;
                        histogram[binTop] += magTop;
                    }
                    else
                    {
                        histogram[binBottom] += mag;
                    }
                }
            }
        }
        
        return histogram;
    });
    
    // 6. Divide cells into blocks with as close to 50% overlap as possible (preferring less overlap)
    int blockStepX = (int)std::ceil(_BlockSize.width / 2.0);
    int blockStepY = (int)std::ceil(_BlockSize.height / 2.0);
    
    int cellsX = (int)std::floor(img.cols / _CellSize.width);
    int cellsY = (int)std::floor(img.rows / _CellSize.height);
    
    int blockSize = _Bins * _BlockSize.width * _BlockSize.height;
    std::vector<std::vector<double>> blocks;
    for(int j = 0; j < cellsY - _BlockSize.height + 1; j+= blockStepY)
    {
        for(int i = 0; i < cellsX - _BlockSize.width + 1; i+= blockStepX)
        {        
            std::vector<double> block(blockSize, 0);
           
            for(int cx = 0; cx < _BlockSize.width; cx++)
            {   
                for(int cy = 0; cy < _BlockSize.height; cy++)
                {
                    int cellIndex = (cx + i) + ((cy + j) * cellsX);
                    int blockIndex = cx + (cy * _BlockSize.width);
                    
                    std::vector<double> &hist = cellHistograms[cellIndex];
                    std::copy(hist.begin(), hist.end(), block.begin() + (blockIndex * _Bins));
                }
            }
            
            blocks.push_back(block);
        }
    }
    
    // 7. Normalize histograms for each cell in a block
    for(std::vector<double> &hist : blocks)
    {
        _Norm(hist);
    }
    
    // 8. Concatenate histograms for each block
    std::vector<double> finalDescriptor(blocks.size() * blockSize, 0);
    auto descriptorIter = finalDescriptor.begin();
    for(std::vector<double> &histogram : blocks)
    {
        std::copy(histogram.begin(), histogram.end(), descriptorIter);
        descriptorIter += blockSize;
    }
    
    return finalDescriptor;
}

void ColorTextureShape::L2Norm(std::vector<double> &hist)
{
    double e = std::numeric_limits<double>::epsilon();
    
    double norm2 = std::accumulate(hist.begin(), hist.end(), e * e, [](double accum, double elem) { return accum + elem * elem; });
    double norm = std::sqrt(norm2);
        
    std::transform(hist.begin(), hist.end(), hist.begin(), [&norm](double histVal) { return histVal / norm; });
}

void ColorTextureShape::L2Hys(std::vector<double> &hist)
{
    L2Norm(hist);
    
    auto greaterThanPoint2 = std::bind(std::greater<double>(), std::placeholders::_1, 0.2);
    std::replace_if(hist.begin(), hist.end(), greaterThanPoint2, 0.2);
    
    L2Norm(hist);
}

void ColorTextureShape::L1Norm(std::vector<double> &hist)
{
    double e = std::numeric_limits<double>::epsilon();
    
    double norm = std::accumulate(hist.begin(), hist.end(), e);
    
    std::transform(hist.begin(), hist.end(), hist.begin(), [&norm](double histVal) { return histVal / norm; });
}

void ColorTextureShape::L1Sqrt(std::vector<double> &hist)
{
    L1Norm(hist);
    std::transform(hist.begin(), hist.end(), hist.begin(), std::ptr_fun<double, double>(std::sqrt));
}

cv::Size HistogramOfOrientedGradients::CellSize(void) const
{
    return _CellSize;
}

void HistogramOfOrientedGradients::CellSize(const cv::Size &size)
{
    _CellSize = size;
}

cv::Size HistogramOfOrientedGradients::BlockSize(void) const
{
    return _BlockSize;
}

void HistogramOfOrientedGradients::BlockSize(const cv::Size &size)
{
    _BlockSize = size;
}

int HistogramOfOrientedGradients::Bins(void) const
{
    return _Bins;
}

void HistogramOfOrientedGradients::Bins(int b)
{
    _Bins = b;
}

cv::Mat HistogramOfOrientedGradients::Mx(void) const
{
    return _Mx;
}

void HistogramOfOrientedGradients::Mx(const cv::Mat &m)
{
    _Mx = m;
}

cv::Mat HistogramOfOrientedGradients::My(void) const
{
    return _My;
}

void  HistogramOfOrientedGradients::My(const cv::Mat &m)
{
    _My = m;
}

NormStrategy HistogramOfOrientedGradients::Norm(void) const
{
    return _Norm;
}

void HistogramOfOrientedGradients::Norm(NormStrategy &n)
{
    _Norm = n;
}
