
#include "advisor.h"

play advise(const int *cards, int count, const play& target)
{
	std::vector<card> vect;
	vect.resize(count);
	int n = 0;
	for (int i = 0; i < count; ++i) {
		if (cards[i] >= 0)
			vect[n++] = card(cards[i]);
	}
	vect.resize(n);
	return advise(vect, target);
}

play advise(const std::vector<card>& cards, const play& target)
{
	return play();
}
