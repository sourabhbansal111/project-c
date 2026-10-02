#include <stdio.h>

#include "game.h"

void initialize_game(Game *game, int num_players)
{
    game->num_players = num_players;

    game->dealer_position = 0;

    game->pot = 0;
    game->current_bet = 0;

    for (int i = 0; i < num_players; i++) {

        char name[30];

        snprintf(
            name,
            sizeof(name),
            "Player %d",
            i + 1
        );

        initialize_player(
            &game->players[i],
            name
        );
    }
}

void start_hand(Game *game)
{
    initialize_deck(&game->deck);
    shuffle_deck(&game->deck);

    game->pot = 0;
    game->current_bet = 0;

    for (int i = 0; i < game->num_players; i++) {

        reset_player_for_round(
            &game->players[i]
        );
    }

    deal_hole_cards(game);
}

void deal_hole_cards(Game *game)
{
    for (int round = 0; round < 2; round++) {

        for (int i = 0;
             i < game->num_players;
             i++) {

            game->players[i].hole_cards[round] =
                deal_card(&game->deck);
        }
    }
}

void deal_flop(Game *game)
{
    /*
     * Burn one card.
     */
    deal_card(&game->deck);

    for (int i = 0; i < 3; i++) {

        game->community_cards[i] =
            deal_card(&game->deck);
    }
}

void deal_turn(Game *game)
{
    /*
     * Burn one card.
     */
    deal_card(&game->deck);

    game->community_cards[3] =
        deal_card(&game->deck);
}

void deal_river(Game *game)
{
    /*
     * Burn one card.
     */
    deal_card(&game->deck);

    game->community_cards[4] =
        deal_card(&game->deck);
}

void collect_bet(
    Game *game,
    int player_index,
    int amount
)
{
    if (player_bet(
            &game->players[player_index],
            amount)) {

        game->pot += amount;
    }
}

int count_active_players(Game *game)
{
    int count = 0;

    for (int i = 0;
         i < game->num_players;
         i++) {

        if (!game->players[i].folded)
            count++;
    }

    return count;
}

static int next_player(
    Game *game,
    int position
)
{
    return (position + 1) %
           game->num_players;
}

static int everyone_matched(Game *game)
{
    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        if (player->folded)
            continue;

        if (player->current_bet !=
            game->current_bet) {

            return 0;
        }
    }

    return 1;
}

static void player_action(
    Game *game,
    int position
)
{
    Player *player =
        &game->players[position];

    int amount_to_call =
        game->current_bet -
        player->current_bet;

    printf("\n--------------------------------\n");

    printf("%s's turn\n", player->name);

    printf("Chips: %d\n", player->chips);

    printf(
        "Current bet: %d\n",
        game->current_bet
    );

    printf(
        "Your bet: %d\n",
        player->current_bet
    );

    printf(
        "Amount to call: %d\n",
        amount_to_call
    );

    printf("--------------------------------\n");

    if (amount_to_call == 0) {

        printf("1. Check\n");
        printf("2. Raise\n");
        printf("3. Fold\n");

    } else {

        printf("1. Call\n");
        printf("2. Raise\n");
        printf("3. Fold\n");
    }

    int choice;

    printf("Choose: ");
    scanf("%d", &choice);

    /*
     * CHECK / CALL
     */

    if (choice == 1) {

        if (amount_to_call == 0) {

            printf(
                "%s checks.\n",
                player->name
            );

        } else {

            if (!player_bet(
                    player,
                    amount_to_call)) {

                printf(
                    "You don't have enough chips.\n"
                );

                return;
            }

            game->pot += amount_to_call;

            printf(
                "%s calls %d.\n",
                player->name,
                amount_to_call
            );
        }

        return;
    }

    /*
     * RAISE
     */

    if (choice == 2) {

        int raise_to;

        printf(
            "Enter total bet amount: "
        );

        scanf("%d", &raise_to);

        if (raise_to <= game->current_bet) {

            printf(
                "Raise must be greater than %d.\n",
                game->current_bet
            );

            return;
        }

        int amount =
            raise_to -
            player->current_bet;

        if (!player_bet(player, amount)) {

            printf(
                "You don't have enough chips.\n"
            );

            return;
        }

        game->pot += amount;

        game->current_bet = raise_to;

        printf(
            "%s raises to %d.\n",
            player->name,
            raise_to
        );

        return;
    }

    /*
     * FOLD
     */

    if (choice == 3) {

        player_fold(player);

        printf(
            "%s folds.\n",
            player->name
        );

        return;
    }

    printf("Invalid choice.\n");
}

void betting_round(
    Game *game,
    int start_position
)
{
    int active_players = count_active_players(game);
    int position = start_position;
    int players_acted = 0;

    while (1) {

        /*
         * If only one player remains,
         * betting ends.
         */

        if (count_active_players(game) <= 1)
            return;

        /*
         * If everyone has matched the
         * current bet, betting ends.
         */

        if (everyone_matched(game) && 
                players_acted >= active_players)
            return;

        /*
         * Skip folded players.
         */

        if (game->players[position].folded) {

            position =
                next_player(game, position);

            continue;
        }

        int old_bet =
            game->current_bet;

        player_action(
            game,
            position
        );

        players_acted++;

        /*
         * If player raised, continue from
         * the next player.
         *
         * Because current_bet changed,
         * everyone else will need to match it.
         */

        // if (game->current_bet > old_bet) {

        //     position =
        //         next_player(game, position);

        //     continue;
        // }

        /*
         * Normal call/check/fold.
         */

        position =
            next_player(game, position);
    }
}

int find_winner(Game *game)
{
    int winner = -1;

    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        if (player->folded)
            continue;

        /*
         * Create the player's
         * seven-card hand.
         */

        player->all_cards[0] =
            player->hole_cards[0];

        player->all_cards[1] =
            player->hole_cards[1];

        for (int j = 0; j < 5; j++) {

            player->all_cards[j + 2] =
                game->community_cards[j];
        }

        evaluate_hand(player);

        if (winner == -1) {

            winner = i;

        } else {

            if (compare_players(
                    player,
                    &game->players[winner]
                ) > 0) {

                winner = i;
            }
        }
    }

    return winner;
}

void award_pot(Game *game)
{
    int winner = find_winner(game);

    if (winner == -1)
        return;

    printf("\n================================\n");
    printf("            SHOWDOWN\n");
    printf("================================\n");

    for (int i = 0;
         i < game->num_players;
         i++) {

        if (game->players[i].folded)
            continue;

        printf(
            "%s: ",
            game->players[i].name
        );

        print_cards(
            game->players[i].hole_cards,
            2
        );

        printf(
            "Hand: %s\n",
            hand_rank_name(
                game->players[i].hand_rank
            )
        );
    }

    printf("\nWinner: %s\n",
           game->players[winner].name);

    printf(
        "Winning hand: %s\n",
        hand_rank_name(
            game->players[winner].hand_rank
        )
    );

    printf(
        "Pot: %d chips\n",
        game->pot
    );

    game->players[winner].chips +=
        game->pot;

    game->pot = 0;

    printf(
        "%s now has %d chips.\n",
        game->players[winner].name,
        game->players[winner].chips
    );
}

void rotate_dealer(Game *game)
{
    game->dealer_position =
        next_player(
            game,
            game->dealer_position
        );
}