#include "HistogramIntersection.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

//Histogram Intersection ∑(i)( min⁡(HI(i), HJ(i)) )
double HistogramIntersection::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
		return -1;
	}
	double sum = 0;
	
    for(int i = 0; i < v1.size(); i++){
        sum += std::min (v1.at(i), v2.at(i));		
    }
    return sum;
}
