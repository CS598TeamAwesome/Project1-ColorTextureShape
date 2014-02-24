#include "KLDistance.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

//     D(I, J) = KL(I, J) + KL(J, I) / 2
//		KL(I, J) = ∑(i) (HI(i) * log(HI(i)/HJ(i)))
double KLDistance::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
		return -1;
	}
    
	double sum = 0;
   	double sum_1 = 0; // KL(I, J)
	double sum_2 = 0; // KL(J, I)
	
    for(int i = 0; i < v1.size(); i++){
        if(v1.at(i) - v2.at(i) != 0){ // if =0 then log(HI(i)/HJ(i)) and log(HJ(i)/HI(i)) both are 0, no need add to sum_1, sum_2;
			if(v1.at(i) != 0 && v2.at(i) != 0){ // both HI(i) and HJ(i) can not be 0;
				sum_1 = v1.at(i) * std::log(v1.at(i)) - std:log(v2.at(i));
				sum_2 = v2.at(i) * std::log(v2.at(i)) - std:log(v1.at(i));
			}
		}	
    }		
            
    sum = (sum_1 + sum_2) / 2;
    return sum;
}
