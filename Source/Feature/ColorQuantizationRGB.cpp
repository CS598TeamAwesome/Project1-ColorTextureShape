#include "ColorQuantizationRGB.hpp"

using namespace ColorTextureShape;

//TODO: NEED DESTRUCTOR

//constructor
ColorQuantizationRGB::ColorQuantizationRGB(int r_ct, int g_ct, int b_ct){
    rq = r_ct;
    gq = g_ct;
    bq = b_ct;
    //default bin ranges -- divided equally
    r_ranges = new int[r_ct+1];
    g_ranges = new int[g_ct+1];
    b_ranges = new int[b_ct+1];

    //these might have a rounding problem for large color counts, need to test
    for(int i = 0; i < r_ct; i++){
        r_ranges[i] = (256/r_ct*1.0)*i;
    }
    r_ranges[r_ct] = 257; //upper bound

    for(int i = 0; i < g_ct; i++){
        g_ranges[i] = (256/g_ct*1.0)*i;
    }
    g_ranges[g_ct] = 257; //upper_bound

    for(int i = 0; i < b_ct; i++){
        b_ranges[i] = (256/b_ct*1.0)*i;
    }
    b_ranges[b_ct] = 257; //upper_bound
}

int ColorQuantizationRGB::getBinCt(){
    return bq*gq*rq;
}

//assuming the src and dest dimensions are equal
void ColorQuantizationRGB::quantize(cv::Mat src, int** dest){
    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            cv::Vec3b intensity = src.at<cv::Vec3b>(i, j);
            dest[i][j] = binForColor(intensity);
        }
    }
}

//the convention we use for ordering is blue*(g_ct*r_ct) + green*(r_ct) + r_ct
int ColorQuantizationRGB::binForColor(cv::Vec3b pixel){
    uchar blue = pixel.val[0];
    uchar green = pixel.val[1];
    uchar red = pixel.val[2];
    int blueInt = blue;
    int greenInt = green;
    int redInt = red;

    int blueBucket = -1;
    for(int b = 0; b < bq; b++){
        float pbVal = b_ranges[b]; //get lower bound
        float bVal = b_ranges[b+1]; //get upper bound
        if(blueInt >= pbVal && blueInt < bVal){
            blueBucket = b;
            break;
        }
    }

    int greenBucket = -1;
    for(int g = 0; g < gq; g++){
        float pgVal = g_ranges[g];
        float gVal = g_ranges[g+1];
        if(greenInt >= pgVal && greenInt < gVal){
            greenBucket = g;
            break;
        }
    }

    int redBucket = -1;
    for(int r = 0; r < rq; r++){
        float prVal = r_ranges[r];
        float rVal = r_ranges[r+1];
        if(redInt >= prVal && redInt < rVal){
            redBucket = r;
            break;
        }
    }

    if(blueBucket < 0 || redBucket < 0 || greenBucket < 0)
        return -1; //something wrong with the pixel values
    else
        return blueBucket*gq*rq+greenBucket*rq+redBucket; //(b-1)*gq*rq+(g-1)*rq+(r-1);
}
