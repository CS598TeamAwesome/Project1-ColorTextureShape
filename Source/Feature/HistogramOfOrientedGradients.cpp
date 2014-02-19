#include "HistogramOfOrientedGradients.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace ColorTextureShape;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(void)
{
    double gradData[] = {-1, 0, 1};
    _My = cv::Mat(3, 1, CV_64F, gradData);
    _Mx = cv::Mat(1, 3, CV_64F, gradData);
}

double *HistogramOfOrientedGradients::Compute(cv::Mat &img)
{
    // 1. Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, CV_RGB2GRAY);
    
    // 2. Convert grayscale to double 
    cv::Mat dblImg;
    gray.convertTo(dblImg, CV_64F);

    // 3. Compute the horizontal and vertical gradients    
    cv::Mat gradH, gradV;
    cv::filter2D(dblImg, gradH, CV_64F, _Mx); 
    cv::filter2D(dblImg, gradV, CV_64F, _My);
    
    // 4. Divide image in cells (How big should each cell be? using 3x3)
    std::vector<cv::Mat> cellsH;
    std::vector<cv::Mat> cellsV;
    for(int i = 0; i < img.cols; i+=_CellSize.width)
    {
        if(i + _CellSize.width > img.cols)
            continue;
        
        for(int j = 0; j < img.rows; j+=_CellSize.height)
        {
            if(j + _CellSize.height > img.rows)
                continue;
            
            cellsH.push_back(gradH(cv::Rect(cv::Point(i, j), _CellSize)));
            cellsV.push_back(gradV(cv::Rect(cv::Point(i, j), _CellSize)));
        }
    }
    
    // 5. Compute histogram for each cell (using 9 bins)
    std::vector<double *> cellHistograms(cellsH.size());
    std::transform(cellsH.begin(), cellsH.end(), cellsV.begin(), cellHistograms.begin(),
    [](cv::Mat &gradH, cv::Mat &gradV)
    {
        double *histogram = new double[9](); 
        
        for(int i = 0; i < gradH.cols; i++)
        {
            for(int j = 0; j < gradH.rows; j++)
            {
                double h = gradH.at<double>(j, i);
                double v = gradV.at<double>(j, i);
                
                if(h != 0 || v != 0)
                {
                    double angle = std::abs(std::atan2(v, h));
                    double mag = std::sqrt((h * h) + (v * v));
                    
                    int bin = (int)std::floor(9 * angle / M_PI);
                    
                    if(angle == M_PI) // Last bin must also include alpha = 180
                        bin = 8;
                    
                    histogram[bin] += mag;
                }
            }
        }
        
        return histogram;
    });
    
    // 6. Divide cells into overlapping blocks (How big should each be? again using 3x3)
    
    // 7. Normalize histograms for each cell in a block
    
    // 8. Concatenate histograms for each cell
    double *finalDescriptor = new double[cellHistograms.size() * 9];
    double *descriptorIter = finalDescriptor;
    for(double *histogram : cellHistograms)
    {
        std::copy(histogram, histogram + 9, descriptorIter);
        descriptorIter += 9;
    }
    
    return finalDescriptor;
}