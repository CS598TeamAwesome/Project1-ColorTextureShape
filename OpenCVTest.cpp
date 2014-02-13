#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

int main(int argc, char **argv)
{
    // Make sure arguments are correct
    if(argc <= 1)
    {
        std::cout << "Pass an image as the first argument" << std::endl;
        return 1;
    }

    // Load and display input image
    std::cout << "Loading input image" << std::endl;
    cv::Mat img = cv::imread(argv[1]);
    cv::imshow("Input Image", img);
    cv::waitKey(0);

    // Convert image to grayscale and show
    std::cout << "Converting input image to grayscale" << std::endl;
    cv::Mat grayscale;
    cv::cvtColor(img, grayscale, CV_RGB2GRAY);
    cv::imshow("Grayscale", grayscale);
    cv::waitKey(0);

    // Find edges in image and show
    std::cout << "Finding edges in image using canny edge detection algorithm" << std::endl;
    cv::Mat edges;
    cv::Canny(grayscale, edges, 200, 200);
    cv::imshow("Edges", edges);
    cv::waitKey(0);

    // Find circles using the hough transform and show
    std::cout << "Finding circles in the image using the hough transform" << std::endl;

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(grayscale, circles, CV_HOUGH_GRADIENT, 1, 1, 50, 80);

    // Loop through all the detected circles and draw them
    cv::Mat circlesImage = cv::Mat::zeros(img.size(), CV_8UC3);  // 8UC3 => 8 bits per pixel, 3 channel image initialized to 0 (black)
    std::for_each(circles.begin(), circles.end(), [&circlesImage](cv::Vec3f &circleParams)
    {
        // Circle params
        cv::Point center(circleParams[0], circleParams[1]);
        int radius = circleParams[2];

        // Circle center
        cv::circle(circlesImage, center, 3, cv::Scalar(0, 255, 0)); // green center point

        // Circle outline
        cv::circle(circlesImage, center, radius, cv::Scalar(0, 0, 255)); // RED outline (b,g,r by default this is the opposite of MATLAB)
    });

    // Write out the number of detected circles
    std::ostringstream stringFormatter;
    stringFormatter << circles.size() << " circles detected";
    cv::putText(circlesImage, stringFormatter.str(), cv::Point(15, 15), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));

    // Diplay the circles
    cv::imshow("Hough Circles", circlesImage);
    cv::waitKey(0);

    // Done
    std::cout << "Finished, press any key to continue..." << std::endl;
    std::cin.get();

    return 0;
}
