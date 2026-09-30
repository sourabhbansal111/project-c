#ifndef DECK_H
#define DECK_H

#include "card.h"

#define DECK_SIZE 52

typedef struct {
    Card cards[DECK_SIZE];
    int top;
} Deck;

void initialize_deck(Deck *deck);
void shuffle_deck(Deck *deck);
Card deal_card(Deck *deck);

#endif