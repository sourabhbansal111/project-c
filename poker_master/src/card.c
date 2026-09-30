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