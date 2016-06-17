#ifndef DDZ_H_INLUDED_
#define DDZ_H_INLUDED_

#include <list>
#include <map>
#include <vector>
#include <functional>

enum role_t {
	landowner = 0,
	farmer1 = 1,
	farmer2 = 2,
};

enum suit_t {
	unknown = 0,
	heart = 1,
	spade = 2,
	diamond = 3,
	club  = 4,
};

struct card {
	int rank;
	suit_t suit;

	card(): rank(-1), suit(unknown) {}

	card(int seq)
	{
		if (seq >= 52)
			rank = seq;
		suit = static_cast<suit_t>(((seq%4)+1));
		rank = seq/4;
	}
	int order() const
	{
		return rank * 4 + suit - 1;
	}
};

enum playing_type_t {
	invalidplay = 0,
	single = 1,
	pair = 2,
	sequence = 3,
	pair_sequence = 4,
	three_with_single = 5,
	three_with_pair = 6,
	bomb = 7,
	four_with_two = 8,
	four_with_two_pair = 9,
	plane = 10,
	plane_with_wing = 11,
	plane_with_pair_wing = 12,
};

struct play {
	playing_type_t type = invalidplay;
	std::vector<card> cards;
	play() {}
	play(const std::vector<card>& cards);
};

template <int N = 17>
struct player {
	role_t role;
	card cards[N];
	card all_cards[N];
	std::list<play> history;
	std::list<play> planning;
};

struct Game {
	role_t role_;
	player<> farmer1_ = {role: farmer1};
	player<> farmer2_ = {role: farmer2};
	player<20> landowner_ = {role: landowner};
	std::vector<card> cards_;

	play Parse(const std::vector<card>& cards);
	bool Init(role_t role, const std::vector<card>& cards);
	static bool init_cards(card *cards, card *all_cards, const std::vector<card>& input);
};

#endif // DDZ_H_INLUDED_
