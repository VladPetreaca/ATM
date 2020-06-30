#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "info.h"

/* add a operation in the card's history with the result */
LCards add_history(LCards card, char *operation, char *result)
{
	History res = NULL;
	res = create_history_cell(result, operation);

	if (!res) {
		printf("Error to create history cell in add_history funcion\n");
		return card;
	}

	if (card->history == NULL) {
		card->history = res;
	} else {
		res->next = card->history;
		card->history = res;
	}

	return card;
}

/* insert the card in the atm */
Subsets_Cards insert_card(FILE *out, Subsets_Cards all_subsets, char *card_number, char *pin)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't find!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));
		
	if (!operation) {
		printf("Error to allocte the operation!\n");
		return all_subsets;
	}

	strcpy(operation, "insert_card ");
	strcat(operation, card_number);
	strcat(operation, " ");
	strcat(operation, pin);

	/* check if the card is inserted corectly */
	if (strcmp(res->pin, pin) == 0) {
		if (strcmp(res->status, "LOCKED") != 0) {
			if (res->changed_pin == 0 && strcmp(res->status,"ACTIVE") != 0)
				fprintf(out, "You must change your PIN.\n");

			res->inserted = 1;
			res->pin_attempts = 0;
			res = add_history(res, operation, "SUCCESS");
		} else {
			fprintf(out, "The card is blocked. Please contact the administrator.\n");
			res = add_history(res, operation, "FAIL");
		}
		
	} else {
		res->pin_attempts++;

		if (res->pin_attempts <= 3)
			fprintf(out, "Invalid PIN\n");

		if (res->pin_attempts >= 3) {
			fprintf(out, "The card is blocked. Please contact the administrator.\n");
			strcpy(res->status, "LOCKED");
		}

		res = add_history(res, operation, "FAIL");
	}

	free(operation);

	return all_subsets;
}

/* recharge the balance of the card */
Subsets_Cards recharge(FILE *out, Subsets_Cards all_subsets, char *card_number, int sum)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));
	char *sum_string = 	(char *)malloc(sizeof(char)*(20));

	if (!operation || !sum_string) {
		printf("Error to allocte the operation in recharge!\n");
		return all_subsets;
	}

	sprintf(sum_string, "%d", sum);
	strcpy(operation, "recharge ");
	strcat(operation, card_number);
	strcat(operation, " ");
	strcat(operation, sum_string);

	if (strcmp(res->status, "LOCKED") != 0) {
		if (res->inserted == 1) {
			if (sum % 10 != 0) {
				fprintf(out, "The added amount must be multiple of 10\n");
				res = add_history(res, operation, "FAIL");
			} else {
				res = add_history(res, operation, "SUCCESS");
				res->balance += sum;
				fprintf(out, "%d\n", res->balance);
			}
		
			res->inserted = 0;
		}
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	free(sum_string);
	return all_subsets;
}

/* withdraw a sum from the card */
Subsets_Cards cash_withdrawal(FILE* out, Subsets_Cards all_subsets, char *card_number, int sum)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));
	char *sum_string = 	(char *)malloc(sizeof(char)*(20));

	if (!operation || !sum_string) {
		printf("Error to allocte the operation in recharge!\n");
		return all_subsets;
	}

	sprintf(sum_string, "%d", sum);
	strcpy(operation, "cash_withdrawal ");
	strcat(operation, card_number);
	strcat(operation, " ");
	strcat(operation, sum_string);

	if (strcmp(res->status, "LOCKED") != 0) {
		if(res->inserted == 1) {
			if (sum % 10 != 0) {
				fprintf(out, "The requested amount must be multiple of 10\n");
				res = add_history(res, operation, "FAIL");
			} else {
				if (res->balance < sum) {
					fprintf(out, "Insufficient funds\n");
					res = add_history(res, operation, "FAIL");
				} else {
					res->balance -= sum;
					res = add_history(res, operation, "SUCCESS");
					fprintf(out, "%d\n", res->balance);
				}
			}

			res->inserted = 0;
		}
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	free(sum_string);
	return all_subsets;
}

/* show the balance of the card */
Subsets_Cards balance_inquiry(FILE *out, Subsets_Cards all_subsets, char *card_number)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));

	if (!operation) {
		printf("Error to allocte the operation in recharge!\n");
		return all_subsets;
	}

	strcpy(operation, "balance_inquiry ");
	strcat(operation, card_number);

	if (strcmp(res->status, "LOCKED") != 0) {
		if (res->inserted == 1) {
			fprintf(out, "%d\n", res->balance);
			res = add_history(res, operation, "SUCCESS");
			res->inserted = 0;
		}		
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	return all_subsets;
}

/* transfer funds from a card to another card */
Subsets_Cards transfer_funds(FILE *out, Subsets_Cards all_subsets, char *card1, char *card2, int sum)
{
	LCards source = NULL;
	LCards destination = NULL;

	source = find_card(all_subsets, card1);
	destination = find_card(all_subsets, card2);

	if (!source || !destination) {
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
	strcpy(operation, "transfer_funds ");
	strcat(operation, card1);
	strcat(operation, " ");
	strcat(operation, card2);
	strcat(operation, " ");
	strcat(operation, sum_string);

	if (strcmp(source->status, "LOCKED") != 0) {
		if (source->inserted == 1) {
			if (sum % 10 != 0) {
				fprintf(out, "The transferred amount must be multiple of 10\n");

				source = add_history(source, operation, "FAIL");
			} else if (sum > source->balance) {
				fprintf(out, "Insufficient funds\n");

				source = add_history(source, operation, "FAIL");
			} else {
				source->balance -= sum;
				destination->balance += sum;

				fprintf(out, "%d\n", source->balance);

				source = add_history(source, operation, "SUCCESS");
				destination = add_history(destination, operation, "SUCCESS");		
			}

			source->inserted = 0;
		}
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	free(sum_string);
	return all_subsets;
}

/* change the pin and save the result in the history */
Subsets_Cards pin_change(FILE *out, Subsets_Cards all_subsets, char *card_number, char *pin)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));

	if (!operation) {
		printf("Error to allocte the operation in recharge!\n");
		return all_subsets;
	}

	strcpy(operation, "pin_change ");
	strcat(operation, card_number);
	strcat(operation, " ");
	strcat(operation, pin);

	if (strcmp(res->status, "LOCKED") != 0) {
		if (res->inserted == 1) {
			if (strlen(pin) != 4) {
				fprintf(out, "Invalid PIN\n");

				res = add_history(res, operation, "FAIL");
			} else {
				int i;

				for(i = 0;i < strlen(pin); i++) {
					if (pin[i] < '0' || pin[i] > '9') {
						i = 10;
						break;
					}
				}

				if (i == 10) {
					fprintf(out, "Invalid PIN\n");
					res = add_history(res, operation, "FAIL");
				} else {
					res->changed_pin = 1;
					strcpy(res->pin, pin);
					res = add_history(res, operation, "SUCCESS");
					strcpy(res->status, "ACTIVE");
				}
			}

			res->inserted = 0;
		}
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	return all_subsets;
}

/* remove the card from the ATM */
Subsets_Cards cancel(FILE *out, Subsets_Cards all_subsets, char *card_number)
{
	LCards res = NULL;
	res = find_card(all_subsets, card_number);

	if (!res) {
		printf("The card wasn't found!\n");
		return all_subsets;
	}

	char *operation = (char *)malloc(sizeof(char)*(256));

	if (!operation) {
		printf("Error to allocte the operation in cancel!\n");
		return all_subsets;
	}

	strcpy(operation, "cancel ");
	strcat(operation, card_number);

	if (strcmp(res->status, "LOCKED") != 0) {
		if(res->inserted == 1) {
			res->inserted = 0;

			res = add_history(res, operation, "SUCCESS");
		}
	} else {
		fprintf(out, "The card is blocked. Please concact the administrator.\n");
	}

	free(operation);
	return all_subsets;
}