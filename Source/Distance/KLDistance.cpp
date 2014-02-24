#include "KLDistance.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

// L1 distance  Σ(v1(i)-v2(i))
double KLDistance::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
     return -1;
    }
    
    double sum = 0;
    double sum_1 = 0;
    double sum_2 = 0;
    
    for(int i = 0; i < v1.size(); i++){
     if(v1.at(i) - v2.at(i) != 0){
         if(v1.at(i) != 0){
             sum_1 = v1.at(i) * std::log(v1.at(i)) / std::log(v2.at(i));
         }
         if(v2.at(i) != 0){
             sum_2 = v2.at(i) * std::log(v2.at(i)) / std::log(v1.at(i));
         }
     }	
    }		
         
    sum = (sum_1 + sum_2) / 2;
    return sum;
}
