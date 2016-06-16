#include "ddz.h"

static int comp_int(const void *p1, const void *p2) {
	if (*((int*)p1) > *((int*)p2))
		return 1;
	if (*((int*)p1) < *((int*)p2))
		return -1;
	return 0;

}

void deal_cards(int *cards)
{
	for (int i = 0; i < 54; ++i)
		cards[i] = i;
	int index = std::rand()%54;
	int card = cards[index];
	for (int i = 0; i < 54 * 5; ++i) {
		int insert = std::rand()%54;
		int tmp = cards[insert];
		cards[insert] = card;
		card = tmp;
		if (insert == index) {
			index = std::rand()%54;
			card = cards[index];
		}
	}
	cards[index] = card;
	std::qsort(cards, 17, sizeof (int), comp_int);
	std::qsort(cards+17, 17, sizeof (int), comp_int);
	std::qsort(cards+34, 20, sizeof (int), comp_int);
}
