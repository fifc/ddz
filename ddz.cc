#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "ddz.h"

bool ThreeGreater(const std::vector<card>& cards1, const std::vector<card>& cards2)
{
	int rank1 = 0;
	for (unsigned i = 0; i < cards1.size() - 2; ++i) {
		if (cards1[i].rank == cards1[i+1].rank && cards1[i].rank == cards1[i+2].rank) {
			rank1 = cards1[i].rank;
			break;
		}
	}
	int rank2 = 0;
	for (unsigned i = 0; i < cards2.size() - 2; ++i) {
		if (cards2[i].rank == cards2[i+1].rank && cards2[i].rank == cards2[i+2].rank) {
			rank2 = cards2[i].rank;
			break;
		}
	}
	return rank1 > rank2;
}

static bool operator > (const play& play1, const play play2) {
	if (play1.type != play2.type)
		return play1.type == bomb;
	if (play1.cards.size() != play2.cards.size())
		return play1.cards.size() == 2 &&
			play1.cards[0].rank == 52 &&
			play1.cards[1].rank == 53;

        if (play1.type != single && play1.type != pair && play1.type != sequence && play1.type != pair_sequence)
		return ThreeGreater(play1.cards, play2.cards);

	return play1.cards.empty() || play1.cards[0].rank > play2.cards[0].rank;
}

bool Game::commit(const play& curplay, role_t role)
{
	if (target_role_ == role) {
		target_play_ = nullptr;
	}
	if (target_play_ != nullptr) {
		if (!(curplay > *target_play_))
			return false;
	}
	target_role_ = role;
	player *p = nullptr;
	if (role == landowner) {
		p = &landowner_;
	} else if (role == farmer1) {
		p = &farmer1_;
	} else {
		p = &farmer2_;
	}

	p->history.push_back(curplay);
	target_play_ = &p->history.back();

	return true;
}

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

static int build_rank_map(const card *cards, int *rank_map, int count, bool sort = false)
{
	for (int i = 1; i < count; ++i)
		rank_map[i] = 0;

	int n = 0;
	rank_map[0] = 1;
	int rank = cards[0].rank;
	for (int i = 1; i < count; ++i) {
		if (cards[i].rank != rank) {
			rank = cards[i].rank;
			n++;
		}
		rank_map[n]++;
	}
	if (sort)
		std::qsort(rank_map, n+1, sizeof rank_map[0], comp_int);
	return n + 1;
}

static int get_rank(const card *cards, int count, int index)
{
	int n = 0;
	int i = 0;
	for ( ; i < count && n < index; ++n) {
		int rank = cards[i++].rank;
		for ( ; i < count && cards[i].rank == rank; )
			++i;
	}
	return n == index ? cards[i].rank : -1;
}

static play_type_t ParsePlane(const card *cards, int count)
{
	if (((count%3) != 0 && (count%4) != 0 && (count%5) != 0) || count < 6)
		return invalidplay;

	int rank_map[count];
	int n = build_rank_map(cards, rank_map, count);
	int single_num = 0;
	int pair_num = 0;
	int triple_num = 0;
	for (int i = 0; i < n; ++i) {
		if (rank_map[i] == 1) {
			single_num++;
			if (pair_num != 0) {
				single_num += (pair_num<<1);
				pair_num = 0;
			}
			continue;
		}
		if (rank_map[i] == 2) {
			if (single_num != 0)
				single_num += 2;
			else
				pair_num++;
			continue;
		}
		int j = i + 1;
		int rank = get_rank(cards, count, i);
		for ( ; j < n; ++j) {
			if (rank_map[j] < 3 || get_rank(cards, count, j) != rank + 1)
				break;
			rank++;
		}
		if (j < i + 2) {
			if (rank_map[i] == 3) {
				single_num += 3;
				if (pair_num != 0) {
					single_num += (pair_num<<1);
					pair_num = 0;
				}
			}
			if (rank_map[i] == 4) {
				if (single_num != 0)
					single_num += 4;
				else
					pair_num += 2;
			}
			continue;
		}

		if (triple_num != 0)
			return invalidplay;

		for ( ; i < j; ++i) {
			triple_num++;
			if (rank_map[i] == 4) {
				single_num++; 
				if (pair_num != 0) {
					single_num += (pair_num<<1);
					pair_num = 0;
				}
			}
		}
		i--;
	} 

	if (triple_num >= 2) {
std::cout << "=========== " << __LINE__ << ": t" << triple_num << "/s" << single_num << "/p" << pair_num << std::endl;
		if (single_num == triple_num && pair_num == 0)
			return plane_with_wing;
		if ((pair_num<<1) == triple_num && single_num == 0)
			return plane_with_wing;
		if (pair_num == triple_num && single_num == 0)
			return plane_with_pair_wing;
		if (single_num == 0 && pair_num == 0)
			return plane;
	}

	return invalidplay;
}

play_type_t SortParsePlane(card *cards, int count)
{
	std::qsort(cards, count, sizeof (card), comp_card_order);
	return ParsePlane(cards, count);
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

	int rank_map[count+1];
	build_rank_map(tmp_cards, rank_map, count, true);
	rank_map[count] = 0;

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
			if (!has3s && rank_map[i+1] >= 3) {
				has3s = true;
				++i;
			}
		} else if (n == 3 && !has3s) {
			has3s = rank_map[i+1] >= 3;
			if (has3s) ++i;
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

	if (has3s)
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
	player *p = nullptr;
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
