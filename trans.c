// Bank-account program - modified version
// Same menu logic: create txt, update, add, delete records in random-access file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECORDS 100

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

unsigned int enterChoice(void);
void clearInputBuffer(void);
void createEmptyFile(FILE **fPtr);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void printRecord(struct clientData client);

int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File credit.dat not found. Creating new file with %d blank records.\n", RECORDS);
        createEmptyFile(&cfPtr);
        if (cfPtr == NULL)
        {
            printf("%s: File could not be created.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    while ((choice = enterChoice())!= 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        }
        clearerr(cfPtr); // clear any file errors before next operation
    }

    fclose(cfPtr);
    puts("End of program.");
    return 0;
}

void clearInputBuffer(void)
{
    int c;
    while ((c = getchar())!= '\n' && c!= EOF);
}

void createEmptyFile(FILE **fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};
    *fPtr = fopen("credit.dat", "wb");
    if (*fPtr == NULL)
        return;

    for (int i = 0; i < RECORDS; i++)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, *fPtr);
    }
    fclose(*fPtr);
    *fPtr = fopen("credit.dat", "rb+");
}

void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;
    int count = 0;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("Could not create accounts.txt");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    fprintf(writePtr, "------------------------------------------------\n");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum!= 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName, client.firstName, client.balance);
            count++;
        }
    }

    fclose(writePtr);
    printf("%d record(s) written to accounts.txt\n", count);
}

void printRecord(struct clientData client)
{
    printf("%-6u%-16s%-11s%10.2f\n",
           client.acctNum, client.lastName, client.firstName, client.balance);
}

void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - %d): ", RECORDS);
    if (scanf("%u", &account)!= 1 || account < 1 || account > RECORDS)
    {
        puts("Invalid account number.");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fseek(fPtr, (long)(account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr)!= 1 || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    puts("\nCurrent record:");
    printRecord(client);

    printf("\nEnter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction)!= 1)
    {
        puts("Invalid amount.");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    client.balance += transaction;
    puts("\nUpdated record:");
    printRecord(client);

    fseek(fPtr, (long)(account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);
    puts("Account updated.");
}

void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;
    char confirm;

    printf("Enter account number to delete (1 - %d): ", RECORDS);
    if (scanf("%u", &accountNum)!= 1 || accountNum < 1 || accountNum > RECORDS)
    {
        puts("Invalid account number.");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fseek(fPtr, (long)(accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr)!= 1 || client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    printf("Delete this record? ");
    printRecord(client);
    printf("Confirm (y/n): ");
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y')
    {
        fseek(fPtr, (long)(accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        fflush(fPtr);
        puts("Account deleted.");
    }
    else
    {
        puts("Delete canceled.");
    }
}

void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - %d): ", RECORDS);
    if (scanf("%u", &accountNum)!= 1 || accountNum < 1 || accountNum > RECORDS)
    {
        puts("Invalid account number.");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    fseek(fPtr, (long)(accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) == 1 && client.acctNum!= 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    memset(&client, 0, sizeof(struct clientData));
    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance)!= 3)
    {
        puts("Invalid input.");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    client.acctNum = accountNum;
    fseek(fPtr, (long)(accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);
    puts("Account created.");
}

unsigned int enterChoice(void)
{
    unsigned int menuChoice;
    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    if (scanf("%u", &menuChoice)!= 1)
    {
        menuChoice = 0;
        clearInputBuffer();
    }
    else
    {
        clearInputBuffer();
    }
    return menuChoice;
}