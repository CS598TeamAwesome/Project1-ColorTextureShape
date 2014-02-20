#include "ColorCorrelogram.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using std::vector;

using namespace ColorTextureShape;

//TODO: should we flatten into a vector (1D double array), or leave that up to the distance functions
//TODO: either add an autocorrelogram flag, or a separate function for it

void ColorCorrelogram::Compute(cv::Mat img, int distance){
    //use a default quantization
    ColorQuantizationRGB quantization(4,4,4);
    Compute(img, distance, quantization);
}

//distance should be less than the dimensions of the img
void ColorCorrelogram::Compute(cv::Mat img, int distance, ColorQuantizationRGB quantization){
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

    //2. build lambda tables -- may want to make these fields and lazy load them

    //a horizontal and a vertical table for each color
    int**** lambda_tables = new int***[color_ct*2];
    for(int i = 0; i < color_ct; i++){
        if(hist[i] == 0){ //no pixels quantized to that color, so setting to NULL
            lambda_tables[2*i] = NULL;
            lambda_tables[2*i+1] = NULL;
        } else {
            lambda_tables[2*i] = buildLambdaTable(i, 'h', quantized_img, rows, columns, distance);
            lambda_tables[2*i+1] = buildLambdaTable(i, 'v', quantized_img, rows, columns, distance);
        }
    }
    //PERFORMANCE: some colors may be very sparse, so we may be able to save space
    //TODO: gonna have to destroy these guys after we're done

    //3a. use gamma results to construct full correlogram
    double*** correlogram = new double**[color_ct];
    for(int i = 0; i < color_ct; i++){
        correlogram[i] = new double*[color_ct];
        for(int j = 0; j < color_ct; j++){
            correlogram[i][j] = new double[distance+1];
            for(int k = 1; k <= distance; k++){ //TODO: check if zero distances are needed
                int uGammaCt;
                //std::cout << i << ", " << j << std::endl;
                if(hist[i] == 0 || hist[j] == 0){
                    uGammaCt = 0;
                    correlogram[i][j][k] = 0;
                } else {
                    uGammaCt = uGammaValue(i, j, quantized_img, rows, columns, distance, k, lambda_tables);
                    //divid by histogram count of color * 8k for correlogram value
                    correlogram[i][j][k] = uGammaCt/(hist[i]*8.0*k); //TODO: if we need zeroes, need to fix divide by 0
                    //std::cout << i << "," << j << "," << k << "->" << correlogram[i][j][k] << std::endl;
                }
            }
        }
    }

    //3b. same, but for auto-correlogram
    double** autocorrelogram = new double*[color_ct];
    for(int i = 0; i < color_ct; i++){
        autocorrelogram[i] = new double[distance+1];
        for(int k = 1; k <= distance; k++){
            int uGammaCt;
            //std::cout << i << ", " << j << std::endl;
            if(hist[i] == 0){
                uGammaCt = 0;
                autocorrelogram[i][k] = 0;
            } else {
                uGammaCt = uGammaValue(i, i, quantized_img, rows, columns, distance, k, lambda_tables);
                //divid by histogram count of color * 8k for correlogram value
                autocorrelogram[i][k] = uGammaCt/(hist[i]*8.0*k); //TODO: if we need zeroes, need to fix divide by 0
                //std::cout << i << "," << k << "->" << autocorrelogram[i][k] << std::endl;
            }
        }
    }

    //4. flatten into vector? or leave that up to the distance functions
    //size = N * N * D = 64 * 64 * 10 = 40960
}

/**
 * @brief buildLambdaTable - builds the lambda tables for a given color
 * @param color - bounded by color_ct
 * @param direction - 'h' for horizontal, 'v' for vertical
 * @param q_img - a 2D array of the quantized color values from an img
 * @param rows - rows in img
 * @param columns - columns in img
 * @param d - the correlogram distance cap
 * @param dest - the resulting lambda table
 */
int*** ColorCorrelogram::buildLambdaTable(int color, char direction, int** q_img, int rows, int columns, int d){
    //initialize table
    int*** lambda = new int**[rows];
    for(int i = 0; i < rows; i++){
        lambda[i] = new int*[columns];
        for(int j = 0; j < columns; j++){
            lambda[i][j] = new int[d+1];

            //initialize for the zero distance
            if(q_img[i][j] == color)
                lambda[i][j][0] = 1;
            else
                lambda[i][j][0] = 0;
        }
    }

    //build up the rest of lambda dynamically
    for(int k = 1; k <= d; k++){
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
 * @brief uGammaValue - gets the result of the big gamma function, given color1, color2, and k
 * @param color1 - the color of the pixels we are using as centers
 * @param color2 - the color of the pixels that we are counting around the center pixel
 * @param k - the distance from the center pixel we are looking
 * @param q_img - quantized image
 * @param rows - rows in image
 * @param columns - columns in image
 * @param distance - the distance limit
 * @param lambda_tables - lambda values for image
 * @return
 */
int ColorCorrelogram::uGammaValue(int color1, int color2, int** q_img, int rows, int columns, int distance, int k, int**** lambda_tables){
    int uGammaCt = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(q_img[i][j] == color1){

                if(2*k <= distance){ //boundary checks
                    if(i-k >= 0 && j+k < columns)
                        uGammaCt += lambda_tables[2*color2][i-k][j+k][2*k];
                    if(i-k >= 0 && j-k >= 0)
                        uGammaCt += lambda_tables[2*color2][i-k][j-k][2*k];
                }

                if(2*k - 2 <= distance){ //boundary checks
                    if(i-k >= 0 && j-k+1 >= 0)
                        uGammaCt += lambda_tables[2*color2+1][i-k][j-k+1][2*k-2];
                    if(i+k < rows && j-k+1 >= 0)
                        uGammaCt += lambda_tables[2*color2+1][i+k][j-k+1][2*k-2];
                }
            }
        }
    }
    return uGammaCt;
}
