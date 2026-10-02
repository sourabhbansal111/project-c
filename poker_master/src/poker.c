#include <stdio.h>
#include <string.h>
#include "poker.h"
#include "player.h"

static void sort_desc(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++) {

        for (int j = i + 1; j < n; j++) {

            if (arr[j] > arr[i]) {

                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

static int check_straight(int counts[], int *high)
{
    for (int i = ACE; i >= FIVE; i--) {

        int consecutive = 1;

        for (int j = 0; j < 5; j++) {

            if (counts[i - j] == 0) {
                consecutive = 0;
                break;
            }
        }

        if (consecutive) {
            *high = i;
            return 1;
        }
    }

    /* A-2-3-4-5 */
    if (counts[ACE] &&
        counts[TWO] &&
        counts[THREE] &&
        counts[FOUR] &&
        counts[FIVE]) {

        *high = FIVE;
        return 1;
    }

    return 0;
}

void evaluate_hand(Player *player)
{
    int rank_count[15] = {0};
    int suit_count[4] = {0};

    for (int i = 0; i < 7; i++) {

        rank_count[player->all_cards[i].rank]++;
        suit_count[player->all_cards[i].suit]++;
    }

    int flush_suit = -1;

    for (int i = 0; i < 4; i++) {

        if (suit_count[i] >= 5) {
            flush_suit = i;
            break;
        }
    }

    int straight_high = 0;

    int has_straight =
        check_straight(rank_count, &straight_high);

    /*
     * Check straight flush
     */

    if (flush_suit != -1) {

        int flush_ranks[15] = {0};

        for (int i = 0; i < 7; i++) {

            if (player->all_cards[i].suit == flush_suit) {
                flush_ranks[player->all_cards[i].rank] = 1;
            }
        }

        int sf_high = 0;

        if (check_straight(flush_ranks, &sf_high)) {

            player->hand_rank = STRAIGHT_FLUSH;
            player->score[0] = sf_high;

            return;
        }
    }

    /*
     * Four of a kind
     */

    int four = 0;

    for (int r = ACE; r >= TWO; r--) {

        if (rank_count[r] == 4) {
            four = r;
            break;
        }
    }

    if (four) {

        player->hand_rank = FOUR_OF_A_KIND;
        player->score[0] = four;

        for (int r = ACE; r >= TWO; r--) {

            if (r != four && rank_count[r] > 0) {
                player->score[1] = r;
                break;
            }
        }

        return;
    }

    /*
     * Full House
     */

    int triple = 0;
    int pair = 0;

    for (int r = ACE; r >= TWO; r--) {

        if (rank_count[r] >= 3) {

            if (!triple) {
                triple = r;
            }
            else if (!pair) {
                pair = r;
            }
        }
    }

    for (int r = ACE; r >= TWO; r--) {

        if (rank_count[r] >= 2 && r != triple) {

            if (!pair) {
                pair = r;
            }
        }
    }

    if (triple && pair) {

        player->hand_rank = FULL_HOUSE;
        player->score[0] = triple;
        player->score[1] = pair;

        return;
    }

    /*
     * Flush
     */

    if (flush_suit != -1) {

        int index = 0;

        for (int r = ACE; r >= TWO && index < 5; r--) {

            for (int i = 0; i < 7; i++) {

                if (player->all_cards[i].suit == flush_suit &&
                    player->all_cards[i].rank == r) {

                    player->score[index++] = r;
                    break;
                }
            }
        }

        player->hand_rank = FLUSH;

        return;
    }

    /*
     * Straight
     */

    if (has_straight) {

        player->hand_rank = STRAIGHT;
        player->score[0] = straight_high;

        return;
    }

    /*
     * Three of a kind
     */

    if (triple) {

        player->hand_rank = THREE_OF_A_KIND;
        player->score[0] = triple;

        int index = 1;

        for (int r = ACE; r >= TWO && index < 3; r--) {

            if (r != triple && rank_count[r] > 0) {
                player->score[index++] = r;
            }
        }

        return;
    }

    /*
     * Two Pair / One Pair
     */

    int pairs[2] = {0};
    int pair_count = 0;

    for (int r = ACE; r >= TWO; r--) {

        if (rank_count[r] >= 2) {

            if (pair_count < 2) {
                pairs[pair_count++] = r;
            }
        }
    }

    if (pair_count >= 2) {

        player->hand_rank = TWO_PAIR;

        player->score[0] = pairs[0];
        player->score[1] = pairs[1];

        for (int r = ACE; r >= TWO; r--) {

            if (r != pairs[0] &&
                r != pairs[1] &&
                rank_count[r] > 0) {

                player->score[2] = r;
                break;
            }
        }

        return;
    }

    if (pair_count == 1) {

        player->hand_rank = ONE_PAIR;
        player->score[0] = pairs[0];

        int index = 1;

        for (int r = ACE; r >= TWO && index < 4; r--) {

            if (r != pairs[0] && rank_count[r] > 0) {

                player->score[index++] = r;
            }
        }

        return;
    }

    /*
     * High Card
     */

    player->hand_rank = HIGH_CARD;

    int index = 0;

    for (int r = ACE; r >= TWO && index < 5; r--) {

        if (rank_count[r] > 0) {
            player->score[index++] = r;
        }
    }
}

const char *hand_rank_name(HandRank rank)
{
    switch (rank) {

        case HIGH_CARD:
            return "High Card";

        case ONE_PAIR:
            return "One Pair";

        case TWO_PAIR:
            return "Two Pair";

        case THREE_OF_A_KIND:
            return "Three of a Kind";

        case STRAIGHT:
            return "Straight";

        case FLUSH:
            return "Flush";

        case FULL_HOUSE:
            return "Full House";

        case FOUR_OF_A_KIND:
            return "Four of a Kind";

        case STRAIGHT_FLUSH:
            return "Straight Flush";

        default:
            return "Unknown";
    }
}

int compare_players(Player *a, Player *b)
{
    if (a->hand_rank > b->hand_rank)
        return 1;

    if (a->hand_rank < b->hand_rank)
        return -1;

    for (int i = 0; i < 5; i++) {

        if (a->score[i] > b->score[i])
            return 1;

        if (a->score[i] < b->score[i])
            return -1;
    }

    return 0;
}