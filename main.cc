#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <cstdlib>
#include <ctime>

#include "ddz.h"
#include "run.h"
#include "dealer.h"

bool Init(Game *g, int *cards)
{
	std::srand(std::time(0));
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
	int cards[54];
	Game game;
	if (!Init(&game, cards)) {
		std::cerr << "init error!\n";
	}
	run(cards, &game);
}

