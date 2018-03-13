#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "poker.h"

/* converts a hand (of 5 cards) to a string representation, and stores it in the
 * provided buffer. The buffer is assumed to be large enough.
 */
void hand_to_string (hand_t hand, char *handstr) {
    char *p = handstr;
    int i;
    char *val, *suit;
    for (i=0; i<5; i++) {
        if (hand[i].value < 10) {
            *p++ = hand[i].value + '0';
        } else {
            switch(hand[i].value) {
            case 10: *p++ = 'T'; break;
            case 11: *p++ = 'J'; break;
            case 12: *p++ = 'Q'; break;
            case 13: *p++ = 'K'; break;
            case 14: *p++ = 'A'; break;
            }
        }
        switch(hand[i].suit) {
        case DIAMOND: *p++ = 'D'; break;
        case CLUB: *p++ = 'C'; break;
        case HEART: *p++ = 'H'; break;
        case SPADE: *p++ = 'S'; break;
        }
        if (i<=3) *p++ = ' ';
    }
    *p = '\0';
}

/* converts a string representation of a hand into 5 separate card structs. The
 * given array of cards is populated with the card values.
 */
void string_to_hand (const char *handstr, hand_t hand) {
}

/* sorts the hands so that the cards are in ascending order of value (two
 * lowest, ace highest */
void sort_hand (hand_t hand) {
}

int count_pairs (hand_t hand) {
    return 0;
}

int is_onepair (hand_t hand) {
    return 0;
}

int is_twopairs (hand_t hand) {
    return 0;
}

int is_threeofakind (hand_t hand) {
    return 0;
}

int is_straight (hand_t hand) {
    return 0;
}

int is_fullhouse (hand_t hand) {
    return 0;
}

int is_flush (hand_t hand) {
    return 0;
}

int is_straightflush (hand_t hand) {
    return 0;
}

int is_fourofakind (hand_t hand) {
    return 0;
}

int is_royalflush (hand_t hand) {
    return 0;
}

/* compares the hands based on rank -- if the ranks (and rank values) are
 * identical, compares the hands based on their highcards.
 * returns 0 if h1 > h2, 1 if h2 > h1.
 */
int compare_hands (hand_t h1, hand_t h2) {
    return compare_highcards(h1, h2);
}

/* compares the hands based solely on their highcard values (ignoring rank). if
 * the highcards are a draw, compare the next set of highcards, and so forth.
 */
int compare_highcards (hand_t h1, hand_t h2) {
    return 0;
}
