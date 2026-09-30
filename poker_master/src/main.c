#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "card.h"
#include "deck.h"
#include "poker.h"

#define MAX_PLAYERS 6

int main(void)
{
    srand((unsigned int)time(NULL));

    int num_players;

    printf("====================================\n");
    printf("       POKER MASTER - V1\n");
    printf("       Texas Hold'em Engine\n");
    printf("====================================\n\n");

    printf("Enter number of players (2-%d): ", MAX_PLAYERS);

    scanf("%d", &num_players);

    if (num_players < 2 || num_players > MAX_PLAYERS) {

        printf("Invalid number of players.\n");

        return 1;
    }

    Deck deck;

    initialize_deck(&deck);
    shuffle_deck(&deck);

    Player players[MAX_PLAYERS];

    /*
     * Player names
     */

    for (int i = 0; i < num_players; i++) {

        snprintf(
            players[i].name,
            sizeof(players[i].name),
            "Player %d",
            i + 1
        );
    }

    /*
     * Deal hole cards
     */

    for (int round = 0; round < 2; round++) {

        for (int i = 0; i < num_players; i++) {

            players[i].hole_cards[round] =
                deal_card(&deck);
        }
    }

    /*
     * Community cards
     */

    Card community[5];

    for (int i = 0; i < 5; i++) {

        community[i] = deal_card(&deck);
    }

    /*
     * Display hole cards
     */

    printf("\n========== HOLE CARDS ==========\n");

    for (int i = 0; i < num_players; i++) {

        printf("%s: ", players[i].name);

        print_cards(players[i].hole_cards, 2);
    }

    /*
     * Display community cards
     */

    printf("\n======= COMMUNITY CARDS ========\n");

    print_cards(community, 5);

    /*
     * Build 7-card hand
     */

    for (int i = 0; i < num_players; i++) {

        players[i].all_cards[0] =
            players[i].hole_cards[0];

        players[i].all_cards[1] =
            players[i].hole_cards[1];

        for (int j = 0; j < 5; j++) {

            players[i].all_cards[j + 2] =
                community[j];
        }

        evaluate_hand(&players[i]);
    }

    /*
     * Display results
     */

    printf("\n========== RESULTS =============\n");

    for (int i = 0; i < num_players; i++) {

        printf(
            "%s -> %s\n",
            players[i].name,
            hand_rank_name(players[i].hand_rank)
        );
    }

    /*
     * Find winner
     */

    int winner = 0;

    for (int i = 1; i < num_players; i++) {

        int result =
            compare_players(
                &players[i],
                &players[winner]
            );

        if (result > 0) {
            winner = i;
        }
    }

    /*
     * Check for tie
     */

    int tie = 0;

    for (int i = 0; i < num_players; i++) {

        if (i != winner &&
            compare_players(
                &players[i],
                &players[winner]
            ) == 0) {

            tie = 1;
        }
    }

    printf("\n====================================\n");

    if (tie) {

        printf("Result: Tie!\n");

    } else {

        printf(
            "Winner: %s\n",
            players[winner].name
        );

        printf(
            "Hand: %s\n",
            hand_rank_name(
                players[winner].hand_rank
            )
        );
    }

    printf("====================================\n");

    return 0;
}