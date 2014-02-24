#pragma once
#include "Similarity.hpp"

namespace ColorTextureShape
{
	class HistogramIntersection : public Similarity
	{
		public:
            double Compute(const std::vector<double> &v1, const std::vector<double> &v2) const;
	};
}
