#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Feature/ColorCorrelogram.hpp"
#include "Feature/ColorHistogram.hpp"
#include "Feature/HistogramOfOrientedGradients.hpp"
#include <cmath>
using std::vector;
using namespace ColorTextureShape;

struct distance_measure {
    int src_img;
    int dest_img;
    double distance;
};

//assumes vectors are same length
double getL1Distance(vector<double> v1, vector<double> v2){
    if(v1.size() != v2.size()){
        return -1;
    }

    double sum = 0;

    for(int i = 0; i < v1.size(); i++){
        if(v1.at(i) - v2.at(i) != 0){
            sum += std::abs(v1.at(i) - v2.at(i))/(1 + v1.at(i) + v2.at(i));
        }
    }
    return sum;
}

void load_images(cv::Mat* images, int img_ct){
    std::cout << "Loading input images" << std::endl;
    clock_t start = clock();
    for(int i = 0; i < img_ct; i++){
        std::string result;
        
        //looping over fixed directory path, with expected file names 0.jpg, 1.jpg, etc
        std::ostringstream convert;
        convert << "wang_subset/" << i << ".jpg";
        std::string s = convert.str();

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
    ColorCorrelogram cc1(5, true);

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
/*
        for(int j = 0; j < hogs[i].size(); j++){
            if(hogs[i].at(j) != 0)
                std::cout << i << "," << j << ": " << hogs[i].at(j) << std::endl;
        }
*/
        std::cout << double( clock() - inner_start ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl;
    }
    std::cout << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds. - total time for all images" << std::endl;
}

void calculate_distances(vector<vector<distance_measure>> &table, vector<double>* points, int size){
    std::cout << "Calculating Distance" << std::endl;
    clock_t start = clock();
    //calculate pairwise distances
    for(int i = 0; i < size; i++){
        vector<distance_measure> row;
        for(int j = 0; j < size; j++){
            distance_measure dm;
            dm.src_img = i;
            dm.dest_img = j;
            dm.distance = getL1Distance(points[i], points[j]);
            row.push_back(dm);
        }

        //sort row
        std::sort(row.begin(), row.end(),[](distance_measure const & a, distance_measure const &b){return a.distance < b.distance;});

        table.push_back(row);
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
   vector<vector<distance_measure>> colorhist_distances;
   calculate_distances(colorhist_distances, wang_histograms, img_ct);


   std::ofstream fileout ("colorhist_ranks.out");
   for(int i = 0; i < img_ct; i++){
       fileout << "Source Img: " << i << std::endl;
        for(int j = 0; j < img_ct; j++){
            distance_measure item = colorhist_distances[i][j];
            fileout << item.dest_img << ", distance - " << item.distance << std::endl;
        }
   }
   fileout.close();

   //calc pairwise correlogram distances
   vector<vector<distance_measure>> correlogram_distances;
   calculate_distances(correlogram_distances, wang_auto_correlograms, img_ct);

   std::ofstream fileout2 ("correlogram_ranks.out");
   for(int i = 0; i < img_ct; i++){
       fileout2 << "Source Img: " << i << std::endl;
        for(int j = 0; j < img_ct; j++){
            distance_measure item = correlogram_distances[i][j];
            fileout2 << item.dest_img << ", distance - " << item.distance << std::endl;
        }
   }
   fileout2.close();


   //calc pairwise hog distances
   vector<vector<distance_measure>> hog_distances;
   calculate_distances(hog_distances, wang_hogs, img_ct);

   std::ofstream fileout3 ("hog_ranks.out");
   for(int i = 0; i < img_ct; i++){
       fileout3 << "Source Img: " << i << std::endl;
        for(int j = 0; j < img_ct; j++){
            distance_measure item = hog_distances[i][j];
            fileout3 << item.dest_img << ", distance - " << item.distance << std::endl;
        }
   }
   fileout3.close();

   return 0;
}
