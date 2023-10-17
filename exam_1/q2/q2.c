#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SUITS "CDHS"
#define RANKS "A23456789TJQK"
#define NCARDS 52

#define ERROR (stderr)

typedef struct card_ {
    char rank;
    char suit;
} card;

typedef struct deck_ {
    size_t top;     // the index into cards of the top card in the deck
    card **cards;   // an array of NCARDS pointers to cards
} deck;

/**
 * Takes as input a deck (which is an array-based list implementation) and 
 * size_t n, and returns the first n cards off the top of the deck.
 *
 * If the number of requested cards exceeds the number of cards in the deck,
 * a NULL pointer is returned and there is no effect on the deck.
 *
 * Drawn cards should reside at the same memory address as when they were
 * added to the deck and ownership of the returned cards is transferred
 * to the caller.
 *
 * Your implementation need not make any calls to functions defined below.
 */

card **deck_draw_n(deck *d, size_t n) {
    // WRITE THE deck_draw_n FUNCTION HERE
    if(n >= d->top) return NULL;
    card** drawn;
    drawn = malloc(n * sizeof(card*));
    for(int i = 0; i < n; i++)
        drawn[n - i] = d->cards[d->top - i];
    d->top = d->top - n;
    return drawn;
}

// DO NOT MODIFY THIS LINE. YOU MAY MODIFY SUBSEQUENT LINES, BUT ANY CHANGES MADE WILL NOT BE TAKEN INTO ACCOUNT DURING THE GRADING PROCESS.

deck *deck_create_empty(void) {
    deck *d = malloc(sizeof(deck));

    if (d == NULL) {
        return d;
    }

    card **cards = malloc(sizeof(card *) * NCARDS);

    if (cards == NULL) {
        free(d);
        return NULL;
    }

    d->cards = cards;

    return d;
}

deck *deck_create() {
    deck *d = deck_create_empty();

    if (d == NULL) {
        return NULL;
    }

    d->top = 0;

    int i = 0;
    for (int s = 0; SUITS[s]; s++) {
        for (int r = 0; RANKS[r]; r++) {
          card *c = malloc(sizeof(card));
          c->suit = SUITS[s];
          c->rank = RANKS[r];
          d->cards[i++] = c;
        }
    }

    return d;
}

void deck_destroy(deck *d) {
  for (size_t i = d->top; i < NCARDS; i++)
    {
      free(d->cards[i]);
    }
    free(d->cards);
    free(d);
}

void deck_print(const deck *d) {
    for (size_t i = d->top; i < NCARDS - 1; i++) {
        printf("%c%c ", d->cards[i]->rank, d->cards[i]->suit);
    }
    if (d->top < NCARDS)
      {
	printf("%c%c\n", d->cards[NCARDS - 1]->rank, d->cards[NCARDS - 1]->suit);
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

    card *top_card = d->cards[d->top];

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
