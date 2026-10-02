#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "card.h"

static void show_players(Game *game)
{
    printf("\n================================\n");
    printf("            PLAYERS\n");
    printf("================================\n");

    for (int i = 0;
         i < game->num_players;
         i++) {

        printf(
            "%s - %d chips",
            game->players[i].name,
            game->players[i].chips
        );

        if (game->players[i].folded)
            printf(" [FOLDED]");

        printf("\n");
    }
}

static void show_hole_cards(Game *game)
{
    printf("\n========== HOLE CARDS ==========\n");

    for (int i = 0;
         i < game->num_players;
         i++) {

        printf(
            "%s: ",
            game->players[i].name
        );

        print_cards(
            game->players[i].hole_cards,
            2
        );
    }
}

static void show_community_cards(
    Game *game,
    int count
)
{
    printf("\n======= COMMUNITY CARDS =======\n");

    print_cards(
        game->community_cards,
        count
    );
}

static void reset_betting(Game *game)
{
    game->current_bet = 0;

    for (int i = 0;
         i < game->num_players;
         i++) {

        game->players[i].current_bet = 0;
    }
}

static void setup_blinds(Game *game)
{
    int small_blind =
        (game->dealer_position + 1)
        % game->num_players;

    int big_blind =
        (game->dealer_position + 2)
        % game->num_players;

    printf("\n========== BLINDS ==========\n");

    collect_bet(
        game,
        small_blind,
        SMALL_BLIND
    );

    collect_bet(
        game,
        big_blind,
        BIG_BLIND
    );

    game->current_bet =
        BIG_BLIND;

    printf(
        "%s -> Small Blind (%d)\n",
        game->players[small_blind].name,
        SMALL_BLIND
    );

    printf(
        "%s -> Big Blind (%d)\n",
        game->players[big_blind].name,
        BIG_BLIND
    );

    printf(
        "Pot: %d\n",
        game->pot
    );
}

static int play_hand(Game *game)
{
    start_hand(game);

    printf("\n\n================================\n");
    printf("             NEW HAND\n");
    printf("================================\n");

    show_hole_cards(game);

    /*
     * ==============================
     * PRE-FLOP
     * ==============================
     */

    setup_blinds(game);

    int big_blind =
        (game->dealer_position + 2)
        % game->num_players;

    int preflop_start =
        (big_blind + 1)
        % game->num_players;

    printf("\n========== PRE-FLOP ==========\n");

    betting_round(
        game,
        preflop_start
    );

    /*
     * Someone may have won because
     * everyone else folded.
     */

    if (count_active_players(game) == 1) {

        award_pot(game);

        rotate_dealer(game);

        return 1;
    }

    /*
     * ==============================
     * FLOP
     * ==============================
     */

    reset_betting(game);

    deal_flop(game);

    show_community_cards(
        game,
        3
    );

    printf("\n========== FLOP BETTING ==========\n");

    int postflop_start =
        (game->dealer_position + 1)
        % game->num_players;

    betting_round(
        game,
        postflop_start
    );

    if (count_active_players(game) == 1) {

        award_pot(game);

        rotate_dealer(game);

        return 1;
    }

    /*
     * ==============================
     * TURN
     * ==============================
     */

    reset_betting(game);

    deal_turn(game);

    show_community_cards(
        game,
        4
    );

    printf("\n========== TURN BETTING ==========\n");

    betting_round(
        game,
        postflop_start
    );

    if (count_active_players(game) == 1) {

        award_pot(game);

        rotate_dealer(game);

        return 1;
    }

    /*
     * ==============================
     * RIVER
     * ==============================
     */

    reset_betting(game);

    deal_river(game);

    show_community_cards(
        game,
        5
    );

    printf("\n========== RIVER BETTING ==========\n");

    betting_round(
        game,
        postflop_start
    );

    /*
     * ==============================
     * SHOWDOWN
     * ==============================
     */

    award_pot(game);

    rotate_dealer(game);

    return 1;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    Game game;

    int num_players;
    int choice;

    printf("====================================\n");
    printf("         POKER MASTER - V2\n");
    printf("         TEXAS HOLD'EM\n");
    printf("====================================\n\n");

    printf(
        "Enter number of players (2-%d): ",
        MAX_PLAYERS
    );

    scanf(
        "%d",
        &num_players
    );

    if (
        num_players < 2 ||
        num_players > MAX_PLAYERS
    ) {

        printf(
            "Invalid number of players.\n"
        );

        return 1;
    }

    initialize_game(
        &game,
        num_players
    );

    while (1) {

        printf("\n====================================\n");
        printf("              MAIN MENU\n");
        printf("====================================\n");

        printf("1. Play Hand\n");
        printf("2. Show Players\n");
        printf("3. Exit\n");

        printf("\nChoose: ");

        scanf(
            "%d",
            &choice
        );

        switch (choice) {

            case 1:

                play_hand(&game);

                break;

            case 2:

                show_players(&game);

                break;

            case 3:

                printf(
                    "\nThanks for playing!\n"
                );

                return 0;

            default:

                printf(
                    "Invalid choice.\n"
                );
        }
    }
}