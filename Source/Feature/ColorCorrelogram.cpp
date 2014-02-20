#include "ColorCorrelogram.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using std::vector;

using namespace ColorTextureShape;

void ColorCorrelogram::Compute(cv::Mat img){
    //use a default quantization
    ColorQuantizationRGB quantization(4,4,4);
    Compute(img, quantization);
}

void ColorCorrelogram::Compute(cv::Mat img, ColorQuantizationRGB quantization){
    //how many colors (N) and their ranges are passed in as quantization
    int color_ct = quantization.getBinCt();
    int rows = img.rows;
    int columns = img.cols;
    int distance = 10;

    //destination for quantized image
    int** quantized_img = new int*[rows];
    for(int i = 0; i < img.rows; i++){
        quantized_img[i] = new int[columns];
    }

    //quantize image
    quantization.quantize(img, quantized_img);

    //build 2N lambda tables, a horizontal and vertical one for each of the N colors
    // . do we want a function to build a lambda table given a color?
    // . definitely need to pass in max distance

    //LAMBDA TABLES
    //given color
    //build horizontal lambda table for color (nxnxd)
    //build vertical lambda table for color (nxnxd)

    //NEED TO CONVERT THIS TO 3D INT ARRAY
    //NEED TO BREAK THIS OUT AS A FUNCTION, THEN LOOP OVER ALL COLORS

    //START LAMBDA FUNCTION, PASSING IN RED (bin 3)
    //initialize horizontal lambda -- potentially convert to int*** array for speed?
    //lambdas should be built on the search j color, we use red for now

    int color = 3; //the red from the sample png reduces to bin 3
    vector<vector<vector<int>>> lambda_h;
    vector<vector<vector<int>>> lambda_v;
    lambda_h.resize(rows);
    lambda_v.resize(rows);
    int r_ct = 0;
    for(int i = 0; i < rows; i++){
        lambda_h[i].resize(columns);
        lambda_v[i].resize(columns);
        for(int j = 0; j < columns; j++){
            lambda_h[i][j].resize(distance);
            lambda_v[i][j].resize(distance);

            //initialize for k = 0
            //parallelize lambda_v and lambda_h
            //TODO: parallelize here? (for all color checks)
            if(quantized_img[i][j] == color){
                r_ct++;
                lambda_h[i][j][0] = 1;
                lambda_v[i][j][0] = 1;
            } else {
                lambda_h[i][j][0] = 0;
                lambda_v[i][j][0] = 0;
            }
        }
    }
    //TODO: gonna have to destroy these guys after we're done

    std::cout << "red count:" << r_ct << std::endl;

    //build up the rest of lambda_h dynamically
    for(int k = 1; k < distance; k++){
        //using k = n, build k = n+1
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < columns; j++){
                if(i+k < rows)
                    lambda_h[i][j][k] = lambda_h[i][j][k-1] + lambda_h[i+k][j][0];
                else
                    lambda_h[i][j][k] = lambda_h[i][j][k-1];

                if(j+k < columns)
                    lambda_v[i][j][k] = lambda_v[i][j][k-1] + lambda_v[i][j+k][0];
                else
                    lambda_v[i][j][k] = lambda_v[i][j][k-1];
            }
        }
    }
    //END LAMBDA FUNCTION
    //potential speedups in using MatIterator instead of individual pixel reads

    //uGAMMA RESULTS FOR COLOR PAIR
    //given k, color1, color2
    //iterate over every pixel of color1, and sum the 4 lambda values

    int uGammaCt = 0;
    int k = 1;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            //choosing color1 as red, so autocorrelogram
            //choosing k = 1
            if(quantized_img[i][j] == color){
                if(i-k >= 0 && j+k < columns)
                    uGammaCt += lambda_h[i-k][j+k][2*k];
                if(i-1 >= 0 && j-1 >= 0)
                    uGammaCt += lambda_h[i-k][j-k][2*k];
                if(i-1 >= 0 && j-1+1 >= 0)
                    uGammaCt += lambda_v[i-k][j-k+1][2*k-2];
                if(i+1 < rows && j-1+1 >= 0)
                    uGammaCt += lambda_v[i+k][j-k+1][2*k-2];
            }

        }
    }

    std::cout << uGammaCt << std::endl;


    //lGAMMA RESULTS FOR COLOR PAIR
    //divided by histogram count of color1 * 8k
    double lGammaCt = uGammaCt/(r_ct * 8.0 * k);
    std::cout << lGammaCt << std::endl;


    //COLOR CORRELOGRAM
    //process for all color pairs and all k's
    //size = N * N * D = 64 * 64 * 10 = 40960
}
