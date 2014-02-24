#include "CosineSimilarity.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

// L1 distance  Σ(v1(i)-v2(i))
double CosineSimilarity::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
		return -1;
	}
	double sum = 0;
	double sum_q = 0;
    double sum_d = 0;
	double sum_qd = 0;
	
    for(int i = 0; i < v1.size(); i++){
        if(v1.at(i) != 0){
			sum_q += v1.at(i) * v1.at(i);
			if(v2.at(i) != 0){
				sum_qd += v1.at(i) * v2.at(i);
			}
		}
		if(v2.at(i) != 0){
			sum_d += v2.at(i) * v2.at(i);
		}				
    }
	if(sum_q !=0 && sum_d != 0){
		sum = sum_qd/(std::sqrt(sum_q) * std::sqrt(sum_q));
	}
    return sum;
}
