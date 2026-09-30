#ifndef POKER_H
#define POKER_H

#include "card.h"

#define MAX_PLAYERS 6

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

typedef struct {
    char name[30];
    Card hole_cards[2];
    Card all_cards[7];

    HandRank hand_rank;
    int score[5];
} Player;

void evaluate_hand(Player *player);
const char *hand_rank_name(HandRank rank);
int compare_players(Player *a, Player *b);

#endif