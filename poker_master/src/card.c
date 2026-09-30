#include <stdio.h>
#include "card.h"

void print_card(Card card)
{
    const char *ranks[] = {
        "", "", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "J", "Q", "K", "A"
    };

    const char *suits[] = {
        "C", "D", "H", "S"
    };

    printf("%s%s", ranks[card.rank], suits[card.suit]);
}

void print_cards(Card cards[], int count)
{
    for (int i = 0; i < count; i++) {
        print_card(cards[i]);

        if (i < count - 1)
            printf(" ");
    }

    printf("\n");
}