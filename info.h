#include <stdio.h>

typedef struct hist {
    char *result;
    char *operation;
    struct hist *next;
}HCell, *History;

typedef struct card {
    char *card_number;
    char *pin;
    char *expiry_date;
    char *cvv;
    int balance;
    char *status;
    History history;
    int pin_attempts;
    int inserted;
    int changed_pin;
    struct card *next;
}Card, *LCards;

typedef struct subsets {
    int order;
    LCards cards_subset;
    struct subsets *next;
}Subset, *Subsets_Cards; 

/* allocation of the structures */
History create_history_cell(char *result, char *operation);
LCards create_cards_list(char *card_number, char *pin, char *expiry_date, char *cvv);
Subsets_Cards create_all_subsets(int *pos);

/* admin operations */
Subsets_Cards add_card(FILE *out, Subsets_Cards all_subsets, int *contor_subsets, int *nr_Max, int *curr_cards, char *card_number, char *pin, char *expiry_date, char *cvv);
void show_history(FILE *out, LCards card);
void show_card(FILE *out, LCards card);
void show_all_subsets(FILE *out, Subsets_Cards all_subsets, int deleted_cards);
Subsets_Cards delete_card(Subsets_Cards all_subsets, char *card_number, int *curr_card, int *deleted_cards);
LCards find_card(Subsets_Cards all_subsets, char *card_number);
Subsets_Cards reverse_transaction(FILE *out, Subsets_Cards all_subsets, char *card1, char *card2, int sum);
Subsets_Cards unblock(Subsets_Cards all_subsets, char *card_number);
Subsets_Cards resize(Subsets_Cards all_subsets, int *contor_subsets, int curr_cards, int nr_max);
History reverse_history_list(History l);

/* client operations */
LCards add_history(LCards card, char *operation, char *result);
Subsets_Cards insert_card(FILE *out, Subsets_Cards all_subsets, char *card_number, char *pin);
Subsets_Cards recharge(FILE *out, Subsets_Cards all_subsets, char *card_number, int sum);
Subsets_Cards cash_withdrawal(FILE *out, Subsets_Cards all_subsets, char *card_number, int sum);
Subsets_Cards balance_inquiry(FILE *out, Subsets_Cards all_subsets, char *card_number);
Subsets_Cards transfer_funds(FILE *out, Subsets_Cards all_subsets, char *card1, char *card2, int sum);
Subsets_Cards pin_change(FILE *out, Subsets_Cards all_subsets, char *card_number, char *pin);
Subsets_Cards cancel(FILE *out, Subsets_Cards all_subsets, char *card_number);

/* frees */
void free_history(History *a);
void free_cards_list(LCards *a);
void free_subsets(Subsets_Cards *a);