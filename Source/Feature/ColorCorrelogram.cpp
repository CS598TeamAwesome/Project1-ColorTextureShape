#include "ColorCorrelogram.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
using std::vector;

using namespace ColorTextureShape;

void ColorCorrelogram::Compute(cv::Mat img){
    //cv::Mat img = cv::imread(argv[1]);
    cv::imshow("Input Image", img);
    cv::waitKey(0);

    //LAMBDA TABLES
    //given color
    //build horizontal lambda table for color (nxnxd)
    //build vertical lambda table for color (nxnxd)

    int rows = img.rows;
    int columns = img.cols;
    int distance = 10;

    std::cout << rows << "," << columns << std::endl;

    //accessing pixel info
    cv::Vec3b pixel = img.at<cv::Vec3b>(0,0);
    double b = pixel[0];
    double g = pixel[1];
    double r = pixel[2];
    std::cout << r << "," << g << "," << b << std::endl;

    pixel = img.at<cv::Vec3b>(64,100);
    b = pixel[0];
    g = pixel[1];
    r = pixel[2];
    std::cout << r << "," << g << "," << b << std::endl;

    //initialize horizontal lambda -- potentially convert to int*** array for speed?
    //lambdas should be built on the search j color, we use red for now
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
            cv::Vec3b pixel = img.at<cv::Vec3b>(i,j);
            b = pixel.val[0];
            g = pixel.val[1];
            r = pixel.val[2];
            //std::cout << b << "," << r << ","<< g << std::endl;
            //initialize for k = 0
            //parallelize lambda_v and lambda_h
            //TODO: parallelize here? (for all color checks)
            if(r > 0){
                r_ct++;
                lambda_h[i][j][0] = 1;
                lambda_v[i][j][0] = 1;
            } else {
                lambda_h[i][j][0] = 0;
                lambda_v[i][j][0] = 0;
            }
        }
    }

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

    //potential speedups in using MatIterator instead of individual pixel reads

    /*
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            std::cout << lambda_h[i][j][2] << " ";
        }
        std::cout << std::endl;
    }
    */

    //uGAMMA RESULTS FOR COLOR PAIR
    //given k, color1, color2
    //iterate over every pixel of color1, and sum the 4 lambda values

    int uGammaCt = 0;
    int k = 1;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            cv::Vec3b pixel = img.at<cv::Vec3b>(i,j);
            b = pixel.val[0];
            g = pixel.val[1];
            r = pixel.val[2];

            //choosing color1 as red, so autocorrelogram
            //choosing k = 1
            if(r > 0){
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
}
