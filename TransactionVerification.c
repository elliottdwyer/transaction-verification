// Elliott Dwyer
// 260943981
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct ACCOUNT {
	int accountNumber;
        double startingBalance;
        double endingBalance;
        struct ACCOUNT *next;
};

void errorMsg()
{
	char *syntax = "./tv STATE TRANSACTIONS";
	char *legal_usage = "./tv state.csv transaction.csv\n./tv ../state.csv /data/log/transaction.csv";
	printf("%s\n%s\n", syntax, legal_usage);
}

// INSERT NODE AS HEAD FUNCTION

void insert_at_head(struct ACCOUNT **head, struct ACCOUNT *newnode){
	newnode->next = *head;
	*head = newnode;
}	


// INSERT AFTER NODE FUNCTION

void insert_after_node(struct ACCOUNT *node_to_insert_after, struct ACCOUNT *newnode){;
	newnode->next = node_to_insert_after->next;
	node_to_insert_after->next = newnode;
}

int main(int argc, char *argv[])
{
	// Command Line Arguments Error Checking
	if (argc != 3){
		printf("Wrong number of arguments!\n");
		errorMsg();
		exit(1);
	}
	
	FILE *state = fopen(argv[1], "rt"), *transactions = fopen(argv[2], "rt");
	
	// Check that filenames are valid
	if (state == NULL){
		printf("Unable to open the filename %s\n", argv[1]);
		errorMsg();
		exit(2);
	}
	else if (transactions == NULL){
		printf("Unable open the filename %s\n", argv[2]);
		errorMsg();
		exit(2);
	}

	
	// ========================================================================================================================================================================== \\
									READING THROUGH THE STATE.CSV FILE					
    // ========================================================================================================================================================================== \\
	
	struct ACCOUNT *head = NULL; // Creates head pointer of linked list	
	char acct[100];

	fgets(acct, 100, state); // skips over first line

	// PARSING THROUGH LINE ===================================================================================================================================================== \\
	
	while (fgets(acct,100,state)){	
		
		char accountNumber[20], startingBalance[20], endingBalance[20];

		sscanf(acct,"%[^,],%[^,],%s", accountNumber, startingBalance, endingBalance);

		// CREATE TEMPORARY NODE ============================================================================================================================================= \\

		struct ACCOUNT *temp;
		temp = (struct ACCOUNT *)malloc(sizeof(struct ACCOUNT));
		
		temp->accountNumber = atoi(accountNumber);
		temp->startingBalance = atof(startingBalance);
		temp->endingBalance = atof(endingBalance);
		temp->next = NULL;
		
		if (temp->accountNumber == 0){
			continue;
		}

		
		// ADD NODE TO LINKED LIST =========================================================================================================================================== \\

		// First check that the account number does not already exist in the linked list
		
		struct ACCOUNT *test = head;
		int duplicate_error = 1;

		while (test != NULL){
			if (temp->accountNumber == test->accountNumber){
			       	duplicate_error = 0;
				break;
			}
			else duplicate_error = 1;
			test = test->next;
		}
		
		if (duplicate_error == 0){
			printf("Duplicate account number: [account, start, end]: %d, %0.2f, %0.2f\n", temp->accountNumber, temp->startingBalance, temp->endingBalance);
			continue;
		}

		// If first node, add to list with no accountNumber checks

		if (head == NULL){
			head = temp;
			temp->next = NULL;
			continue;
		}

		// Set pointer to head and begin iterating through linked list to find where to link temp
		
		else {
			struct ACCOUNT *ptr = head;

			if (temp->accountNumber < head->accountNumber) insert_at_head(&head, temp); // First checks to see if it is smaller than head
			else {
				while (ptr->next != NULL && ptr->next->accountNumber < temp->accountNumber) ptr = ptr->next; //Iterates through nodes until a greater than is found
				insert_after_node(ptr, temp);
			}
		}


		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------- \\
		
	}
	fclose(state);

	// ========================================================================================================================================================================== \\
										READING THROUGH THE TRANSACTION.CSV FILE
	// ========================================================================================================================================================================== \\

	// Parsing through file line by line
	
	char transaction[100];

	fgets(transaction, 100, transactions); // Reads header line to skip it
        
	while (1){

		// Parse through record ============================================================================================================================================ \\

		char accountNumber_str[20], absoluteValueAmount_str[20];
        char mode_str[1] = "n";
		int account_found = 0;
		
        fgets(transaction, 100, transactions); // creates a character array which will contain the account information

		sscanf(transaction,"%[0-9],%[^,],%s", accountNumber_str, mode_str, absoluteValueAmount_str);

		int accountNumber = atoi(accountNumber_str);
		char mode = mode_str[0];

		if (feof(transactions)) break;

		if(mode == 'n'){
			continue; // skips over blank lines
		}
		
		account_found = 1; // signals that an account was found under the transaction.csv file (it is not empty)

		double absoluteValueAmount = atof(absoluteValueAmount_str); // stores absolute value amount as float
		
		// If state.csv file is empty but transaction.csv file has data, program exits:
		
		if (head == NULL && account_found == 1){
			printf("File %s is empty. Unable to validate transaction.csv\n", argv[1]);
			exit(3);
		}

		// Find account number in linked list ============================================================================================================================= \\

		struct ACCOUNT *ptr = head;
		int accountError = 1;

		while (ptr != NULL){
			if (accountNumber == ptr->accountNumber){
				accountError = 0;
				break;
			}
			ptr = ptr->next;
		}

		if (accountError == 1){
			printf("Account not found (account, mode, amount): %d, %c, %0.2f\n", accountNumber, mode, absoluteValueAmount);
			continue;
		}

		// Calculate result of withdrawals or deposits =================================================================================================================== \\
		
		double startingBalance = ptr->startingBalance; // store starting balance in the case of an error

		if (mode == 'w') ptr->startingBalance = ptr->startingBalance - absoluteValueAmount;
		else if (mode == 'd') ptr->startingBalance = ptr->startingBalance + absoluteValueAmount;

		if (ptr->startingBalance < 0){
			printf("Balance below zero error (account, mode, transaction, startingBalance before): %d, %c, %0.2f, %0.2f\n", accountNumber, mode, absoluteValueAmount, startingBalance);
			ptr->startingBalance = 0.0;
		}
		
	}
	
	fclose(transactions);
		
        // ========================================================================================================================================================================== \\
											FINAL STATISTICS
        // ========================================================================================================================================================================== \\
	
	struct ACCOUNT *stat = head;
		
	while (stat != NULL){
		if (stat->startingBalance != stat->endingBalance){
			printf("End of day balances do not agree (account, starting, ending): %d, %0.2f, %0.2f\n", stat->accountNumber, stat->startingBalance, stat->endingBalance);
		}
		stat = stat->next;
	}

        // ========================================================================================================================================================================== \\
											DEALLOCATING MEMORY
        // ========================================================================================================================================================================== \\

	struct ACCOUNT *ptr = head;
	
	while (ptr != NULL){
		free(ptr);
		ptr = ptr->next;
	}

	return 0;
}







