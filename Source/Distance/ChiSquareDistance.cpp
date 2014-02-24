#include "ChiSquareDistance.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

//  ∑(i) { 2 * (|HI(i)- HJ(i)|^2])  /  (HI(i) + HJ(i)) }
double ChiSquareDistance::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
        return -1;
    }

    double sum = 0;
    double sub_sum = 0; // sub_sum use to get the result of 2 * (|HI(i)- HJ(i)|^2])
    
    for(int i = 0; i < v1.size(); i++){
        if(v1.at(i) - v2.at(i) != 0){
            sub_sum = std::abs(v1.at(i) - v2.at(i));
            sub_sum *= sub_sum;		// ^2	
            sum += sub_sum * 2/(v1.at(i) + v2.at(i));
        }
    }
    return sum;
}

