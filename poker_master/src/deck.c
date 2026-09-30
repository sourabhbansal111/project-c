#include <stdlib.h>
#include "deck.h"

void initialize_deck(Deck *deck)
{
    int index = 0;

    for (int suit = CLUBS; suit <= SPADES; suit++) {
        for (int rank = TWO; rank <= ACE; rank++) {

            deck->cards[index].suit = suit;
            deck->cards[index].rank = rank;

            index++;
        }
    }

    deck->top = 0;
}

void shuffle_deck(Deck *deck)
{
    for (int i = DECK_SIZE - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }

    deck->top = 0;
}

Card deal_card(Deck *deck)
{
    if (deck->top >= DECK_SIZE) {
        Card empty = {0, 0};
        return empty;
    }

    return deck->cards[deck->top++];
}