#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SUITS "CDHS"
#define RANKS "A23456789TJQK"
#define NCARDS 52
#define NSUITS 4
#define NRANKS 13

#define ERROR (stderr)

typedef struct card_ {
    char rank;   /* from RANKS */
    char suit;   /* from SUITS */
    struct card_ *next;
} card;

typedef struct deck_ {
    card *top;  // a pointer to the top card in the deck
} deck;

/**
 * Takes as input a deck (which is a linked-list implementation) and size_t n,
 * and returns the first n cards off the top of the deck.
 *
 * If the number of requested cards exceeds the number of cards in the deck,
 * a NULL pointer is returned and there is no effect on the deck.
 *
 * Drawn cards should reside at the same memory address as when they were
 * added to the deck and ownership is transferred to the caller.
 *
 * Your implementation need not make any calls to functions defined below.
 */

card **deck_draw_n(deck *d, size_t n) {
    // WRITE THE deck_draw_n FUNCTION HERE
}

// DO NOT MODIFY THIS LINE. YOU MAY MODIFY SUBSEQUENT LINES, BUT ANY CHANGES MADE WILL NOT BE TAKEN INTO ACCOUNT DURING THE GRADING PROCESS.

deck *deck_create_empty(void) {
    deck *d = malloc(sizeof(deck));

    if (d == NULL) {
        return d;
    }

    d->top = NULL;

    return d;
}

card *deck_create_card(card c) {
    card *new_card = malloc(sizeof(card));
    new_card->suit = c.suit; new_card->rank = c.rank;
    return new_card;
}

// place card on top of deck
void deck_put_card(deck *d, card c) {
    card *card = deck_create_card(c);
    card->next = d->top;
    d->top = card;
}

deck *deck_create() {
    deck *d = deck_create_empty();

    if (d == NULL) {
        return NULL;
    }

    card c;

    for (int s = NSUITS - 1; s >= 0; s--) {
        c.suit = SUITS[s];
        for (int r = NRANKS - 1; r >= 0; r--) {
            c.rank = RANKS[r];
            deck_put_card(d, c);
        }
    }

    return d;
}

void deck_destroy(deck *d) {
    for (card *c = d->top; c != NULL;) {
        card *next = c->next;
        free(c);
        c = next;
    }
    free(d);
}

void deck_print(const deck *d) {
    card *c;
    for (c = d->top; c != NULL && c->next != NULL; c = c->next) {
        printf("%c%c ", c->rank, c->suit);
    }
    if (c != NULL)
      {
	printf("%c%c\n", c->rank, c->suit);
      }
    else
      {
	printf("\n");
      }
}

void deck_error_insufficient_cards(deck *d) {
    deck_destroy(d);
    printf("The requested number of cards to be drawn exceeded the number of cards in the deck\n");
}

int main() {
    deck *d = deck_create();

    size_t n_cards = 5;

    card *top_card = d->top;

    card **drawn_cards = deck_draw_n(d, n_cards);

    // NOT EXPECTED TO BE NULL
    if (drawn_cards == NULL) {
        deck_error_insufficient_cards(d);
        return 1;
    }

    // FIRST TEST CASE: pointer to top card should remain valid even
    // after said card is drawn
    // output: AC
    printf("First drawn card: %c%c\n", top_card->rank, top_card->suit);

    // SECOND TEST CASE: expected behavior is printing of five freshly drawn
    // cards
    // output: AC 2C 3C 4C 5C
    printf("Drawn cards: ");

    for (size_t i = 0; i < n_cards; i++) {
        printf("%c%c ", drawn_cards[i]->rank, drawn_cards[i]->suit);
    }
    
    for (size_t i = 0; i < n_cards; i++) {
        free(drawn_cards[i]);
    }

    free(drawn_cards);

    printf("\nRemaining cards in the deck: ");

    // THIRD TEST CASE: expected behavior is printing of remaining 47 cards
    // output: 6C 7C 8C 9C TC JC QC KC AD 2D 3D 4D ... 7S 8S 9S TS JS QS KS
    deck_print(d);

    n_cards = 55;

    card **more_drawn_cards = deck_draw_n(d, n_cards);

    // FOURTH TEST CASE: return value is NULL and so main prints an error
    // message
    if (more_drawn_cards == NULL) {
        deck_error_insufficient_cards(d);
        return 1;
    }

    free(more_drawn_cards);

    deck_destroy(d);
}
