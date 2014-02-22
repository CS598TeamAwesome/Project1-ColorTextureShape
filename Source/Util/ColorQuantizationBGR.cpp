#include "ColorQuantizationBGR.hpp"

using namespace ColorTextureShape;

//constructor
ColorQuantizationBGR::ColorQuantizationBGR(int r_ct, int g_ct, int b_ct){
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
    r_ranges[r_ct] = 256; //upper bound

    for(int i = 0; i < g_ct; i++){
        g_ranges[i] = (256/g_ct*1.0)*i;
    }
    g_ranges[g_ct] = 256; //upper_bound

    for(int i = 0; i < b_ct; i++){
        b_ranges[i] = (256/b_ct*1.0)*i;
    }
    b_ranges[b_ct] = 256; //upper_bound

    mapping = new int**[256];
    //initialize 256x256x256 array for mapping
    for(int b = 0; b < bq; b++){
        for(int i = b_ranges[b]; i < b_ranges[b+1]; i++){
            mapping[i] = new int*[256];

            for(int g = 0; g < gq; g++){
                for(int j = g_ranges[g]; j < g_ranges[g+1]; j++){
                    mapping[i][j] = new int[256];

                    for(int r = 0; r < rq; r++){
                        //the convention we use for ordering is blue*(g_ct*r_ct) + green*(r_ct) + r_ct
                        int bin = b*gq*rq+g*rq+r;

                        for(int k = r_ranges[r]; k < r_ranges[r+1]; k++){
                            mapping[i][j][k] = bin;
                        }
                    }

                }
            }

        }
    }
}

ColorQuantizationBGR::~ColorQuantizationBGR(){
    delete[] b_ranges;
    delete[] g_ranges;
    delete[] r_ranges;
}

int ColorQuantizationBGR::getBinCt(){
    return bq*gq*rq;
}

//assuming the src and dest dimensions are equal
void ColorQuantizationBGR::quantize(cv::Mat src, int** dest){
    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            cv::Vec3b intensity = src.at<cv::Vec3b>(i, j);
            dest[i][j] = binForColor(intensity);
        }
    }
}

int ColorQuantizationBGR::binForColor(cv::Vec3b pixel){
    int blue = pixel.val[0];
    int green = pixel.val[1];
    int red = pixel.val[2];

    return mapping[blue][green][red];
}
