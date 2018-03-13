typedef enum { DIAMOND, CLUB, HEART, SPADE } suit_t;

typedef struct {
    int value; /* 2-10 for number cards, 10-13 for face cards, 14 for ace */
    suit_t suit;
} card_t;

typedef card_t *hand_t;

/* convert hand to/from string representation */
void hand_to_string (hand_t, char *);
void string_to_hand (const char *, hand_t);

/* misc. utility functions */
void sort_hand (hand_t);

/* hand type predicates -- note that these aren't necessarily mutually
 * exclusive! (e.g., a fullhouse is also a threeofakind and a onepair) */
int is_onepair       (hand_t);
int is_twopairs      (hand_t);
int is_threeofakind  (hand_t);
int is_straight      (hand_t);
int is_fullhouse     (hand_t);
int is_flush         (hand_t);
int is_straightflush (hand_t);
int is_fourofakind   (hand_t);
int is_royalflush    (hand_t);

int compare_hands (hand_t, hand_t);
int compare_highcards (hand_t, hand_t);
