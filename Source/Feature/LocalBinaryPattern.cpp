#include <vector>
#include <opencv2\opencv.hpp>
#include "LocalBinaryPattern.hpp"

using namespace cv;
using namespace ColorTextureShape;

Hist LocalBinaryPattern::LBP_Hist( Mat& img )
{
	Hist hist;

	for ( int i = 1; i < (img.rows - 1); i++ )
	{
		for ( int j = 1; j < (img.cols - 1); j++ )
		{
			Vec3b pix = img.at<Vec3b>(i, j);

			char lbp_temp_blue[8];
			char lbp_temp_green[8];
			char lbp_temp_red[8];

			// pick up rgb channels for one pixel
			int blue = (int)pix.val[0];
			int green = (int)pix.val[1];
			int red = (int)pix.val[2];

			// left up
			Vec3b channel_temp = img.at<Vec3b>(i-1, j-1);

			lbp_temp_blue[0] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[0] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[0] = Value_Compare( channel_temp, 'r', red );

			// up
			channel_temp = img.at<Vec3b>(i-1, j);

			lbp_temp_blue[1] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[1] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[1] = Value_Compare( channel_temp, 'r', red );

			// right up
			channel_temp = img.at<Vec3b>(i-1, j+1);
			lbp_temp_blue[2] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[2] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[2] = Value_Compare( channel_temp, 'r', red );

			// right
			channel_temp = img.at<Vec3b>(i, j+1);
			lbp_temp_blue[3] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[3] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[3] = Value_Compare( channel_temp, 'r', red );

			//right down
			channel_temp = img.at<Vec3b>(i+1, j+1);
			lbp_temp_blue[4] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[4] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[4] = Value_Compare( channel_temp, 'r', red );

			// down
			channel_temp = img.at<Vec3b>(i+1, j);
			lbp_temp_blue[5] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[5] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[5] = Value_Compare( channel_temp, 'r', red );

			// down left
			channel_temp = img.at<Vec3b>(i+1, j-1);
			lbp_temp_blue[6] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[6] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[6] = Value_Compare( channel_temp, 'r', red );

			//left
			channel_temp = img.at<Vec3b>(i, j-1);
			lbp_temp_blue[7] = Value_Compare( channel_temp, 'b', blue );
			lbp_temp_green[7] = Value_Compare( channel_temp, 'g', green );
			lbp_temp_red[7] = Value_Compare( channel_temp, 'r', red );

			char* t_b = MoveBinary( lbp_temp_blue );
			char* t_g = MoveBinary( lbp_temp_green );
			char* t_r = MoveBinary( lbp_temp_red );

			int bin_b = B2D( t_b );
			int bin_g = B2D( t_g );
			int bin_r = B2D( t_r );

			// blue_hist
			bool isexist = false;
			for ( int k = 0; k < hist.hist_blue.size(); k++ )
			{
				// search for the values that are already exist
				if ( hist.hist_blue[k].bin_id == bin_b )
				{
					hist.hist_blue[k].hist_value += 1;
					isexist = true;
					break;
				}
			}

			// if the LBP is not exist in the vector
			if ( isexist == false )
			{
				LBP temp;
				temp.bin_id = bin_b;
				temp.hist_value = 1;
				hist.hist_blue.push_back(temp);
			}

			// green_hist
			isexist = false;
			for ( int k = 0; k < hist.hist_green.size(); k++ )
			{
				// search for the values that are already exist
				if ( hist.hist_green[k].bin_id == bin_g )
				{
					hist.hist_green[k].hist_value += 1;
					isexist = true;
					break;
				}
			}

			// if the LBP is not exist in the vector
			if ( isexist == false )
			{
				LBP temp;
				temp.bin_id = bin_g;
				temp.hist_value = 1;
				hist.hist_green.push_back(temp);
			}

			//red_hist
			isexist = false;
			for ( int k = 0; k < hist.hist_red.size(); k++ )
			{
				// search for the values that are already exist
				if ( hist.hist_red[k].bin_id == bin_r )
				{
					hist.hist_red[k].hist_value += 1;
					isexist = true;
					break;
				}
			}

			// if the LBP is not exist in the vector
			if ( isexist == false )
			{
				LBP temp;
				temp.bin_id = bin_r;
				temp.hist_value = 1;
				hist.hist_red.push_back(temp);
			}
		}
	}

	Sort( hist.hist_blue, 0, hist.hist_blue.size()-1 );
	Sort( hist.hist_green, 0, hist.hist_green.size()-1 );
	Sort( hist.hist_red, 0, hist.hist_red.size()-1 );

	return hist;
}

int LocalBinaryPattern::B2D( char str[8] )
{
	char* pEnd;
	unsigned long long val;

	val = strtoul(str, &pEnd, 2);

	return (int)val;
}

char LocalBinaryPattern::Value_Compare( Vec3b pix_val, char channel, int val )
{
	char feedback;

	switch ( channel )
	{
	case 'b':
		// blue LBP
		if ( (int)pix_val.val[0] >= val )
		{
			return feedback = '1';
		}
		else 
		{
			return feedback = '0';
		}
		break;
	case 'g':
		// green LBP
		if ( (int)pix_val.val[1] >= val )
		{
			return feedback = '1';
		}
		else
		{
			return feedback = '0';
		}
		break;
	case 'r':
		// red LBP
		if ( (int)pix_val.val[2] >= val )
		{
			return feedback = '1';
		}
		else
		{
			return feedback = '0';
		}
		break;
	default:
		return 1;
		break;
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