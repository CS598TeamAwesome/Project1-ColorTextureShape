#include "CosineSimilarity.hpp"
#include <cmath>

using namespace std;
using namespace ColorTextureShape;

  // CosineSimilarity cos(q, d) (q, d are vectors here) = 
  //	 ∑(i) (q(i) . d(i)) /  ( sqrt(∑(i) q(i)^2) * sqrt(∑(i) d(i)^2) )
double CosineSimilarity::Compute(const vector<double> &v1, const vector<double> &v2) const {
    if(v1.size() != v2.size()){
		return -1;
	}
	double sum = 0;
	double sum_q = 0; //Σ(i) q(i)^2
    	double sum_d = 0; //Σ(i) d(i)^2
	double sum_qd = 0; // Σ(i) (q(i) . d(i))
	
    for(int i = 0; i < v1.size(); i++){
	sum_q += v1.at(i) * v1.at(i);
	sum_d += v2.at(i) * v2.at(i);
	sum_qd += v1.at(i) * v2.at(i);
    }
	sum = sum_qd/(std::sqrt(sum_q) * std::sqrt(sum_q));
    	return sum;
}
