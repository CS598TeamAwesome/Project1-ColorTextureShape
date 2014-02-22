#include "HistogramFeature.hpp"

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

		// Generate one bit for LBP
		char Value_Compare( Vec3b pix_val, int val );

		// Exclude the same type of texture
		char* MoveBinary( char temp[8] );

		//---------Quick Sort Part----------//
		int partition( vector<LBP> hist_channel, int p, int r );

		void Sort( vector<LBP> hist_channel, int p, int r );
		//---------------------------------//

		//Generate LBP Histogram
		vector<LBP> LBP_Hist( Mat& img );
	};
}