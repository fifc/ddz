#ifndef ADVISOR_H
#define ADVISOR_H

#include <string>
#include "ddz.h"

play advise(const int *cards, int count, const play& target);
play advise(const std::vector<card>& cards, const play& target);

#endif // ADVISOR_H
