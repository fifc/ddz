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
	card cards1[] = {
		{5, heart}, {5, spade}, {5, diamond}, {5, club},
		{6, heart}, {6, spade}, {6, diamond}, {6, club},
		{7, heart}, {7, spade}, {7, diamond}, {7, club},
	};
	card cards2[] = {
		{6, heart}, {6, spade}, {6, diamond}, {6, club},
		{7, heart}, {7, spade}, {7, diamond}, {7, club},
		{8, heart}, {8, spade}, {8, diamond}, {8, club},
	};
	auto type = SortParsePlane(cards1, sizeof cards1 / sizeof (card));
	type = SortParsePlane(cards2, sizeof cards2 / sizeof (card));
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
	std::vector<card> vc1;
	vc1.assign(cards1, cards1 + sizeof(cards1)/sizeof(cards1[0]));
	std::vector<card> vc2;
	vc2.assign(cards2, cards2 + sizeof(cards2)/sizeof(cards2[0]));
	Game game;
	auto play1 = game.Parse(vc1);
	auto play2 = game.Parse(vc2);
	if (play1 > play2) {
		std::cout << " greater!\n";
	}
	if (play2 > play1) {
		std::cout << " less!\n";
	}
}

int testdeal()
{
        std::srand(std::time(0));
        int cards[54];
        int num[54];
        for (int i = 0; i < 54; ++i)
                num[i] = 0;
        for (int j = 0; j < 1000000; ++j) {
                deal_cards(cards);
                for (int i = 34; i < 54; ++i)
                        num[cards[i]]++;
        }
        long min = 100000000;
        long max = 0;
        for (int i = 0; i < 54; ++i) {
                if (num[i] > max)
                        max = num[i];
                if (num[i] < min)
                        min = num[i];
                std::cout << num[i] << " ";
        }
        std::cout << "\nmin: " << min << ", max: " << max << ", devi: " << (max - min)*1000000 / (max + min) *2 << "\n";
}

int main(int argc, char *argv[])
{
	std::srand(std::time(0));
	testplane();
}

