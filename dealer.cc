#include <string.h>

#include "ddz.h"

static int comp_int(const void *p1, const void *p2) {
	if (*((int*)p1) > *((int*)p2))
		return 1;
	if (*((int*)p1) < *((int*)p2))
		return -1;
	return 0;

}

static void deal(int *cards)
{
	int buf[20];
	int mode = std::rand()%6;
	switch (mode) {
	case 1:
		for (int i = 0; i < 17; ++i) {
			int tmp = cards[i];
			cards[i] = cards[i+17];
			cards[i+17] = tmp;
		}
		break;
	case 2:
		for (int i = 17; i < 34; ++i) {
			int tmp = cards[i];
			cards[i] = cards[i+20];
			cards[i+20] = tmp;
		}
		break;
	case 3:
		for (int i = 17; i < 34; ++i) {
			int tmp = cards[i];
			cards[i] = cards[i+20];
			cards[i+20] = tmp;
		}
		for (int i = 0; i < 17; ++i) {
			int tmp = cards[i];
			cards[i] = cards[i+17];
			cards[i+17] = tmp;
		}
		break;
	case 4:
		for (int i = 0; i < 20; ++i)
			buf[i] = cards[i];
		for (int i = 0; i < 34; ++i)
			cards[i] = cards[i+20];
		for (int i = 0; i < 20; ++i)
			cards[i+34] = buf[i];
		break;
	case 5:
		for (int i = 0; i < 20; ++i)
			buf[i] = cards[i];
		for (int i = 0; i < 17; ++i)
			cards[i] = cards[i+37];
		for (int i = 17; i < 34; ++i)
			cards[i] = cards[i+3];
		for (int i = 0; i < 20; ++i)
			cards[i+34] = buf[i];
		break;
	default:
		break;
	}
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
	deal(cards);
	std::qsort(cards, 17, sizeof (int), comp_int);
	std::qsort(cards+17, 17, sizeof (int), comp_int);
	std::qsort(cards+34, 17, sizeof (int), comp_int);
	std::qsort(cards+51, 3, sizeof (int), comp_int);
}

void assign(int *cards, int seat)
{
	int buf[17];
	if (seat == 0) {
		memmove(buf, cards, 17 * sizeof (int));
		memmove(cards, cards + 17, 34 * sizeof (int));
		memmove(cards + 34, buf, 17 * sizeof (int));
	}
	if (seat == 1) {
		memmove(buf, cards + 34, 17 * sizeof (int));
		memmove(cards + 17, cards, 34 * sizeof (int));
		memmove(cards, buf, 17 * sizeof (int));
	}
	std::qsort(cards+34, 20, sizeof (int), comp_int);
}

