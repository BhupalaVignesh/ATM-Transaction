#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define FILENAME "account.txt"
pthread_mutex_t mutex; // Mutex for synchronizing balance access

typedef struct {
    int balance;
} Account;

// Function to load the account balance from the file
void load_balance(Account *account) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("File not found, initializing balance to 5000\n");
        account->balance = 5000; // Default balance if file doesn't exist
    } else {
        fscanf(file, "%d", &account->balance);
        fclose(file);
    }
}

// Function to save the account balance to the file
void save_balance(Account *account) {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "%d", account->balance);
    fclose(file);
}

// Function to check the balance
void *check_balance(void *arg) {
    Account *account = (Account *)arg;
    pthread_mutex_lock(&mutex);
    printf("Your current balance is %d\n", account->balance);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// Function to deposit money
void *deposit(void *arg) {
    Account *account = (Account *)arg;
    int amount;
    printf("Enter the amount to deposit: ");
    scanf("%d", &amount);

    pthread_mutex_lock(&mutex);
    account->balance += amount;
    save_balance(account);
    printf("Deposit successful! Your current balance is %d\n", account->balance);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// Function to withdraw money
void *withdraw(void *arg) {
    Account *account = (Account *)arg;
    int amount;
    printf("Enter the amount to withdraw: ");
    scanf("%d", &amount);

    pthread_mutex_lock(&mutex);
    if (amount > account->balance) {
        printf("Insufficient funds!\n");
    } else {
        account->balance -= amount;
        save_balance(account);
        printf("Withdrawal successful! Your current balance is %d\n", account->balance);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    Account account;
    pthread_mutex_init(&mutex, NULL);
    load_balance(&account);

    int choice;
    while (1) {
        printf("\nSelect an option:\n");
        printf("1. Check Balance\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        pthread_t tid;
        switch (choice) {
            case 1:
                pthread_create(&tid, NULL, check_balance, &account);
                pthread_join(tid, NULL);
                break;
            case 2:
                pthread_create(&tid, NULL, deposit, &account);
                pthread_join(tid, NULL);
                break;
            case 3:
                pthread_create(&tid, NULL, withdraw, &account);
                pthread_join(tid, NULL);
                break;
            case 4:
                printf("Exiting. Thank you!\n");
                pthread_mutex_destroy(&mutex);
                return 0;
            default:
                printf("Invalid option. Try again.\n");
        }
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
