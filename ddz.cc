#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "ddz.h"

static int comp_int(const void *p1, const void *p2) {
	if (*((int*)p1) > *((int*)p2))
		return 1;
	if (*((int*)p1) < *((int*)p2))
		return -1;
	return 0;
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

static play_type_t ParsePlane(const card *tmp_cards, int count)
{
	return invalidplay;
}

play Game::Parse(const std::vector<card>& cards)
{
	play p;
	int count = cards.size();
	if (count == 0)
		return p;

	if (count == 1) {
		p.type = single;
		p.cards = cards;
		return p;
	}
	if (count == 2) {
		if (cards[1].rank == cards[0].rank && cards[1].suit != cards[0].suit) {
			p.type = single;
			p.cards = cards;
		} else if (cards[0].rank >= 52 && cards[1].rank >= 52 && cards[0].rank != cards[1].rank) {
			p.type = bomb;
			p.cards = cards;
		}
		return p;
	}
	if (count == 3) {
		if (cards[1].rank == cards[0].rank && cards[2].rank == cards[0].rank &&
			cards[1].suit != cards[0].suit && cards[2].suit != cards[0].suit && cards[1].suit != cards[2].suit) {
			p.type = three_with_none;
			p.cards = cards;
		}
		return p;
	}

	long mask = 0L;
	card tmp_cards[count];
	for (int i = 0; i < count; ++i) {
		long order_bit = 1L<<cards[i].order();
		if (mask&order_bit)
			return p; // dup!
		mask |= order_bit;
		tmp_cards[i] = cards[i];
	}
	std::qsort(tmp_cards, count, sizeof (card), comp_card_order);

	p.cards.resize(count);
	if (count == 4) {
		if (tmp_cards[0].rank == tmp_cards[3].rank) 
			p.type = bomb;
		else if (tmp_cards[0].rank == tmp_cards[2].rank || 
			tmp_cards[1].rank == tmp_cards[3].rank)
			p.type = three_with_single;
		else {
			p.cards.resize(0);
			return p;
		}
		for (int i = 0; i < count; ++i)
			p.cards[i] = tmp_cards[i];
		return p;
	}

	bool isSequence = true;
	p.cards[0] = tmp_cards[0];
	for (int i = 1; i < count; ++i) {
		p.cards[i] = tmp_cards[i];
		if (p.cards[i].rank != p.cards[i-1].rank + 1)
			isSequence = false; 
	}

	if (isSequence && tmp_cards[count-1].rank < 12) {
		p.type = sequence;
		return p;
	}

	int rank_map[count];
	for (int i = 1; i < count; ++i)
		rank_map[i] = 0;

	int n = 0;
	rank_map[0] = 1;
	int rank = tmp_cards[0].rank;
	for (int i = 1; i < count; ++i) {
		if (tmp_cards[i].rank != rank) {
			rank = tmp_cards[i].rank;
			n++;
		}
		rank_map[n]++;
	}
	std::qsort(rank_map, n+1, sizeof rank_map[0], comp_int);
	if (count == 5) {
		if (rank_map[2] == 0 && rank_map[0] == 2)
			p.type = three_with_pair;
		return p;
	}
	if ((count%2) == 0) {
		bool ok = true;
		for (int i = 0; rank_map[i] != 0; ++i) {
			if (rank_map[i] != 2) {
				ok = false;
				break;
			}
		}
		if (ok) {
			p.type = pair_sequence;
			return p;
		}
	}

	bool has4 = false;
	bool has3s = false;
	for (int i = 0; rank_map[i] != 0; ++i) {
		int n = rank_map[i];
		if (n == 4) {
			has4 = true;
		} else if (n == 3 && !has3s) {
			has3s = rank_map[++i] == 3;
		}
	}

	if (has4) {
		if (rank_map[3] == 0) {
			if ((rank_map[2] == 0 && rank_map[0] == 2) ||
				(rank_map[0] == 1 && rank_map[1] == 1))
				p.type = four_with_two;
			else if (rank_map[0] == 2 && rank_map[1] == 2)
				p.type = four_with_two_pair;
		}
		return p;
	}

	if (has3s && (count%3) == 0)
		p.type = ParsePlane(tmp_cards, count);

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
