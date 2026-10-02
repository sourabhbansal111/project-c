#ifndef GAME_H
#define GAME_H

#include "deck.h"
#include "player.h"

#define MAX_PLAYERS 6

#define SMALL_BLIND 10
#define BIG_BLIND 20

typedef struct {

    Deck deck;

    Player players[MAX_PLAYERS];

    Card community_cards[5];

    int num_players;

    int dealer_position;

    int pot;

    int current_bet;

} Game;

void initialize_game(Game *game, int num_players);

void start_hand(Game *game);

void deal_hole_cards(Game *game);

void deal_flop(Game *game);

void deal_turn(Game *game);

void deal_river(Game *game);

void collect_bet(Game *game, int player_index, int amount);

void betting_round(Game *game, int start_position);

int count_active_players(Game *game);

int find_winner(Game *game);

void award_pot(Game *game);

void rotate_dealer(Game *game);

#endif