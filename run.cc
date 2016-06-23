#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <strings.h>
#include <cstdio>
#include <unistd.h>

#include "ddz.h"
#include "dealer.h"

static bool prompt(int *cards, int step, char *buf, int len);

static std::vector<std::string> split(const std::string &s, char delim = ' ') {
        std::stringstream ss(s);
        std::vector<std::string> elems;
        std::string item;
        while (std::getline(ss, item, delim)) {
                elems.push_back(item);
        }
        return elems;
}

static card checkout(int *cards, int count, char name)
{
	int rank = -1;
	if (name == 'G' || name == 'g')
		rank = 52;
	else if (name == 'W' || name == 'w')
		rank = 53;
	else if (name == '3')
		rank = 0;
	else if (name == '4')
		rank = 1;
	else if (name == '5')
		rank = 2;
	else if (name == '6')
		rank = 3;
	else if (name == '7')
		rank = 4;
	else if (name == '8')
		rank = 5;
	else if (name == '9')
		rank = 6;
	else if (name == '1' || name == 'x' || name == 'X')
		rank = 7;
	else if (name == 'J' || name == 'j')
		rank = 8;
	else if (name == 'Q' || name == 'q')
		rank = 9;
	else if (name == 'K' || name == 'k')
		rank = 10;
	else if (name == 'A' || name == 'a')
		rank = 11;
	else if (name == '2')
		rank = 12;
	else
		return card();

	card the_card;
	the_card.rank = rank;

	for (int i = 0; i < count; ++i) {
		int order = cards[i];
		if (order < 0)
			continue;
		if (rank >= 52 && order == rank) {
			cards[i] = 888;
			return the_card;
		}
		if ((order/4) == rank) {
			cards[i] = 888;
			the_card.suit = static_cast<suit_t>((order%4)+1);
			return the_card;
		}
	}

	return card();
}

static std::vector<card> checkout(int *cards, int count, const char *input)
{
	int tmp_cards[count];
	for (int i = 0; i < count; ++i) {
		tmp_cards[i] = cards[i];
	}
	std::vector<card> cur_cards;
	cur_cards.resize(count);
	int n = 0;
	for (const char *p = input; *p != 0; ++p) {
		char ch = *p;
		if (ch == ' ' || ch == '\t')
			continue;
		if (ch == 'J') {
			if (strncasecmp(p, "joker", 5) == 0) {
				p += 5;
				ch = 'W';
			}
		}
		else if (ch == 'j') {
			if (strncasecmp(p, "joker", 5) == 0) {
				p += 4;
				ch = 'g';
			}
		}
		else if (ch == '1') {
			if (p[1] == '0')
				p++;
			else
				return std::vector<card>();
		}
		auto the_card = checkout(tmp_cards, count, ch);
		if (the_card.rank < 0 || n >= count)
			return std::vector<card>();
		
		cur_cards[n++] = the_card;
	}

	cur_cards.resize(n);

	return cur_cards;
}

static std::vector<card> parse(char *buf, int *cards, int role)
{
	int total_card = 17;
	int *start_card = cards;
	if (role == 0) {
		total_card = 20;
		start_card += 34;
	}
	if (role == 2) {
		start_card += 17;
	}

	return checkout(start_card, total_card, buf);
}

bool commit(int *cards, std::vector<card>& cur_cards, int role)
{
	if (cur_cards.empty())
		return false;
	int count = 17;
	int *start = cards;
	if (role == 2)
		start = cards + 17;
	if (role == 0) {
		start = cards + 34;
		count = 20;
	}
	int tmp_cards[count];
	for (int i = 0; i < count; ++i)
		tmp_cards[i] = start[i];

	for (unsigned i = 0; i < cur_cards.size(); ++i) {
		int order = cur_cards[i].rank * 4 + cur_cards[i].suit - 1;
		if (cur_cards[i].rank >= 52)
			order = cur_cards[i].rank;
		int j = 0;
		for ( ; j < count; ++j) {
			if (tmp_cards[j] == order) {
				tmp_cards[j] = 888;
				break;
			}
		}
		if (j >= count) {
			return false;
		}
	}

	for (int i = 0; i < count; ++i)
		start[i] = tmp_cards[i];

	return true;
}

static bool finished(int *cards, int role)
{
	int count = 17;
	int *start = cards;
	if (role == 2)
		start = cards + 17;
	if (role == 0) {
		start = cards + 34;
		count = 20;
	}
	for (int i = 0; i < count; ++i) {
		if (start[i] != 888)
			return false;
	}
	const char *player = "农民";
	if (role == 0)
		player = "地主";
	std::cout << "\033c\n\n\n\t\t" << player << "赢！\n\n\n";
	sleep(2);
	return true;
}

void beep()
{
	std::cerr << "\x07";
}

void assign(int *cards)
{
	assign(cards, std::rand()%3);
}

void run(int *cards, Game *game)
{
	char buf[1024];
	for (int j = 0; ; ) {
		int role = j%3;
		if (!prompt(cards, role, buf, sizeof buf))
			break;
		if (buf[0] == 0) {
			if (game->skip((role_t)role))
				++j;
			else
				beep();
			continue;
		}
		auto cur_cards = parse(buf, cards, role);
		auto cur_play = game->Parse(cur_cards);
		if (cur_play.type == invalidplay) {
			beep();
			continue;
		}
		if (!game->commit(cur_play, (role_t)role)) {
			beep();
			continue;
		}
		if (commit(cards, cur_cards, role)) {
			if (finished(cards, role))
				break;
			++j;
		} else { 
			beep();
		}
	}
}

//static const std::string CL = "\33[0;32m";
//static const std::string EL = "\33[m";
static const std::string CL;
static const std::string EL;

static std::string card_name(int card)
{
        if (card == 52)
                return "joker";
        if (card == 53)
                return "JOKER";
        if (card > 53)
                return "xx";
        int suit = card%4;
        int rank = card/4;
        //std::string suit_name[] = {"h", "s", "d", "c"};
        std::string suit_name[] = {"", "", "", ""};
        std::string rank_name[] = {" 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", " J", " Q", " K", " A", " 2"};
        return  rank_name[rank]+ CL + suit_name[suit] + EL;
}

static void show(int *cards, int role)
{
	std::cout << "\033c\n";
	std::string msg;
	int i = 0;
	int n = 0;
	for ( ; i < 20; ++i) {
		int order = cards[i+34];
		if (order != 888) {
			++n;
			msg += card_name(order) + " ";
		}
	}
	if (role==0)
		msg = "\t地主[" + std::to_string(n) + "] \x1b[7m" + msg + "\x1b[0m";
	else
		msg = "\t地主[" + std::to_string(n) + "] " + msg;
	std::cout << msg << "\n";
	msg.clear();
	i = 0;
	n = 0;
	for ( ; i < 17; ++i) {
		int order = cards[i];
		if (order != 888) {
			++n;
			msg += card_name(order) + " ";
		}
	}
	if (role==1)
		msg = "\t农民[" + std::to_string(n) + "] \x1b[7m" + msg + "\x1b[0m";
	else
		msg = "\t农民[" + std::to_string(n) + "] " + msg;
	std::cout << msg << "\n";
	msg.clear();
	i = 0;
	n = 0;
	for ( ; i < 17; ++i) {
		int order = cards[i+17];
		if (order != 888) {
			++n;
			msg += card_name(order) + " ";
		}
	}
	if (role==2)
		msg = "\t农民[" + std::to_string(n) + "] \x1b[7m" + msg + "\x1b[0m";
	else
		msg = "\t农民[" + std::to_string(n) + "] " + msg;
	std::cout << msg << "\n";
}

static bool prompt(int *cards, int role, char *buf, int len)
{
	show(cards, role);
	const char *player = "地 主";
	if (role==1)
		player = "农民1";
	if (role==2)
		player = "农民2";
	std::cout << "\n\n\t" << player << "-> ";
	buf[0] = 0;
	if (std::fgets(buf, len, stdin) == NULL)
		if (std::feof(stdin)) {
			std::cout << "\033c\n";
			return false;
		}
	int slen = std::strlen(buf);
	if (slen >= 1 && (buf[slen-1] == '\n' || buf[slen-1] == '\r'))
		buf[slen-1] = 0;
	if (slen >= 2 && (buf[slen-2] == '\n' || buf[slen-2] == '\r'))
		buf[slen-2] = 0;

	return true;
}

