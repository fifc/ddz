#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include "ddz.h"
#include "run.h"
#include "dealer.h"

void testplane()
{
	card cards[] = {
		{5, heart}, {5, spade}, {5, diamond}, {5, club},
		{9, heart}, {9, spade}, {9, diamond}, {9, club},
	};
	auto type = SortParsePlane(cards, sizeof cards / sizeof (card));
	switch (type) {
	case plane:
		std::cout << "plane" << std::endl;
		break;
	case plane_with_wing:
		std::cout << "plane_with_wing" << std::endl;
		break;
	case plane_with_pair_wing:
		std::cout << "plane_with_pair_wing" << std::endl;
		break;
	default:
		std::cout << "none plane type: " << type << std::endl;
	}
}

bool Init(Game *g, int *cards)
{
	deal_cards(cards);
	std::vector<card> card_vect;
	card_vect.resize(20);
	for (int i = 0; i < 20; ++i) {
		card_vect[i] = card(cards[i+34]);
	}
	return g->Init(landowner, card_vect);
}

int main(int argc, char *argv[])
{
	std::srand(std::time(0));

	for ( ; ; ) {
		int cards[54];
		Game game;
		if (!Init(&game, cards)) {
			std::cerr << "init error!\n";
		}
		run(cards, &game);
	}
}

