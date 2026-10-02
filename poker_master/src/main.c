#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "card.h"

/* =========================================================
   DISPLAY
   ========================================================= */

static void show_players(Game *game)
{
    printf("\n====================================\n");
    printf("             PLAYERS\n");
    printf("====================================\n");

    for (int i = 0;
         i < game->num_players;
         i++) {

        printf(
            "%d. %s - %d chips",
            i + 1,
            game->players[i].name,
            game->players[i].chips
        );

        if (game->players[i].chips == 0)
            printf(" [OUT]");

        printf("\n");
    }
}

static void show_hole_cards(Game *game)
{
    printf("\n========== HOLE CARDS ==========\n");

    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        if (player->folded)
            continue;

        printf(
            "%s: ",
            player->name
        );

        print_cards(
            player->hole_cards,
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

/* =========================================================
   INPUT PLAYER NAMES
   ========================================================= */

static void setup_player_names(Game *game)
{
    char input[100];

    printf("\n========== PLAYER SETUP ==========\n");
    getchar(); // Clear newline from previous input
    for (int i = 0;
         i < game->num_players;
         i++) {

        printf(
            "Name for Player %d "
            "(press Enter for Player %d): ",
            i + 1,
            i + 1
        );


        if (fgets(
                input,
                sizeof(input),
                stdin
            ) == NULL) {

            continue;
        }

        /*
         * Remove newline.
         */
        input[
            strcspn(input, "\n")
        ] = '\0';

        if (strlen(input) == 0) {

            snprintf(
                game->players[i].name,
                sizeof(game->players[i].name),
                "Player %d",
                i + 1
            );

        } else {

            strncpy(
                game->players[i].name,
                input,
                sizeof(game->players[i].name) - 1
            );

            game->players[i].name[
                sizeof(game->players[i].name) - 1
            ] = '\0';
        }
    }
}

/* =========================================================
   RESET CURRENT BETTING ROUND
   ========================================================= */

static void reset_betting_round(Game *game)
{
    game->current_bet = 0;

    game->last_raise_size =
        BIG_BLIND;

    for (int i = 0;
         i < game->num_players;
         i++) {

        game->players[i].current_bet = 0;
        game->players[i].acted = 0;
    }
}

/* =========================================================
   SETUP BLINDS
   ========================================================= */

static void setup_blinds(Game *game)
{
    int small_blind;
    int big_blind;

    /*
     * Heads-up:
     *
     * Dealer = Small Blind
     * Other player = Big Blind
     */
    if (game->num_players == 2) {

        small_blind =
            game->dealer_position;

        big_blind =
            (game->dealer_position + 1)
            % game->num_players;

    } else {

        small_blind =
            (game->dealer_position + 1)
            % game->num_players;

        big_blind =
            (game->dealer_position + 2)
            % game->num_players;
    }

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

    game->last_raise_size =
        BIG_BLIND;

    printf("\n========== BLINDS ==========\n");

    printf(
        "%s -> Small Blind: %d\n",
        game->players[small_blind].name,
        SMALL_BLIND
    );

    printf(
        "%s -> Big Blind: %d\n",
        game->players[big_blind].name,
        BIG_BLIND
    );

    printf(
        "Pot: %d\n",
        game->pot
    );
}

/* =========================================================
   BETTING START POSITIONS
   ========================================================= */

static int preflop_start(Game *game)
{
    /*
     * Heads-up:
     *
     * Dealer/SB acts first pre-flop.
     */
    if (game->num_players == 2) {

        return game->dealer_position;
    }

    int big_blind =
        (game->dealer_position + 2)
        % game->num_players;

    return (big_blind + 1)
           % game->num_players;
}

static int postflop_start(Game *game)
{
    /*
     * Heads-up:
     *
     * Big Blind acts first after flop.
     */
    if (game->num_players == 2) {

        return (game->dealer_position + 1)
               % game->num_players;
    }

    return (game->dealer_position + 1)
           % game->num_players;
}

/* =========================================================
   PLAY ONE HAND
   ========================================================= */

static void play_hand(Game *game)
{
    start_hand(game);

    printf("\n\n====================================\n");
    printf("              NEW HAND\n");
    printf("====================================\n");

    printf(
        "Dealer: %s\n",
        game->players[
            game->dealer_position
        ].name
    );

    show_hole_cards(game);

    /* -----------------------------------------------------
       PRE-FLOP
       ----------------------------------------------------- */

    setup_blinds(game);

    printf("\n========== PRE-FLOP ==========\n");

    betting_round(
        game,
        preflop_start(game)
    );

    /*
     * Only one player remains.
     */
    if (count_active_players(game) <= 1) {

        award_pots(game);

        rotate_dealer(game);

        return;
    }

    /*
     * -----------------------------------------------------
       FLOP
       -----------------------------------------------------
    */

    reset_betting_round(game);

    deal_flop(game);

    show_community_cards(
        game,
        3
    );

    printf("\n========== FLOP BETTING ==========\n");

    betting_round(
        game,
        postflop_start(game)
    );

    if (count_active_players(game) <= 1) {

        award_pots(game);

        rotate_dealer(game);

        return;
    }

    /*
     * -----------------------------------------------------
       TURN
       -----------------------------------------------------
    */

    reset_betting_round(game);

    deal_turn(game);

    show_community_cards(
        game,
        4
    );

    printf("\n========== TURN BETTING ==========\n");

    betting_round(
        game,
        postflop_start(game)
    );

    if (count_active_players(game) <= 1) {

        award_pots(game);

        rotate_dealer(game);

        return;
    }

    /*
     * -----------------------------------------------------
       RIVER
       -----------------------------------------------------
    */

    reset_betting_round(game);

    deal_river(game);

    show_community_cards(
        game,
        5
    );

    printf("\n========== RIVER BETTING ==========\n");

    betting_round(
        game,
        postflop_start(game)
    );

    /*
     * -----------------------------------------------------
       SHOWDOWN
       -----------------------------------------------------
    */

    if (count_active_players(game) > 1) {

        award_pots(game);

    } else {

        award_pots(game);
    }

    /*
     * Move dealer button.
     */
    rotate_dealer(game);
}

/* =========================================================
   REMOVE BUSTED PLAYERS
   ========================================================= */

static void check_eliminated_players(Game *game)
{
    for (int i = 0;
         i < game->num_players;
         i++) {

        if (game->players[i].chips == 0) {

            printf(
                "\n%s has been eliminated.\n",
                game->players[i].name
            );
        }
    }
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    srand(
        (unsigned int)time(NULL)
    );

    Game game;

    int num_players;

    printf("====================================\n");
    printf("          POKER MASTER V3\n");
    printf("          TEXAS HOLD'EM\n");
    printf("====================================\n");

    printf(
        "\nNumber of players (2-%d): ",
        MAX_PLAYERS
    );

    if (scanf(
            "%d",
            &num_players
        ) != 1) {

        printf("Invalid input.\n");

        return 1;
    }

    if (
        num_players < 2 ||
        num_players > MAX_PLAYERS
    ) {

        printf(
            "Players must be between 2 and %d.\n",
            MAX_PLAYERS
        );

        return 1;
    }

    initialize_game(
        &game,
        num_players
    );

    setup_player_names(&game);

    while (1) {

        /*
         * Check whether tournament/game
         * is finished.
         */
        if (count_players_with_chips(&game) <= 1) {

            printf(
                "\n====================================\n"
            );

            printf(
                "           GAME OVER\n"
            );

            for (int i = 0;
                 i < game.num_players;
                 i++) {

                if (game.players[i].chips > 0) {

                    printf(
                        "Winner: %s\n",
                        game.players[i].name
                    );

                    printf(
                        "Chips: %d\n",
                        game.players[i].chips
                    );
                }
            }

            break;
        }

        printf(
            "\n====================================\n"
        );

        printf(
            "              MAIN MENU\n"
        );

        printf(
            "====================================\n"
        );

        printf("1. Play Hand\n");
        printf("2. Show Players\n");
        printf("3. Exit\n");

        printf("\nChoose: ");

        int choice;

        if (scanf(
                "%d",
                &choice
            ) != 1) {

            /*
             * Clear invalid input.
             */
            int c;

            while (
                (c = getchar()) != '\n' &&
                c != EOF
            ) {
            }

            printf(
                "Invalid input.\n"
            );

            continue;
        }

        switch (choice) {

            case 1:

                play_hand(&game);

                check_eliminated_players(
                    &game
                );

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

    return 0;
}