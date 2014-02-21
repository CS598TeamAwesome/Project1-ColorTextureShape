#include "Feature/HistogramOfOrientedGradients.hpp"
#include <opencv2/opencv.hpp>
#include <fstream>

int main(int argc, char **argv)
{
    cv::Mat img = cv::imread(argv[1]);
    
    ColorTextureShape::HistogramOfOrientedGradients hog;
    std::vector<double> descriptor = hog.Compute(img);
    
    // This file can be graphed using Excel or something
    std::ofstream csv("out.csv");
    for(double bin : descriptor)
    {
        csv << bin << ",";
    }
    
    std::cin.get();
    
    return 0;
}
