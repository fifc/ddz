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

static const std::string CL = "\33[0;32m";
static const std::string EL = "\33[m";

std::string card_name(int card)
{
        if (card == 52)
                return "joker";
        if (card == 53)
                return "JOKER";
        if (card > 53)
                return "xx";
        int suit = card%4;
        int rank = card/4;
        std::string suit_name[] = {"h", "s", "d", "c"};
        std::string rank_name[] = {" 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", " J", " Q", " K", " A", " 2"};
        return  rank_name[rank]+ CL + suit_name[suit] + EL;
}

void show(int *cards)
{
	std::string msg;
	int i = 0;
	for ( ; i < 20; ++i) {
		int order = cards[i+34];
		if (order == 888) {
			msg += " XX ";
		} else {
			msg += card_name(order) + " ";
		}
	}
	msg = "\tLO[" + std::to_string(i) + "]" + msg;
	std::cout << msg << "\n";
	msg.clear();
	i = 0;
	for ( ; i < 17; ++i) {
		int order = cards[i];
		if (order == 888) {
			msg += " XX ";
		} else {
			msg += card_name(order) + " ";
		}
	}
	msg = "\tF1[" + std::to_string(i) + "]" + msg;
	std::cout << msg << "\n";
	msg.clear();
	i = 0;
	for ( ; i < 17; ++i) {
		int order = cards[i+17];
		if (order == 888) {
			msg += " XX ";
		} else {
			msg += card_name(order) + " ";
		}
	}
	msg = "\tF2[" + std::to_string(i) + "]" + msg;
	std::cout << msg << "\n";
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

