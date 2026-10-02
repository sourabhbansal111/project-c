#ifndef POKER_H
#define POKER_H

#include "card.h"

typedef enum {
    HIGH_CARD = 1,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH
} HandRank;

//  * Forward declaration.
//  * Player is actually defined in player.h

typedef struct Player Player;

void evaluate_hand(Player *player);
const char *hand_rank_name(HandRank rank);
int compare_players(Player *a, Player *b);

#endif