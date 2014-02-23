#include "LocalBinaryPattern.hpp"

using namespace cv;
using namespace ColorTextureShape;

vector<LBP> LocalBinaryPattern::LBP_Hist( Mat& img )
{
	vector<LBP> hist;

	for ( int i = 1; i < (img.rows - 1); i++ )
	{
		for ( int j = 1; j < (img.cols - 1); j++ )
		{
			Vec3b pix = img.at<Vec3b>(i, j);

			// Calculate the grayscale value of the pixel
			int gray = ( pix.val[2]*299 + pix.val[1]*587 + pix.val[0]*114 + 500)/1000;

			char lbp_temp[8];

			// left up
			Vec3b channel_temp = img.at<Vec3b>(i-1, j-1);

			lbp_temp[0] = Value_Compare( channel_temp, gray );

			// up
			channel_temp = img.at<Vec3b>(i-1, j);

			lbp_temp[1] = Value_Compare( channel_temp, gray );

			// right up
			channel_temp = img.at<Vec3b>(i-1, j+1);
			lbp_temp[2] = Value_Compare( channel_temp, gray );

			// right
			channel_temp = img.at<Vec3b>(i, j+1);
			lbp_temp[3] = Value_Compare( channel_temp, gray );

			//right down
			channel_temp = img.at<Vec3b>(i+1, j+1);
			lbp_temp[4] = Value_Compare( channel_temp, gray );

			// down
			channel_temp = img.at<Vec3b>(i+1, j);
			lbp_temp[5] = Value_Compare( channel_temp, gray );

			// down left
			channel_temp = img.at<Vec3b>(i+1, j-1);
			lbp_temp[6] = Value_Compare( channel_temp, gray );

			//left
			channel_temp = img.at<Vec3b>(i, j-1);
			lbp_temp[7] = Value_Compare( channel_temp, gray );

			char* t = MoveBinary( lbp_temp );

			int bin = B2D( t );

			// blue_hist
			bool isexist = false;
			for ( int k = 0; k < hist.size(); k++ )
			{
				// search for the values that are already exist
				if ( hist[k].bin_id == bin )
				{
					hist[k].hist_value += 1;
					isexist = true;
					break;
				}
			}

			// if the LBP is not exist in the vector
			if ( isexist == false )
			{
				LBP temp;
				temp.bin_id = bin;
				temp.hist_value = 1;
				hist.push_back(temp);
			}
		}
	}

	Sort( hist, 0, (hist.size()-1) );

	return hist;
}

int LocalBinaryPattern::B2D( char str[8] )
{
	char* pEnd;
	unsigned long long val;

	val = strtoul(str, &pEnd, 2);

	return (int)val;
}

char LocalBinaryPattern::Value_Compare( Vec3b pix_val, int val )
{
	char feedback;

	int gray = ( pix_val.val[2]*299 + pix_val.val[1]*587 + pix_val.val[0]*114 + 500)/1000;

	// blue LBP
	if ( gray >= val )
	{
		return feedback = '1';
	}
	else 
	{
		return feedback = '0';
	}
}

char* LocalBinaryPattern::MoveBinary( char temp[8] )
{
	int it = 0;
	while ( temp[7] == '0' )
	{
		char t = temp[7];
		temp[7] = temp[6];
		temp[6] = temp[5];
		temp[5] = temp[4];
		temp[4] = temp[3];
		temp[3] = temp[2];
		temp[2] = temp[1];
		temp[1] = temp[0];
		temp[0] = t;
		it++;
		if ( it > 6 )
		{
			break;
		}
	}
	return temp;
}

int LocalBinaryPattern::partition( vector<LBP> hist_channel, int p, int r )
{
	int s = hist_channel[r].bin_id;
	int mid = p;

	for ( int i = p; i < r; i++ )
	{
		if ( hist_channel[i].bin_id < s )
		{
			LBP temp = hist_channel[mid];
			hist_channel[mid] = hist_channel[i];
			hist_channel[i] = temp;
			mid++;
		}
	}

	LBP temp = hist_channel[r];
	hist_channel[r] = hist_channel[mid];
	hist_channel[mid] = temp;

	return mid;
}

void LocalBinaryPattern::Sort( vector<LBP> hist_channel, int p, int r )
{
	if ( p < r )
	{
		int q = partition( hist_channel, p, r );
		Sort( hist_channel, p, q-1 );
		Sort( hist_channel, q+1, r );
	}
}
