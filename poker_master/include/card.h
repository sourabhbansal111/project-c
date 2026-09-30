#ifndef CARD_H
#define CARD_H

typedef enum {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
} Suit;

typedef enum {
    TWO = 2,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE
} Rank;

typedef struct {
    Rank rank;
    Suit suit;
} Card;

void print_card(Card card);

#endif