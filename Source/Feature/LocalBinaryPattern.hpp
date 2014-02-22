#include "HistogramFeature.hpp"

namespace ColorTextureShape
{
	// bins struct, contains the id of each bins and its value
	struct LBP
	{
		int bin_id;
		int hist_value;
	};

	struct Hist
	{
		vector<LBP> hist_red;
		vector<LBP> hist_green;
		vector<LBP> hist_blue;
	};

	class LocalBinaryPattern : public HistogramFeature
	{
		// Binary to Decimal
		int B2D( char str[8] );

		// Generate one bit for LBP
		char Value_Compare( Vec3b pix_val, char channel, int val );

		// Exclude the same type of texture
		char* MoveBinary( char temp[8] );

		//---------Quick Sort Part----------//
		int partition( vector<LBP> hist_channel, int p, int r );

		void Sort( vector<LBP> hist_channel, int p, int r );
		//---------------------------------//

		//Generate LBP Histogram
		Hist LBP_Hist( Mat& img );
	};
}