#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "info.h"

/* create a history cell which have the pair (result, operation) */
History create_history_cell(char *result, char *operation)
{
    History res = (History)malloc(sizeof(HCell));

    if (!res) {
        printf("Error to alloc a history_cell!\n");
        return NULL;
    }

    res->result = (char *)malloc(sizeof(char)*(strlen(result) + 1));
    res->operation = (char *)malloc(sizeof(char)*(strlen(operation) + 1));

    if (!res->result || !res->operation) {
        printf("Error to alloc result or operation in a history_cell!\n");
        return NULL;
    }

    strcpy(res->result, result);
    strcpy(res->operation, operation);
    res->next = NULL;

    return res;
}

/* create a list of cards */
LCards create_cards_list(char *card_number, char *pin, char *expiry_date, char *cvv)
{
    LCards res = (LCards)malloc(sizeof(Card));

    if (!res) {
        printf("Error to alloc a cards_list!\n");
        return NULL;
    }

    res->card_number = (char *)malloc(sizeof(char)*(strlen(card_number) + 1));
    res->pin = (char *)malloc(sizeof(char)*(strlen(pin) + 1));
    res->expiry_date = (char *)malloc(sizeof(char)*(strlen(expiry_date) + 1));
    res->cvv = (char *)malloc(sizeof(char)*(strlen(cvv) + 1));
    res->status = (char *)malloc(sizeof(char)*8);

    if (!res->card_number || !res->pin || !res->expiry_date || !res->cvv || !res->status) {
        printf("Error to alloc the atributes of a card!\n");
        return NULL;
    }

    strcpy(res->card_number, card_number);
    strcpy(res->pin, pin);
    strcpy(res->expiry_date, expiry_date);
    strcpy(res->cvv, cvv);
    strcpy(res->status, "NEW");
    res->balance = 0;
    res->history = NULL;
    res->pin_attempts = 0;
    res->inserted = 0;
    res->changed_pin = 0;
    res->next = NULL;


    return res;
}

/* create a list with subsets of cards */
Subsets_Cards create_all_subsets(int *pos)
{
    Subsets_Cards res = (Subsets_Cards)malloc(sizeof(Subset));

    if(!res) {
        printf("Cannot alloc all subsets!\n");
        return NULL;
    }

    res->order = (*pos);
    res->cards_subset = NULL;
    res->next = NULL;
    (*pos)++;

    return res;
}

/* frees */
void free_history(History *a)
{
    History tmp;

    while((*a)) {
        free((*a)->operation);
        free((*a)->result);
        tmp = (*a);
        (*a) = (*a)->next;
        free(tmp);
    }
}

void free_cards_list(LCards *a)
{
    LCards tmp;

    while((*a)) {
        free((*a)->card_number);
        free((*a)->pin);
        free((*a)->expiry_date);
        free((*a)->cvv);
        free((*a)->status);
        free_history(&(*a)->history);
        tmp = (*a);
        (*a) = (*a)->next;
        free(tmp);
    }
}

void free_subsets(Subsets_Cards *a)
{
    Subsets_Cards tmp;

    while((*a)) {
        free_cards_list(&(*a)->cards_subset);
        tmp = (*a);
        (*a) = (*a)->next;
        free(tmp);
    }
}