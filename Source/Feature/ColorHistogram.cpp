#include "ColorHistogram.hpp"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdlib.h>

using namespace cv;
using namespace std;

using namespace ColorTextureShape;

ColorHistogram::ColorHistogram(int bq, int gq, int rq)
{
    this->bq = bq;
    this->gq = gq;
    this->rq = rq;
}

vector<double> ColorHistogram::Compute(Mat& img){
	vector<double> hist;
	int nc = img.channels();
	int bin = rq*gq*bq;
	vector<int> valList;
	vector<float> lumList;
	vector<int> blueList;
	vector<int> greenList;
	vector<int> redList;
	
	for(int i = 0; i < bq*gq*rq; i++){
		hist.insert(hist.begin()+i,0);
	}


	for(int i = 0; i < img.rows; i++){
		for(int j = 0; j < img.cols; j++){
			int it = i*img.cols+j;
			Vec3b intensity = img.at<Vec3b>(i, j);
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];
			int blueInt = blue;
			int greenInt = green;
			int redInt = red;

			for(int b = 1; b <= bq; b++){
				for(int g = 1; g <= gq; g++){
					for(int r = 1; r <= rq; r++){
						int it2 = (b-1)*gq*rq+(g-1)*rq+(r-1);	
						float pbVal = (b-1)*256/bq-1;
						float pgVal = (g-1)*256/gq-1;
						float prVal = (r-1)*256/rq-1;
						float bVal = b*256/bq-1;
						float gVal = g*256/gq-1;
						float rVal = r*256/rq-1;					
						if(blueInt > pbVal && blueInt <= bVal && greenInt > pgVal && greenInt <= gVal && redInt > prVal && redInt <= rVal){	
							hist.at(it2) += 1;	
							break;
						}
						else{
							continue;
						}
					}
				}
			}
		}
	}

	return hist;
}
