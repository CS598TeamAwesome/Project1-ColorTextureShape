#include "Convolve.hpp"

void ColorTextureShape::CenterConvolve(cv::Mat &img, cv::Mat &out, cv::Mat &kernel)
{
    if(kernel.rows % 2 != 1 && kernel.cols % 2 != 1)
        return;
    
    out.create(img.rows, img.cols, CV_64F);
    
    double kernelCenterI = kernel.cols / 2;
    double kernelCenterJ = kernel.rows / 2;    
    
    for(int j = 0; j < img.rows; j++)
    {
        for(int i = 0; i < img.cols; i++)
        {
            double pixel = 0;
            
            for(int kj = 0; kj < kernel.rows; kj++)
            {
                for(int ki = 0; ki < kernel.cols; ki++)
                {
                    double pixelPosI = coerce_value<double>(0, i + (ki - kernelCenterI), img.cols - 1);
                    double pixelPosJ = coerce_value<double>(0, j + (kj - kernelCenterJ), img.rows - 1);
                    
                    double kernelVal = kernel.at<double>(kj, ki);
                    double pixelVal = img.at<double>(pixelPosJ, pixelPosI);
                    
                    pixel += pixelVal * kernelVal;
                }
            }
            
            out.at<double>(j, i) = pixel;
        }
    }
}
