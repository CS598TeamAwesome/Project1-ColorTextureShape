#pragma once
#include "Similarity.hpp"

namespace ColorTextureShape
{
	class CosineSimilarity: public Similarity
	{
		public:
            double Compute(const std::vector<double> &v1, const std::vector<double> &v2) const;
	};
}
