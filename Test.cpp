#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include "Feature/ColorCorrelogram.hpp"
#include "Feature/ColorHistogram.hpp"
#include "Feature/HistogramOfOrientedGradients.hpp"
#include <cmath>
using std::vector;
using namespace ColorTextureShape;

//assumes vectors are same length
double getL1Distance(vector<double> v1, vector<double> v2){
    if(v1.size() != v2.size()){
        return -1;
    }

    double sum = 0;

    for(int i = 0; i < v1.size(); i++){
        sum += std::abs(v1.at(i) - v2.at(i))/(1 + v1.at(i) + v2.at(i));
    }

    return sum;
}

void load_images(cv::Mat* images, int img_ct){
    std::cout << "Loading input images" << std::endl;
    clock_t start = clock();
    for(int i = 0; i < img_ct; i++){
        std::string result;
        std::ostringstream convert;
        convert << i;
        result = convert.str();

        //looping over fixed directory path, with expected file names 0.jpg, 1.jpg, etc
        std::string s = "wang_subset/" + result + ".jpg";
        //std::cout << s << std::endl;
        images[i] = cv::imread(s);
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds. - load images" << std::endl;

}

void build_histograms(vector<double>* histograms, cv::Mat* images, int size){
    ColorHistogram ch1;

    std::cout << "Building Histograms" << std::endl;
    clock_t start = clock();

    for(int i = 0; i < size; i++){
        std::cout << "histogram " << i << ": ";
        int inner_start = clock();

        histograms[i] = ch1.Compute(images[i]);

        std::cout << double( clock() - inner_start ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl;
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds. - total time for all images" << std::endl;
}

void build_correlograms(vector<double>* correlograms, cv::Mat* images, int size){
    //autocorrelogram of max distance 1
    ColorCorrelogram cc1(3, true);

    std::cout << "Building Correlograms" << std::endl;
    clock_t start = clock();

    for(int i = 0; i < size; i++){
        std::cout << "correlogram " << i << ": ";
        int inner_start = clock();

        correlograms[i] = cc1.Compute(images[i]);

        std::cout << double( clock() - inner_start ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl;
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds. - total time for all images" << std::endl;
}

void build_hogs(vector<double>* hogs, cv::Mat* images, int size){
    HistogramOfOrientedGradients h1;

    std::cout << "Building Histograms of Oriented Gradient" << std::endl;
    clock_t start = clock();

    for(int i = 0; i < size; i++){
        std::cout << "hog " << i << ": ";
        int inner_start = clock();

        hogs[i] = h1.Compute(images[i]);

        std::cout << double( clock() - inner_start ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl;
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds. - total time for all images" << std::endl;
}

void calculate_distances(double** table, vector<double>* points, int size){
    std::cout << "Calculating Distance" << std::endl;
    clock_t start = clock();
    //calculate pairwise distances
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            table[i][j] = getL1Distance(points[i], points[j]);
        }
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl;
}

int main(int argc, char **argv)
{

   int img_ct = 50;

   //load images
   cv::Mat* wang_images = new cv::Mat[img_ct];
   load_images(wang_images, img_ct);

   //build histograms
   vector<double> wang_histograms[img_ct];
   build_histograms(wang_histograms, wang_images, img_ct);

   //build correlograms
   vector<double> wang_auto_correlograms[img_ct];
   build_correlograms(wang_auto_correlograms, wang_images, img_ct);

   //build histograms of oriented gradient
   vector<double> wang_hogs[img_ct];
   build_hogs(wang_hogs, wang_images, img_ct);

   //calc pairwise histogram distances
   double** colorhist_distances = new double*[img_ct];
   for(int i = 0; i < img_ct; i++){
       colorhist_distances[i] = new double[img_ct];
   }
   calculate_distances(colorhist_distances, wang_histograms, img_ct);

   //calc pairwise correlogram distances
   double** correlogram_distances = new double*[img_ct];
   for(int i = 0; i < img_ct; i++){
       correlogram_distances[i] = new double[img_ct];
   }
   calculate_distances(correlogram_distances, wang_auto_correlograms, img_ct);

   //calc pairwise correlogram distances
   double** hog_distances = new double*[img_ct];
   for(int i = 0; i < img_ct; i++){
       hog_distances[i] = new double[img_ct];
   }
   calculate_distances(hog_distances, wang_hogs, img_ct);

   return 0;
}
