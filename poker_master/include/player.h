#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include "poker.h"

#define STARTING_CHIPS 1000

typedef struct Player {
    char name[30];

    Card hole_cards[2];
    Card all_cards[7];

    int chips;

    int current_bet;

    int total_contribution;

    int folded;
    int all_in;

    int acted;

    HandRank hand_rank;
    int score[5];

} Player;

void initialize_player(Player *player, const char *name);

void reset_player_for_hand(Player *player);

int player_bet(Player *player, int amount);

void player_fold(Player *player);

#endif

