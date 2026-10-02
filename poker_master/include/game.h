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

    /*
     * Index of dealer/button.
     */
    int dealer_position;

    /*
     * Total chips currently in the pot.
     */
    int pot;

    /*
     * Highest contribution during
     * current betting round.
     */
    int current_bet;

    /*
     * Minimum amount by which a full
     * raise must increase the bet.
     */
    int last_raise_size;

} Game;

/* Game setup */
void initialize_game(Game *game, int num_players);

/* Hand management */
void start_hand(Game *game);

void deal_hole_cards(Game *game);

void deal_flop(Game *game);

void deal_turn(Game *game);

void deal_river(Game *game);

/* Betting */
void collect_bet(
    Game *game,
    int player_index,
    int amount
);

void betting_round(
    Game *game,
    int start_position
);

/* Player state */
int count_active_players(Game *game);

int count_players_with_chips(Game *game);

/* Winner / pots */
void award_pots(Game *game);

int find_last_active_player(Game *game);

/* Dealer */
void rotate_dealer(Game *game);

#endif