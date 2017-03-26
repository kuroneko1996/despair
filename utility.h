#pragma once
#include <vector>

namespace utility {
	struct Transition {
		int level;
		int value;
	};

	std::vector<int> calculate_cdf(std::vector<int>& chances);
	int get_random(std::vector<int>& chances);
	int my_rand(int low, int high);

	int fromDungeonLevel(std::vector<Transition>& table, int level);
}
