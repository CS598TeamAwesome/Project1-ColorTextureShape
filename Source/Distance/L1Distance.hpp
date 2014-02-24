#pragma once
#include "Distance.hpp"

namespace ColorTextureShape
{

	class L1Distance: public Distance
	{
		public:
            double Compute(const std::vector<double> &v1, const std::vector<double> &v2) const;
	};

}
