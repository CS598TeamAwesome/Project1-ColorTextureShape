#include "L2Distance.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

// L1 distance  Σ(v1(i)-v2(i))
double L2Distance::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
        return -1;
    }

    double sum = 0;

    for(int i = 0; i < v1.size(); i++){
        if(v1.at(i) - v2.at(i) != 0){
            sum += std::abs(v1.at(i) - v2.at(i));
            sum *= sum;
        }
    }
    
    sum = std::sqrt(sum);
    return sum;
}
