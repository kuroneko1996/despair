#include "utility.h"
#include <algorithm>
#include "libtcod.hpp"

namespace utility {
	std::vector<int> calculate_cdf(std::vector<int>& chances)
	{
		std::vector<int> result;
		int sum = 0;
		for (int i = 0; i < chances.size(); i++) {
			sum += chances[i];
			result.push_back(sum);
		}
		return result;
	}

	int get_random(std::vector<int>& chances)
	{
		std::vector<int> sumc = calculate_cdf(chances);

		int right_value = my_rand(0, sumc.at(sumc.size()-1)); // TODO replace it with good implementation
		std::vector<int>::iterator it;
		it = std::lower_bound(sumc.begin(), sumc.end(), right_value);
		int index = it - sumc.begin();
		return index;
	}
	int my_rand(int low, int high) {
		TCODRandom *rng = TCODRandom::getInstance();
		return rng->getInt(low, high);
		//return(rand() % (high - low + 1) + low);
	}
}
