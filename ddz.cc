#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "ddz.h"

play Game::Parse(const std::vector<card>& cards)
{
	if (cards.empty())
		return play();
	int count = cards.size();

	for (int i = 0; i < count; ++i) {
	}

	play p;
	return p;
}

bool Game::Init(role_t role, const std::vector<card>& cards) {
	role_ = role;
	if (role == landowner) {
		if (cards.size() != 20) {
			return false;
		}
		return init_cards(landowner_.cards, landowner_.all_cards, cards);
	}
	player<> *p = nullptr;
	if (role == farmer1)
		p = &farmer1_;
	if (role == farmer2)
		p = &farmer2_;
	if (p == nullptr)
		return false;
	if (cards.size() != 17)
		return false;
	return init_cards(p->cards, p->all_cards, cards);
}

static int comp_card_order(const void *p1, const void *p2) {
        card *card1 = (card*)p1;
        card *card2 = (card*)p2;
        if (card1->rank > card2->rank)
                return 1;
        if (card1->rank < card2->rank)
                return -1;
        if (card1->suit < card2->rank)
                return 1;
        if (card1->suit > card2->rank)
                return -1;
        return 0;
}

bool Game::init_cards(card *cards, card *all_cards, const std::vector<card>& input)
{
	long mask = 0;
        for (unsigned i = 0; i < input.size(); ++i) {
		auto order = input[i].order();
		if (mask&(1L<<order)) {
			return false; // dup cards
		}
		mask |= 1L<<order;
                cards[i] = input[i];
        }
	std::qsort(cards, input.size(), sizeof(card), comp_card_order);
        *all_cards = *cards;
        return true;
}
