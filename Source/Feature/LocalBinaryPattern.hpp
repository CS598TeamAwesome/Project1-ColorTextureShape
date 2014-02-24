#pragma once

#include "HistogramFeature.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

namespace ColorTextureShape
{
	// bins struct, contains the id of each bins and its value
	struct LBP
	{
		int bin_id;
		int hist_value;
	};

	class LocalBinaryPattern : public HistogramFeature
	{
	public:
		// Binary to Decimal
		int B2D( char str[8] );

		// Generate one byte for LBP
		char Value_Compare( cv::Vec3b pix_val, int val );

		// Exclude the same type of texture
		char* MoveBinary( char temp[8] );

		//Generate LBP Histogram
		std::vector<LBP> LBP_Hist( cv::Mat& img );
        
        std::vector<double> Compute(cv::Mat &img);
	};
}
