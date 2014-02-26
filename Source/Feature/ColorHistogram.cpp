#include "ColorHistogram.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using std::vector;

using namespace ColorTextureShape;

ColorHistogram::ColorHistogram()
    : quantization(4, 4, 4)
{

}

ColorHistogram::ColorHistogram(ColorQuantizationBGR space)
    : quantization(space)
{

}

std::vector<double> ColorHistogram::Compute(cv::Mat &img){
    //how many colors (N) and their ranges are passed in as quantization
    int color_ct = quantization.getBinCt();
    int rows = img.rows;
    int columns = img.cols;

    //1. quantize image
    int** quantized_img = new int*[rows];
    for(int i = 0; i < img.rows; i++){
        quantized_img[i] = new int[columns];
    }

    quantization.quantize(img, quantized_img);

    //2. one more pass over image to put quantized pixels into bins
    int* hist = new int[color_ct];
    for(int i = 0; i < color_ct; i++){
        hist[i] = 0;
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            hist[quantized_img[i][j]]++;
        }
    }

    vector<double> result(hist, hist+color_ct);

    //cleanup
    //delete quantized image
    for(int i = 0; i < rows; i++){
        delete[] quantized_img[i];
    }
    delete[] quantized_img;

    //delete hist
    delete[] hist;

    return result;
}
