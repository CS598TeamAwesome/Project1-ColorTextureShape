#pragma once
#include "Distance.hpp"

namespace ColorTextureShape
{

	class ChiSquareDistance: public Distance
	{
		public:
            double Compute(const std::vector<double> &v1, const std::vector<double> &v2) const;
	};

}
