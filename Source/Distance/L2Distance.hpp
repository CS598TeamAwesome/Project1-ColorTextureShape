#pragma once
#include "Distance.hpp"

namespace ColorTextureShape
{

	class L2Distance: public Distance
	{
		public:
            double Compute(const std::vector<double> &v1, const std::vector<double> &v2) const;
	};

}
