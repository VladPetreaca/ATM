#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

int main(int argc, char **argv)
{
    int contor_subsets, nr_max, curr_cards, deleted_cards;
    Subsets_Cards all_subsets = NULL;
    FILE *in, *out;
    char *operation = NULL;
    int check = 0;

    nr_max = 0;
    curr_cards = 0;
    deleted_cards = 0;
    operation = (char *)malloc(sizeof(char) * 256);
    
    if (!operation) {
        printf("Failed to reading a operation!\n");
        return -1;
    }

    in = fopen("input.in", "r");
    out = fopen("output.out", "w");

    if (!in || !out) {
        printf("Error to open a file!\n");
        return -1;
    }

    fscanf(in, "%d", &nr_max);
    contor_subsets = 0;

	/* create a list with subsets of cards */ 
    all_subsets = create_all_subsets(&contor_subsets);
    
    if (!all_subsets) {
        printf("Error in main 1\n");
    }

	/* read from file every operation */
    while (!feof(in)) {
        if (check == 0)
            fscanf(in, "%s", operation);
        else 
            check = 0;

		/* add a card in the database */
        if (strcmp(operation, "add_card") == 0) {
            
            char *card_number, *pin, *expr_data, *cvv;

            card_number = (char *)malloc(sizeof(char)*17);
            pin = (char *)malloc(sizeof(char)*5);
            expr_data = (char *)malloc(sizeof(char)*6);
            cvv = (char *)malloc(sizeof(char)*4);

            if (!card_number || !pin || !expr_data || !cvv) {
                printf("Error to alloc card atributes in main!\n");
                return -1;
            }

            fscanf(in, "%s%s%s%s", card_number, pin, expr_data, cvv);

            all_subsets = add_card(out, all_subsets, &contor_subsets, &nr_max, &curr_cards, card_number, pin, expr_data, cvv);
            
            if(!all_subsets) {
                printf("Get null from add_card!\n");
                return -1;
            }

            free(card_number);
            free(pin);
            free(expr_data);
            free(cvv);

		/* print the database */
        } else if (strcmp(operation, "show") == 0) {
            strcpy(operation, "aaaaaaaaaaaaaaaa");
            fscanf(in, "%s", operation);

            if (operation[0] >= '0' && operation[0] <= '9') {
                fprintf(out, "(");

                LCards res = NULL;
                res = find_card(all_subsets, operation);

                if (!res) {
                    printf("The card wasn't found!\n");
                    return -1;
                }

                show_card(out, res);
                fprintf(out, ")\n");
            } else {
                show_all_subsets(out, all_subsets, deleted_cards);
                check = 1;
            }

		/* delete a card from database */
        } else if (strcmp(operation, "delete_card") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);

            if (!card_number) {
                printf("Error to read card number for delete a card!\n");
                return -1;
            }

            fscanf(in, "%s", card_number);

            all_subsets = delete_card(all_subsets, card_number, &curr_cards, &deleted_cards);

            if(!all_subsets) {
                printf("Get null from delete_card!\n");
                return -1;
            }

            free(card_number);

		/* insert a card in ATM for a few possible operations */
        } else if (strcmp(operation, "insert_card") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);
            char *pin = (char *)malloc(sizeof(char) * 6);

            if (!card_number || !pin) {
                printf("Error to read card_number and pin!\n");
                return -1;
            }

            fscanf(in, "%s%s", card_number, pin);
            all_subsets = insert_card(out, all_subsets, card_number, pin);

            free(card_number);
            free(pin);

		/* recharge a card which is inserated in the ATM */
        } else if (strcmp(operation, "recharge") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);
            int sum = 0;

            if (!card_number) {
                printf("Error to read card_number for recharge!\n");
                return -1;
            }

            fscanf(in, "%s%d", card_number, &sum);

            all_subsets = recharge(out, all_subsets, card_number, sum);

            free(card_number);

		/* withdraw a specified sum from the card */
        } else if (strcmp(operation, "cash_withdrawal") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);
            int sum = 0;

            if (!card_number) {
                printf("Error to read card_number for recharge!\n");
                return -1;
            }

            fscanf(in, "%s%d", card_number, &sum);

            all_subsets = cash_withdrawal(out, all_subsets, card_number, sum);

            free(card_number);

		/* interrogates the balance */
        } else if (strcmp(operation, "balance_inquiry") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);

            if (!card_number) {
                printf("Error to read card_number for recharge!\n");
                return -1;
            }

            fscanf(in, "%s", card_number);

            all_subsets = balance_inquiry(out, all_subsets, card_number);

            free(card_number);

		/* transfer funds from a card to another */
        } else if (strcmp(operation, "transfer_funds") == 0) {
            char *card_number1 = (char *)malloc(sizeof(char) * 17);
            char *card_number2 = (char *)malloc(sizeof(char) * 17);
            int sum = 0;

            if (!card_number1 || !card_number2) {
                printf("Error to read card_number1 and card_number2!\n");
                return -1;
            }

            fscanf(in, "%s%s%d", card_number1, card_number2, &sum);

            all_subsets = transfer_funds(out, all_subsets, card_number1, card_number2, sum);

            free(card_number1);
            free(card_number2);

		/* change the pin of a card */
        } else if (strcmp(operation, "pin_change") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);
            char *pin = (char *)malloc(sizeof(char) * 6);

            if (!card_number || !pin) {
                printf("Error to read card_number and pin!\n");
                return -1;
            }

            fscanf(in, "%s%s", card_number, pin);

            all_subsets = pin_change(out, all_subsets, card_number, pin);

            free(card_number);
            free(pin);
		
		/* remove the card from the ATM */
        } else if (strcmp(operation, "cancel") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);

            if (!card_number) {
                printf("Error to read card_number\n");
                return -1;
            }

            fscanf(in, "%s", card_number);

            all_subsets = cancel(out, all_subsets, card_number);

            free(card_number);

		/* unblock a locked card */
        } else if (strcmp(operation, "unblock_card") == 0) {
            char *card_number = (char *)malloc(sizeof(char) * 17);

            if (!card_number) {
                printf("Error to read card_number\n");
                return -1;
            }

            fscanf(in, "%s", card_number);

            all_subsets = unblock(all_subsets, card_number);

            free(card_number);
		
		/* reverse a transaction if it's possible */
        } else if (strcmp(operation, "reverse_transaction") == 0) {
            char *card_1 = (char *)malloc(sizeof(char) * 17);
            char *card_2 = (char *)malloc(sizeof(char) * 17);
            int sum = 0;

            if (!card_1 || !card_2) {
                printf("Error to read card_1 and card_2\n");
                return -1;
            }

            fscanf(in, "%s%s%d", card_1, card_2, &sum);

            all_subsets = reverse_transaction(out, all_subsets, card_1, card_2, sum);

            free(card_1);
            free(card_2);
        }
    }

	/* frees */
    free_subsets(&all_subsets);
    free(operation);
    fclose(in);
    fclose(out);
    return 0;
}