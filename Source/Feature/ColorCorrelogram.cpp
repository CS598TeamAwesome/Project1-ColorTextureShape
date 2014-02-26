#include "ColorCorrelogram.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using std::vector;

using namespace ColorTextureShape;

ColorCorrelogram::ColorCorrelogram(int d, bool set_auto)
    : quantization(4,4,4)
{
    distance = d;
    auto_flag = set_auto;
    //use default 64 bin quantization
}

ColorCorrelogram::ColorCorrelogram(int d, bool set_auto, ColorQuantizationBGR space){
    distance = d;
    auto_flag = false;
    quantization = space;
}

std::vector<double> ColorCorrelogram::Compute(cv::Mat &img){
    int color_ct = quantization.getBinCt();
    vector<double> result;

    if(!auto_flag){
        double*** correlogram = computeFullCorrelogram(img);
        for(int i = 0; i < color_ct; i++){
            for(int j = 0; j < color_ct; j++){
                for(int k = 1; k <= distance; k++){
                    result.push_back(correlogram[i][j][k]);
                }
            }
        }
        deleteCorrelogram(correlogram);
    } else {
        double** autocorrelogram = computeAutoCorrelogram(img);
        for(int i = 0; i < color_ct; i++){
            for(int k = 1; k <= distance; k++){
                result.push_back(autocorrelogram[i][k]);
            }
        }
        deleteAutoCorrelogram(autocorrelogram);
    }

    return result;
}

/**
 * @brief ColorCorrelogram::computeFullCorrelogram:
 * Implements a dynamic programming efficiency suggestion found in the
 * ([Huang97] Image indexing using color correlograms) paper. It reduces
 * the counting problem to two passes over the image for each depth and
 * for each color to build a table of lambda values. And then another pass
 * over the image for each depth and for each color pair to calculate the
 * correlogram values.
 *
 * The size of the return vector is O(m*m*d)
 */
double*** ColorCorrelogram::computeFullCorrelogram(cv::Mat &img){
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

    //0. color hist, should call Cheng's function
    int* hist = new int[color_ct];
    for(int i = 0; i < color_ct; i++){
        hist[i] = 0;
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            hist[quantized_img[i][j]]++;
        }
    }

    double*** correlogram = new double**[color_ct];
    for(int i = 0; i < color_ct; i++){

        //2. for each color build lambda tables
        //a horizontal and a vertical table for each color
        int*** lambda_h;
        int*** lambda_v;

        if(hist[i] == 0){ //no pixels quantized to that color, so setting to NULL
            lambda_h = NULL;
            lambda_v = NULL;
        } else {
            lambda_h = buildLambdaTable(i, 'h', quantized_img, rows, columns);
            lambda_v = buildLambdaTable(i, 'v', quantized_img, rows, columns);
        }

        //3a. use gamma results to construct correlogram sections for color2's lambda tables computed about
        correlogram[i] = new double*[color_ct];
        for(int j = 0; j < color_ct; j++){
            correlogram[i][j] = new double[distance+1];
            for(int k = 1; k <= distance; k++){
                int uGammaCt;
                //std::cout << i << ", " << j << std::endl;
                if(hist[i] == 0 || hist[j] == 0){
                    uGammaCt = 0;
                    correlogram[i][j][k] = 0;
                } else {
                    uGammaCt = uGammaValue(j, quantized_img, rows, columns, k, lambda_h, lambda_v);
                    //divid by histogram count of color * 8k for correlogram value
                    correlogram[i][j][k] = uGammaCt/(hist[i]*8.0*k);
                    //i and j are transposed from the formula in the reference
                }
            }
        }

        //lambda tables for this color won't be use again, so destory them to free up memory
        if(lambda_h != NULL)
            deleteLambdaTable(rows, columns, lambda_h);
        if(lambda_v != NULL)
            deleteLambdaTable(rows, columns, lambda_v);
    }
    //PERFORMANCE: some colors may be very sparse, so we may be able to save space


    //4. cleanup
    //quantized image
    for(int i = 0; i < rows; i++){
        delete[] quantized_img[i];
    }
    delete[] quantized_img;

    //histogram
    delete[] hist;

    return correlogram;
}

/**
 * @brief ColorCorrelogram::computeAutoCorrelogram
 * Uses the same algorithm as full correlogram, except in the final
 * calculation step, only need to consider the number of colors (m)
 * rather than the number of color pairs (m*m)
 */
double** ColorCorrelogram::computeAutoCorrelogram(cv::Mat &img){
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

    //0. color hist, should call Cheng's function
    int* hist = new int[color_ct];
    for(int i = 0; i < color_ct; i++){
        hist[i] = 0;
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            hist[quantized_img[i][j]]++;
        }
    }

    double** autocorrelogram = new double*[color_ct];

    for(int i = 0; i < color_ct; i++){

        //2. for each color, build lambda tables for that color
        //a horizontal and a vertical table for each color
        int*** lambda_h;
        int*** lambda_v;
        if(hist[i] == 0){ //no pixels quantized to that color, so done
            lambda_h = NULL;
            lambda_v = NULL;
        } else {
            lambda_h = buildLambdaTable(i, 'h', quantized_img, rows, columns);
            lambda_v = buildLambdaTable(i, 'v', quantized_img, rows, columns);
        }

        //3b. fill auto-correlogram section for color
        autocorrelogram[i] = new double[distance+1];
        for(int k = 1; k <= distance; k++){
            int uGammaCt;
            if(hist[i] == 0){
                uGammaCt = 0;
                autocorrelogram[i][k] = 0;
            } else {
                uGammaCt = uGammaValue(i, quantized_img, rows, columns, k, lambda_h, lambda_v);
                //divid by histogram count of color * 8k for correlogram value
                autocorrelogram[i][k] = uGammaCt/(hist[i]*8.0*k);
            }
        }

        //lambda table for color won't be used again, so destroy to free memory
        if(lambda_h != NULL)
            deleteLambdaTable(rows, columns, lambda_h);
        if(lambda_v != NULL)
            deleteLambdaTable(rows, columns, lambda_v);
    }
    //PERFORMANCE: some colors may be very sparse, so we may be able to save space

    //4. cleanup
    //quantized image
    for(int i = 0; i < rows; i++){
        delete[] quantized_img[i];
    }
    delete[] quantized_img;

    //histogram
    delete[] hist;

    return autocorrelogram;
}

/**
 * @brief buildLambdaTable - builds the lambda tables for a given color
 * @param color - bounded by color_ct
 * @param direction - 'h' for horizontal, 'v' for vertical
 * @param q_img - a 2D array of the quantized color values from an img
 * @param rows - rows in img
 * @param columns - columns in img
 * @param dest - the resulting lambda table
 */
int*** ColorCorrelogram::buildLambdaTable(int color, char direction, int** q_img, int rows, int columns){
    //initialize table
    int*** lambda = new int**[rows];
    for(int i = 0; i < rows; i++){
        lambda[i] = new int*[columns];
        for(int j = 0; j < columns; j++){
            lambda[i][j] = new int[distance+1];

            //initialize for the zero distance
            if(q_img[i][j] == color)
                lambda[i][j][0] = 1;
            else
                lambda[i][j][0] = 0;
        }
    }

    //build up the rest of lambda dynamically
    for(int k = 1; k <= distance; k++){
        //using k = n, build k = n+1
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < columns; j++){
                if(direction == 'h'){
                    if(i+k < rows)
                        lambda[i][j][k] = lambda[i][j][k-1] + lambda[i+k][j][0];
                    else
                        lambda[i][j][k] = lambda[i][j][k-1];
                } else if(direction == 'v'){
                    if(j+k < columns)
                        lambda[i][j][k] = lambda[i][j][k-1] + lambda[i][j+k][0];
                    else
                        lambda[i][j][k] = lambda[i][j][k-1];
                }
            }
        }
    }

    return lambda;
}

/**
 * @brief uGammaValue - gets the result of the big gamma function, given color1, the lambda tables for color2, and k
 * @param color1 - the color of the pixels we are using as centers
 * @param k - the distance from the center pixel we are looking
 * @param q_img - quantized image
 * @param rows - rows in image
 * @param columns - columns in image
 * @param lambda_h - horizontal lambda values for color2
 * @param lambda_v - vertical lambda values for color2
 * @return
 */
int ColorCorrelogram::uGammaValue(int color1, int** q_img, int rows, int columns, int k, int*** lambda_h, int*** lambda_v){
    int uGammaCt = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(q_img[i][j] == color1){

                if(2*k <= distance){ //boundary checks
                    if(i-k >= 0 && j+k < columns)
                        uGammaCt += lambda_h[i-k][j+k][2*k];
                    if(i-k >= 0 && j-k >= 0)
                        uGammaCt += lambda_h[i-k][j-k][2*k];
                }

                if(2*k - 2 <= distance){ //boundary checks
                    if(i-k >= 0 && j-k+1 >= 0)
                        uGammaCt += lambda_v[i-k][j-k+1][2*k-2];
                    if(i+k < rows && j-k+1 >= 0)
                        uGammaCt += lambda_v[i+k][j-k+1][2*k-2];
                }
            }
        }
    }
    return uGammaCt;
}

//get dimensions
int ColorCorrelogram::getDistance(){
    return distance;
}

int ColorCorrelogram::getColorCt(){
    return quantization.getBinCt();
}

int ColorCorrelogram::getAutoCorrelogramVectorSize(){
    int color_ct = quantization.getBinCt();
    return distance * color_ct;
}

int ColorCorrelogram::getCorrelogramVectorSize(){
    int color_ct = quantization.getBinCt();
    return distance * color_ct * color_ct;
}

//cleanup functions
void ColorCorrelogram::deleteLambdaTable(int rows, int columns, int*** lambda){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            delete[] lambda[i][j];
        }
        delete[] lambda[i];
    }
    delete[] lambda;
}

void ColorCorrelogram::deleteCorrelogram(double*** correlogram){
    int color_ct = quantization.getBinCt();
    for(int i = 0; i < color_ct; i++){
        for(int j = 0; j < color_ct; j++){
            delete[] correlogram[i][j];
        }
        delete[] correlogram[i];
    }
    delete[] correlogram;
}

void ColorCorrelogram::deleteAutoCorrelogram(double** autocorrelogram){
    int color_ct = quantization.getBinCt();
    for(int i = 0; i < color_ct; i++){
        delete[] autocorrelogram[i];
    }
    delete[] autocorrelogram;
}
