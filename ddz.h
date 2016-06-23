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
	card(int rnk, suit_t suit): rank(rnk), suit(suit) {}

	card(int order)
	{
		if (order > 51) {
			rank = order;
		} else {
			suit = static_cast<suit_t>(((order%4)+1));
			rank = order/4;
		}
	}
	int order() const
	{
		if (rank > 51) return rank;
		return rank * 4 + suit - 1;
	}
};

enum play_type_t {
	invalidplay = 0,
	single,
	pair,
	sequence,
	pair_sequence,
	three_with_none,
	three_with_single,
	three_with_pair,
	bomb,
	four_with_two,
	four_with_two_pair,
	plane,
	plane_with_wing,
	plane_with_pair_wing,
};

struct play {
	play_type_t type = invalidplay;
	std::vector<card> cards;
	play() {}
	play(const std::vector<card>& cards);
	bool isplane() const {
		return type == plane || type == plane_with_wing || type == plane_with_pair_wing;
	}
};

bool operator > (const play& play1, const play play2);

struct player {
	role_t role;
	card cards[20];
	card all_cards[20];
	std::list<play> history;
	std::list<play> planning;
};

play_type_t SortParsePlane(card *cards, int count);

struct Game {
	role_t role_;
	player farmer1_ = {role: farmer1};
	player farmer2_ = {role: farmer2};
	player landowner_ = {role: landowner};
	std::vector<card> cards_;

	play Parse(const std::vector<card>& cards);
	bool Init(role_t role, const std::vector<card>& cards);
	bool commit(const play& curplay, role_t role);
	static bool init_cards(card *cards, card *all_cards, const std::vector<card>& input);
	role_t target_role_;
	play * target_play_ = nullptr;
	bool skip(role_t role) {
		if (target_play_ == nullptr)
			return false;
		return role != target_role_;
	}
};

#endif // DDZ_H_INLUDED_
