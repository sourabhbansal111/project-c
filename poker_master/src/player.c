#include <string.h>

#include "player.h"

void initialize_player(Player *player, const char *name)
{
    strncpy(player->name, name, sizeof(player->name) - 1);

    player->name[sizeof(player->name) - 1] = '\0';

    player->chips = STARTING_CHIPS;
    player->current_bet = 0;
    player->folded = 0;
}

void reset_player_for_round(Player *player)
{
    player->current_bet = 0;
    player->folded = 0;
}

int player_bet(Player *player, int amount)
{
    if (amount <= 0)
        return 0;

    if (amount > player->chips)
        return 0;

    player->chips -= amount;
    player->current_bet += amount;

    return 1;
}

void player_fold(Player *player)
{
    player->folded = 1;
}