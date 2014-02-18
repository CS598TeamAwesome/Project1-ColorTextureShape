#include "HistogramOfOrientedGradients.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace ColorTextureShape;

void HistogramOfOrientedGradients::Compute(void)
{
    // 1. Get the input image
    cv::Mat &inputImage = GetImage();

    // 2. Convert to grayscale
    cv::Mat img;
    cv::cvtColor(inputImage, img, CV_RGB2GRAY);

    // 3. Compute the horizontal and vertical gradients
    double gradData[] = {-1, 0, 1};
    cv::Mat verticalGradient(3, 1, CV_64F, gradData);
    cv::Mat horizontalGradient(1, 3, CV_64F, gradData);
    
    cv::Mat gradH, gradV;
    cv::filter2D(img, gradH, CV_8U, horizontalGradient); // Specifying 8U here explicitly to be safe
    cv::filter2D(img, gradV, CV_8U, verticalGradient);
    
    // 4. Divide image in cells (How big should each cell be? using 3x3)
    std::vector<cv::Mat> cellsH;
    std::vector<cv::Mat> cellsV;
    for(int i = 0; i < img.cols; i+=3)
    {
        if(i + 3 > img.cols)
            continue;
        
        for(int j = 0; j < img.rows; j+=3)
        {
            if(j + 3 > img.rows)
                continue;
            
            cellsH.push_back(gradH(cv::Rect(i, j, 3, 3)));
            cellsV.push_back(gradV(cv::Rect(i, j, 3, 3)));
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
                unsigned char h = gradH.at<unsigned char>(j, i);
                unsigned char v = gradV.at<unsigned char>(j, i);
                
                double angle = std::atan2(v, h);
                double mag = std::sqrt((h * h) + (v * v));
                
                int bin = (int)std::floor(9 * angle / M_PI);
                
                if(angle == M_PI) // Last bin must also include alpha = 180
                    bin = 8;
                
                histogram[bin] += mag;
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
    
    // finalDescriptor now contains the result of the HOG extraction.
}
