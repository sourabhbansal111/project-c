#include <stdio.h>
#include <stdlib.h>

#include "game.h"

/* =========================================================
   BASIC HELPERS
   ========================================================= */

static int next_position(Game *game, int position)
{
    return (position + 1) % game->num_players;
}

int count_active_players(Game *game)
{
    int count = 0;

    for (int i = 0; i < game->num_players; i++) {

        if (!game->players[i].folded)
            count++;
    }

    return count;
}

int count_players_with_chips(Game *game)
{
    int count = 0;

    for (int i = 0; i < game->num_players; i++) {

        if (game->players[i].chips > 0)
            count++;
    }

    return count;
}

int find_last_active_player(Game *game)
{
    for (int i = 0; i < game->num_players; i++) {

        if (!game->players[i].folded)
            return i;
    }

    return -1;
}

/* =========================================================
   GAME INITIALIZATION
   ========================================================= */

void initialize_game(Game *game, int num_players)
{
    game->num_players = num_players;

    game->dealer_position = 0;

    game->pot = 0;
    game->current_bet = 0;
    game->last_raise_size = BIG_BLIND;

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

/* =========================================================
   START HAND
   ========================================================= */

void start_hand(Game *game)
{
    initialize_deck(&game->deck);

    shuffle_deck(&game->deck);

    game->pot = 0;
    game->current_bet = 0;
    game->last_raise_size = BIG_BLIND;

    for (int i = 0; i < game->num_players; i++) {

        reset_player_for_hand(
            &game->players[i]
        );
    }

    deal_hole_cards(game);
}

/* =========================================================
   DEALING
   ========================================================= */

void deal_hole_cards(Game *game)
{
    for (int round = 0; round < 2; round++) {

        for (int i = 0;
             i < game->num_players;
             i++) {

            /*
             * Players with zero chips should
             * already have been removed from
             * the active game.
             */
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

/* =========================================================
   CHIP / BET HANDLING
   ========================================================= */

void collect_bet(
    Game *game,
    int player_index,
    int amount
)
{
    if (amount <= 0)
        return;

    Player *player =
        &game->players[player_index];

    /*
     * If player doesn't have enough chips,
     * they simply go all-in with whatever
     * they have.
     */
    if (amount > player->chips)
        amount = player->chips;

    if (amount <= 0)
        return;

    player_bet(player, amount);

    game->pot += amount;
}

/* =========================================================
   BETTING HELPERS
   ========================================================= */

static int everyone_finished(Game *game)
{
    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        if (player->folded)
            continue;

        /*
         * All-in players don't need to act.
         */
        if (player->all_in)
            continue;

        /*
         * Player still needs to act.
         */
        if (!player->acted)
            return 0;

        /*
         * Player hasn't matched the bet.
         */
        if (player->current_bet !=
            game->current_bet) {

            return 0;
        }
    }

    return 1;
}

static int find_next_player(
    Game *game,
    int position
)
{
    int next =
        next_position(game, position);

    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[next];

        if (!player->folded &&
            !player->all_in) {

            return next;
        }

        next =
            next_position(game, next);
    }

    return -1;
}

static void reset_acted_players(Game *game)
{
    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        if (!player->folded &&
            !player->all_in) {

            player->acted = 0;
        }
    }
}

/* =========================================================
   PLAYER ACTION
   ========================================================= */

static int player_action(
    Game *game,
    int position
)
{
    Player *player =
        &game->players[position];

    int to_call =
        game->current_bet -
        player->current_bet;

    printf("\n================================\n");

    printf(
        "%s's turn\n",
        player->name
    );

    printf(
        "Your chips: %d\n",
        player->chips
    );

    printf(
        "Current bet: %d\n",
        game->current_bet
    );

    printf(
        "Your current bet: %d\n",
        player->current_bet
    );

    printf(
        "To call: %d\n",
        to_call
    );

    printf("================================\n");

    /*
     * Check / Call
     */
    if (to_call == 0) {

        printf("1. Check\n");

    } else {

        printf("1. Call (%d)\n", to_call);
    }

    printf("2. Raise\n");
    printf("3. All-in\n");
    printf("4. Fold\n");

    int choice;

    printf("Choose: ");
    scanf("%d", &choice);

    /* -----------------------------------------------------
       CHECK / CALL
       ----------------------------------------------------- */

    if (choice == 1) {

        if (to_call == 0) {

            printf(
                "%s checks.\n",
                player->name
            );

        } else {

            /*
             * If the player doesn't have enough
             * chips to call, they automatically
             * go all-in.
             */
            if (to_call >= player->chips) {

                int amount = player->chips;

                player_bet(
                    player,
                    amount
                );

                game->pot += amount;

                printf(
                    "%s calls all-in for %d.\n",
                    player->name,
                    amount
                );

            } else {

                player_bet(
                    player,
                    to_call
                );

                game->pot += to_call;

                printf(
                    "%s calls %d.\n",
                    player->name,
                    to_call
                );
            }
        }

        player->acted = 1;

        return 1;
    }

    /* -----------------------------------------------------
       RAISE
       ----------------------------------------------------- */

    if (choice == 2) {

        int raise_to;

        printf(
            "Enter your TOTAL bet amount: "
        );

        scanf("%d", &raise_to);

        /*
         * Minimum full raise:
         *
         * current bet + last raise size
         */
        int minimum_raise =
            game->current_bet +
            game->last_raise_size;

        /*
         * Player cannot raise beyond
         * what they can afford.
         */
        int maximum_bet =
            player->current_bet +
            player->chips;

        /*
         * If the player wants to put all
         * their remaining chips in, it is
         * handled as all-in below.
         */
        if (raise_to >= maximum_bet) {

            int amount =
                maximum_bet -
                player->current_bet;

            if (amount <= 0) {

                printf(
                    "You cannot raise.\n"
                );

                return 0;
            }

            player_bet(
                player,
                amount
            );

            game->pot += amount;

            /*
             * If all-in amount is below the
             * minimum raise, it does NOT
             * count as a full raise.
             */
            if (maximum_bet >
                game->current_bet) {

                int increase =
                    maximum_bet -
                    game->current_bet;

                if (increase >=
                    game->last_raise_size) {

                    game->last_raise_size =
                        increase;

                    game->current_bet =
                        maximum_bet;

                    reset_acted_players(game);

                } else {

                    game->current_bet =
                        maximum_bet;
                }
            }

            player->acted = 1;

            printf(
                "%s goes all-in for %d.\n",
                player->name,
                maximum_bet
            );

            return 1;
        }

        /*
         * Normal raise must be at least
         * current bet + minimum raise.
         */
        if (raise_to < minimum_raise) {

            printf(
                "Minimum raise is to %d.\n",
                minimum_raise
            );

            return 0;
        }

        if (raise_to <=
            player->current_bet) {

            printf(
                "Invalid raise.\n"
            );

            return 0;
        }

        int amount =
            raise_to -
            player->current_bet;

        if (amount > player->chips) {

            printf(
                "You don't have enough chips.\n"
            );

            return 0;
        }

        int raise_size =
            raise_to -
            game->current_bet;

        player_bet(
            player,
            amount
        );

        game->pot += amount;

        game->current_bet =
            raise_to;

        game->last_raise_size =
            raise_size;

        /*
         * A full raise reopens betting.
         */
        reset_acted_players(game);

        player->acted = 1;

        printf(
            "%s raises to %d.\n",
            player->name,
            raise_to
        );

        return 1;
    }

    /* -----------------------------------------------------
       ALL-IN
       ----------------------------------------------------- */

    if (choice == 3) {

        if (player->chips <= 0) {

            printf(
                "%s is already all-in.\n",
                player->name
            );

            player->all_in = 1;

            return 1;
        }

        int old_bet =
            player->current_bet;

        int new_bet =
            player->current_bet +
            player->chips;

        int amount =
            player->chips;

        player_bet(
            player,
            amount
        );

        game->pot += amount;

        printf(
            "%s goes all-in for %d total.\n",
            player->name,
            new_bet
        );

        /*
         * If this increases the highest bet,
         * determine whether it is a full raise.
         */
        if (new_bet >
            game->current_bet) {

            int increase =
                new_bet -
                game->current_bet;

            game->current_bet =
                new_bet;

            if (increase >=
                game->last_raise_size) {

                game->last_raise_size =
                    increase;

                /*
                 * Full raise reopens betting.
                 */
                reset_acted_players(game);
            }
        }

        (void)old_bet;

        player->acted = 1;

        return 1;
    }

    /* -----------------------------------------------------
       FOLD
       ----------------------------------------------------- */

    if (choice == 4) {

        player_fold(player);

        printf(
            "%s folds.\n",
            player->name
        );

        return 1;
    }

    printf(
        "Invalid choice.\n"
    );

    return 0;
}

/* =========================================================
   BETTING ROUND
   ========================================================= */

void betting_round(
    Game *game,
    int start_position
)
{
    /*
     * At the beginning of a new betting
     * round, nobody has acted yet.
     */
    for (int i = 0;
         i < game->num_players;
         i++) {

        if (!game->players[i].folded &&
            !game->players[i].all_in) {

            game->players[i].acted = 0;
        }
    }

    int position =
        start_position;

    while (1) {

        /*
         * Only one player remains.
         */
        if (count_active_players(game) <= 1)
            return;

        /*
         * Everyone has either acted and
         * matched the bet or is all-in.
         */
        if (everyone_finished(game))
            return;

        /*
         * Find someone who can act.
         */
        position =
            find_next_player(
                game,
                position - 1
            );

        if (position == -1)
            return;

        /*
         * Keep asking until a valid action
         * is entered.
         */
        while (!player_action(
                    game,
                    position)) {
        }

        /*
         * Check if only one player remains.
         */
        if (count_active_players(game) <= 1)
            return;

        position =
            next_position(
                game,
                position
            );
    }
}

/* =========================================================
   SIDE POT STRUCTURE
   ========================================================= */

typedef struct {

    int amount;

    int eligible[MAX_PLAYERS];

    int eligible_count;

} SidePot;

/* =========================================================
   FIND WINNER FOR ONE POT
   ========================================================= */

static int find_pot_winner(
    Game *game,
    SidePot *pot
)
{
    int winner = -1;

    for (int i = 0;
         i < pot->eligible_count;
         i++) {

        int index =
            pot->eligible[i];

        Player *player =
            &game->players[index];

        /*
         * Player folded, so they cannot
         * win a pot.
         */
        if (player->folded)
            continue;

        if (winner == -1) {

            winner = index;

        } else {

            int result =
                compare_players(
                    player,
                    &game->players[winner]
                );

            if (result > 0) {

                winner = index;
            }
        }
    }

    return winner;
}

/* =========================================================
   DISTRIBUTE ONE POT
   ========================================================= */

static void distribute_pot(
    Game *game,
    SidePot *pot
)
{
    if (pot->amount <= 0)
        return;

    int winner =
        find_pot_winner(
            game,
            pot
        );

    if (winner == -1)
        return;

    /*
     * First determine all players tied
     * with the winner.
     */
    int tied[MAX_PLAYERS];
    int tied_count = 0;

    for (int i = 0;
         i < pot->eligible_count;
         i++) {

        int index =
            pot->eligible[i];

        Player *player =
            &game->players[index];

        if (player->folded)
            continue;

        if (compare_players(
                player,
                &game->players[winner]
            ) == 0) {

            tied[tied_count++] =
                index;
        }
    }

    if (tied_count == 0)
        return;

    int share =
        pot->amount /
        tied_count;

    int remainder =
        pot->amount %
        tied_count;

    for (int i = 0;
         i < tied_count;
         i++) {

        game->players[tied[i]].chips +=
            share;
    }

    /*
     * Odd chips are distributed one at
     * a time starting from the player
     * immediately after the dealer.
     */
    int position =
        next_position(
            game,
            game->dealer_position
        );

    while (remainder > 0) {

        for (int i = 0;
             i < game->num_players;
             i++) {

            int found = 0;

            for (int j = 0;
                 j < tied_count;
                 j++) {

                if (tied[j] == position) {

                    game->players[position].chips++;

                    remainder--;

                    found = 1;

                    break;
                }
            }

            if (remainder <= 0)
                break;

            (void)found;

            position =
                next_position(
                    game,
                    position
                );
        }
    }
}

/* =========================================================
   CREATE AND DISTRIBUTE SIDE POTS
   ========================================================= */

void award_pots(Game *game)
{
    /*
     * If only one player remains because
     * everyone folded, they get the entire
     * pot without showdown.
     */
    if (count_active_players(game) == 1) {

        int winner =
            find_last_active_player(game);

        printf(
            "\n%s wins %d chips.\n",
            game->players[winner].name,
            game->pot
        );

        game->players[winner].chips +=
            game->pot;

        game->pot = 0;

        return;
    }

    /*
     * Build the seven-card hand for every
     * player who has not folded.
     */
    for (int i = 0;
         i < game->num_players;
         i++) {

        Player *player =
            &game->players[i];

        player->all_cards[0] =
            player->hole_cards[0];

        player->all_cards[1] =
            player->hole_cards[1];

        for (int j = 0; j < 5; j++) {

            player->all_cards[j + 2] =
                game->community_cards[j];
        }

        /*
         * Even folded players don't need
         * their hand evaluated.
         */
        if (!player->folded)
            evaluate_hand(player);
    }

    /*
     * Collect all unique contribution
     * levels.
     *
     * Example:
     *
     * 40
     * 100
     * 250
     *
     * These create:
     *
     * Main pot: 40
     * Side pot: 60
     * Side pot: 150
     */
    int levels[MAX_PLAYERS];
    int level_count = 0;

    for (int i = 0;
         i < game->num_players;
         i++) {

        int contribution =
            game->players[i].total_contribution;

        if (contribution <= 0)
            continue;

        int exists = 0;

        for (int j = 0;
             j < level_count;
             j++) {

            if (levels[j] ==
                contribution) {

                exists = 1;
                break;
            }
        }

        if (!exists) {

            levels[level_count++] =
                contribution;
        }
    }

    /*
     * Sort contribution levels.
     */
    for (int i = 0;
         i < level_count - 1;
         i++) {

        for (int j = i + 1;
             j < level_count;
             j++) {

            if (levels[j] < levels[i]) {

                int temp =
                    levels[i];

                levels[i] =
                    levels[j];

                levels[j] =
                    temp;
            }
        }
    }

    int previous_level = 0;

    printf(
        "\n================================\n"
    );

    printf(
        "             SHOWDOWN\n"
    );

    printf(
        "================================\n"
    );

    /*
     * Create each pot.
     */
    for (int level_index = 0;
         level_index < level_count;
         level_index++) {

        int level =
            levels[level_index];

        int difference =
            level -
            previous_level;

        if (difference <= 0)
            continue;

        SidePot pot;

        pot.amount = 0;
        pot.eligible_count = 0;

        /*
         * Every player who contributed
         * at least this level contributes
         * difference *chips* to this pot.
         */
        for (int i = 0;
             i < game->num_players;
             i++) {

            Player *player =
                &game->players[i];

            if (player->total_contribution
                >= level) {

                pot.amount += difference;

                /*
                 * Folded players contribute
                 * money but cannot win.
                 */
                if (!player->folded) {

                    pot.eligible[
                        pot.eligible_count++
                    ] = i;
                }
            }
        }

        if (pot.amount > 0 &&
            pot.eligible_count > 0) {

            printf(
                "Pot: %d chips\n",
                pot.amount
            );

            /*
             * Show eligible players.
             */
            printf("Eligible: ");

            for (int i = 0;
                 i < pot.eligible_count;
                 i++) {

                printf(
                    "%s",
                    game->players[
                        pot.eligible[i]
                    ].name
                );

                if (i <
                    pot.eligible_count - 1) {

                    printf(", ");
                }
            }

            printf("\n");

            int winner =
                find_pot_winner(
                    game,
                    &pot
                );

            if (winner != -1) {

                printf(
                    "Winner: %s\n",
                    game->players[winner].name
                );

                printf(
                    "Hand: %s\n",
                    hand_rank_name(
                        game->players[winner].hand_rank
                    )
                );
            }

            distribute_pot(
                game,
                &pot
            );
        }

        previous_level = level;
    }

    game->pot = 0;

    /*
     * Show remaining stacks.
     */
    printf(
        "\n========== CHIP COUNTS ==========\n"
    );

    for (int i = 0;
         i < game->num_players;
         i++) {

        printf(
            "%s: %d chips\n",
            game->players[i].name,
            game->players[i].chips
        );
    }
}

/* =========================================================
   DEALER ROTATION
   ========================================================= */

void rotate_dealer(Game *game)
{
    /*
     * Find the next player who still
     * has chips.
     */
    int next =
        next_position(
            game,
            game->dealer_position
        );

    for (int i = 0;
         i < game->num_players;
         i++) {

        if (game->players[next].chips > 0) {

            game->dealer_position =
                next;

            return;
        }

        next =
            next_position(
                game,
                next
            );
    }
}