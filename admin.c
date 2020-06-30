#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "info.h"

/* resize the database if there isn't more space and copy the older cards on their new positions*/
Subsets_Cards resize(Subsets_Cards all_subsets, int *contor_subsets, int curr_cards, int nr_max)
{
	(*contor_subsets) = 0;

    Subsets_Cards res = create_all_subsets(contor_subsets);
    int i, result_position;

    Subsets_Cards park = NULL;
    park = all_subsets;

    while(park) {
        LCards park2 = NULL;
        park2 = park->cards_subset;

        while(park2) {
			/* create a new card */
            LCards new_card = create_cards_list(park2->card_number, park2->pin, park2->expiry_date, park2->cvv);

            if(!new_card) {
                printf("Error to craete a new card!\n");
                return NULL;
            }

            result_position = 0;

			/* copy all the details in the new card */
            new_card->balance = park2->balance;
            new_card->changed_pin = park2->changed_pin;
            new_card->inserted = park2->inserted;
            new_card->pin_attempts = park2->pin_attempts;
            strcpy(new_card->status, park2->status);
            new_card->next = NULL;

            History park3 = NULL;
            park3 = park2->history;

			/* copy the transaction history */
            while(park3) {
                new_card = add_history(new_card, park3->operation, park3->result);
                park3 = park3->next;
            }

			new_card->history = reverse_history_list(new_card->history);

            /* add the cards on their positios */
            for(i = 0; i < 16; i++) {
                result_position += new_card->card_number[i] - '0';

                if (i == 15)
                    result_position %= nr_max;
            }

			/* required position is already created */
            if ((*contor_subsets) - 1 < result_position) {

				/* going to the specified position and put the card into it */
				for(i = (*contor_subsets); i <= result_position; i++) {
					Subsets_Cards aux = NULL;
					aux = create_all_subsets(contor_subsets);
					
					if(!aux) {
						printf("Error to create aux\n");
						return NULL;
					}

					Subsets_Cards crossing_subsets = NULL;
					crossing_subsets = res;

					if (i == result_position) {
						while (crossing_subsets->next)
							crossing_subsets = crossing_subsets->next;

						aux->cards_subset = new_card;
						crossing_subsets->next = aux;
					}
					else {
						while (crossing_subsets->next)
							crossing_subsets = crossing_subsets->next;

						crossing_subsets->next = aux;
					}
				}				
			}

			/* create the last position and put the card */
			else {
				Subsets_Cards crossing_subsets = NULL;
				crossing_subsets = res;

				for(i = 0;i < result_position; i++) {
					crossing_subsets = crossing_subsets->next;
				}

				new_card->next = crossing_subsets->cards_subset;
				crossing_subsets->cards_subset = new_card;
			}

			park2 = park2->next;
		}

		park = park->next;
	}

	free_subsets(&all_subsets);
	return res;
}

/* add a card in the specified(calculated by the formula sum of card number % nrMax of cards) position */
Subsets_Cards add_card(FILE *out, Subsets_Cards all_subsets, int *contor_subsets, int *nr_Max, int *curr_cards, char *card_number, char *pin, char *expiry_date, char *cvv)
{
	LCards res;
	int i, result_position;
	Subsets_Cards park_subsets = NULL;

	park_subsets = all_subsets;
	i = 0;

	/* check if the card already exists */
	while(park_subsets) {
		LCards park = park_subsets->cards_subset;

		while(park) {
			if (strcmp(card_number, park->card_number) == 0) {
				fprintf(out, "The card already exists\n");
				i++;
				break;
			}
			
			if (i == 1)
				break;

			park = park->next;
		}
		park_subsets = park_subsets->next;
	}

	if (i == 1)
		return all_subsets;

	/* create a new card */
	res = create_cards_list(card_number, pin, expiry_date, cvv);
	
	if (!res) {
		printf("Error to add a card in a database. The card is inexistent!\n");
		return NULL;
	}

	result_position = 0;

	/* calculate the sum of card' digits */
	for(i = 0;i < 16; i++) {
		result_position += card_number[i] - '0';

		if (i == 15) {
			if ((*nr_Max) == (*curr_cards)) {
				result_position %= (*nr_Max) * 2;
			}
			else {
				result_position %= (*nr_Max);
			}
		}
	}

	/* check if it's need to more space */
	if ((*curr_cards) == (*nr_Max)) {
		(*nr_Max) *= 2;
	    all_subsets = resize(all_subsets, contor_subsets, (*curr_cards), (*nr_Max));
	}

	(*curr_cards)++;

	/* put the card on the correct position */
	if ((*contor_subsets) - 1 < result_position) {
		for(i = (*contor_subsets); i <= result_position; i++) {
			Subsets_Cards aux = NULL;
			aux = create_all_subsets(contor_subsets);
			
			if(!aux) {
				printf("Error to create aux\n");
				return NULL;
			}

			Subsets_Cards park = NULL;
			park = all_subsets;

			if (i == result_position) {
				while (park->next)
					park = park->next;

				aux->cards_subset = res;
				park->next = aux;
			}
			else {
				while (park->next)
					park = park->next;

				park->next = aux;
			}
		}
	}
	else {
		Subsets_Cards park = NULL;
		park = all_subsets;

		for(i = 0;i < result_position; i++) {
			park = park->next;
		}

		res->next = park->cards_subset;
		park->cards_subset = res;
	}

	return all_subsets;
}

/* print the history of a card */
void show_history(FILE *out, LCards card)
{
	if (!card->history) {
		fprintf(out, "[]");
	}
	else {
		History park = NULL;
		park = card->history;
		
		fprintf(out, "[");

		while(park) {
			if(!park->next)
				fprintf(out, "(%s, %s)", park->result, park->operation);
			else
				fprintf(out, "(%s, %s), ", park->result, park->operation);

			park = park->next;
		}

		fprintf(out, "]");
	}
}

/* print the atrributes of the cards */
void show_card(FILE *out, LCards card)
{
	fprintf(out, "card number: %s, PIN: %s, expiry date: %s, ", card->card_number, card->pin, card->expiry_date);
	fprintf(out, "CVV: %s, balance: %d, status: %s, ", card->cvv, card->balance, card->status);
	fprintf(out, "history: ");

	/* print history */
	show_history(out, card);
}

/* print all subsets with cards */
void show_all_subsets(FILE *out, Subsets_Cards all_subsets, int deleted_cards)
{
	Subsets_Cards park_subsets = NULL;
	park_subsets = all_subsets;

	if (deleted_cards == 0 && !all_subsets->next && !all_subsets->cards_subset)
		return;

	while(park_subsets) {
		fprintf(out, "pos%d: ", park_subsets->order);

		if(!park_subsets->cards_subset) {
			fprintf(out, "[]\n");
		}
		else {
			fprintf(out, "[\n");

			LCards park = NULL;
			park = park_subsets->cards_subset;

			while(park) {
				fprintf(out, "(");
				show_card(out, park);

				if(park->next) {
					/* posibila modificare la afisare */
					fprintf(out, ")\n");
				}
				else {
					fprintf(out, ")\n");
				}

				park = park->next;
			}

			fprintf(out, "]\n");
		}

		park_subsets = park_subsets->next;
	}
}

/* delete a specified card from the list */
Subsets_Cards delete_card(Subsets_Cards all_subsets, char *card_number, int *curr_card, int *deleted_cards)
{
	Subsets_Cards park_subsets = NULL;
	park_subsets = all_subsets;
	int i = 0;

	while(park_subsets) {
		LCards park = NULL;
		LCards prev = NULL;

		park = park_subsets->cards_subset;
		
		while(park) {
			
			if (strcmp(park->card_number, card_number) == 0) {

				free(park->card_number);
				free(park->cvv);    
				free(park->pin);
				free(park->expiry_date);
				free(park->status);
				free_history(&park->history);

				if (!prev) {
					LCards tmp = park;
					park_subsets->cards_subset = park->next;
					free(tmp);
					i++;
					break;

				} else {
					LCards tmp = park;
					prev->next = tmp->next;
					free(tmp);
					i++;
					break;
				}
			}

			if (i == 1)
				break;

			prev = park;
			park = park->next;
		}

		park_subsets = park_subsets->next;
	}

	(*curr_card)--;
	(*deleted_cards)++;
	return all_subsets;

}

/* search a card in all subsets with cards by his number */
LCards find_card(Subsets_Cards all_subsets, char *card_number)
{
	Subsets_Cards park_subsets = NULL;
	park_subsets = all_subsets;

	while(park_subsets) {
		LCards park = NULL;
		park = park_subsets->cards_subset;

		while(park) {
			if (strcmp(park->card_number, card_number) == 0) {
				return park;
			}

			park = park->next;
		}

		park_subsets = park_subsets->next;
	}

	return NULL;
}

/* reverse a transaction, complete the history of the source and destination and refund the money */
Subsets_Cards reverse_transaction(FILE *out, Subsets_Cards all_subsets, char *card1, char *card2, int sum)
{
	LCards source = find_card(all_subsets, card1);
	LCards destination = find_card(all_subsets, card2);

	if(!source || !destination) {
		printf("The cards weren't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));
	char *sum_string = 	(char *)malloc(sizeof(char)*(20));

	if (!operation || !sum_string) {
		printf("Error to allocte the operation in recharge!\n");
		return all_subsets;
	}

	sprintf(sum_string, "%d", sum);
	strcpy(operation, "reverse_transaction ");
	strcat(operation, card1);
	strcat(operation, " ");
	strcat(operation, card2);
	strcat(operation, " ");
	strcat(operation, sum_string);

	if (destination->balance < sum) {
		fprintf(out, "The transaction cannot be reversed\n");
	} else {
		destination->balance -= sum;
		source->balance += sum;

		source = add_history(source, operation, "SUCCESS");

		char *op = (char *)malloc(sizeof(char) * 256);

		if (!op) {
			printf("Error to alloc memory for op!\n");
		}

		strcpy(op, "transfer_funds ");
		strcat(op, card1);
		strcat(op, " ");
		strcat(op, card2);
		strcat(op, " ");
		strcat(op, sum_string);

		History park = destination->history;
		History prev = NULL;

		while(park) {
			if (strcmp(park->operation, op) == 0) {
				if(!prev) {
					History tmp = NULL;
					free(park->operation);
					free(park->result);
					tmp = park;
					destination->history = park->next;
					free(tmp);
				} else {
					History tmp = NULL;
					free(park->operation);
					free(park->result);
					tmp = park;
					prev->next = park->next;
					free(tmp);
				}

				break;
			}

			prev = park;
			park = park->next;
		}

		free(op);
	}

	free(operation);
	free(sum_string);
	return all_subsets;
}

/* unlock a card from the block state */
Subsets_Cards unblock(Subsets_Cards all_subsets, char *card_number)
{
	LCards res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	res->pin_attempts = 0;
	strcpy(res->status, "ACTIVE");

	return all_subsets;
}

/* reverse a history list */
History reverse_history_list(History l)
{
	History prev = NULL, curr = NULL, next = NULL;

	curr = l;

	while(curr) {
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}

	l = prev;

	return l;
}