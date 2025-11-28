/*******************************************************************
 *  ROCKVIEW UNIVERSITY - TERM 3 FINAL PROJECT
 *  Course: C Programming
 *  Student: Emmanuel Lundu Nkole
 *  Student No: 2025013165
 *  
 *  Project: Perkins Finances - Online Banking System (Zambian Kwacha)
 *  Currency: ZMW (Kwacha)
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ACCOUNTS 100
#define FILENAME "perkins_accounts.dat"
#define BANK_NAME "Perkins Finances"

// Structure for each customer
typedef struct {
    char name[100];
    char acc_number[20];
    char password[20];
    double balance;
} Account;

// Global variables
Account accounts[MAX_ACCOUNTS];
int total_accounts = 0;
int logged_in_index = -1;

// Function prototypes
void clearScreen();
void pressEnter();
int isUniqueAccount(char acc[]);
void registerUser();
void login();
void mainMenu();
void deposit();
void withdraw();
void transfer();
void changePassword();
void viewBalance();
void saveToFile();
void loadFromFile();
int validateAmount(double amount);
void welcomeArt();

int main() {
    loadFromFile();
    welcomeArt();

    int choice;
    do {
        printf("\n\t\t=== PERKINS FINANCES ===\n");
        printf("\t\t   Zambian Online Banking\n\n");
        printf("1. Register New Account\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: registerUser(); break;
            case 2: login(); break;
            case 3: 
                printf("\n\tZikomo! Come bank with Perkins again!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Try again.\n");
                pressEnter();
        }
    } while(1);

    return 0;
}

// ASCII Welcome Art
void welcomeArt() {
    clearScreen();
    printf("\n\n");
    printf("    ╔════════════════════════════════╗\n");
    printf("    ║    PERKINS FINANCES            ║\n");
    printf("    ║    Zambia's No.1 Digital Bank  ║\n");
    printf("    ╚════════════════════════════════╝\n");
    printf("    Proudly serving Zambians since 2025\n\n");
    pressEnter();
}

// Registration
void registerUser() {
    clearScreen();
    printf("\n=== REGISTER NEW ACCOUNT ===\n");

    Account newAcc;

    printf("Enter Full Name: ");
    getchar(); // clear buffer
    fgets(newAcc.name, sizeof(newAcc.name), stdin);
    newAcc.name[strcspn(newAcc.name, "\n")] = 0;

    do {
        printf("Enter Account Number (e.g. PF1001): ");
        scanf("%s", newAcc.acc_number);
        if (!isUniqueAccount(newAcc.acc_number)) {
            printf("Account number already exists! Try another.\n");
        }
    } while (!isUniqueAccount(newAcc.acc_number));

    printf("Set Password (max 19 chars): ");
    scanf("%s", newAcc.password);

    printf("Initial Deposit (ZMW): ");
    scanf("%lf", &newAcc.balance);
    while (newAcc.balance < 50) {
        printf("Minimum initial deposit is K50. Enter again: ");
        scanf("%lf", &newAcc.balance);
    }

    accounts[total_accounts++] = newAcc;
    saveToFile();

    printf("\nCongratulations %s!\n", newAcc.name);
    printf("Account created successfully!\n");
    printf("Your Account Number: %s\n", newAcc.acc_number);
    printf("Balance: ZMW %.2lf\n", newAcc.balance);
    pressEnter();
}

// Login system
void login() {
    clearScreen();
    char acc[20], pass[20];
    printf("\n=== LOGIN TO PERKINS FINANCES ===\n");
    printf("Account Number: ");
    scanf("%s", acc);
    printf("Password: ");
    scanf("%s", pass);

    for (int i = 0; i < total_accounts; i++) {
        if (strcmp(accounts[i].acc_number, acc) == 0 && strcmp(accounts[i].password, pass) == 0) {
            logged_in_index = i;
            printf("\nLogin successful! Welcome back, %s!\n", accounts[i].name);
            pressEnter();
            mainMenu();
            return;
        }
    }
    printf("\nWrong account number or password!\n");
    pressEnter();
}

// Main dashboard after login
void mainMenu() {
    int choice;
    while(1) {
        clearScreen();
        printf("\n=== PERKINS FINANCES - %s ===\n", accounts[logged_in_index].name);
        printf("Account: %s     |     Balance: ZMW %.2lf\n\n", accounts[logged_in_index].acc_number, accounts[logged_in_index].balance);

        printf("1. Deposit Money\n");
        printf("2. Withdraw Money\n");
        printf("3. Transfer Funds\n");
        printf("4. View Account Details\n");
        printf("5. Change Password\n");
        printf("6. Logout\n");
        printf("\nChoose option: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: deposit(); break;
            case 2: withdraw(); break;
            case 3: transfer(); break;
            case 4: viewBalance(); break;
            case 5: changePassword(); break;
            case 6: 
                logged_in_index = -1;
                printf("\nLogged out successfully!\n");
                pressEnter();
                return;
            default:
                printf("Invalid option!\n");
                pressEnter();
        }
    }
}

void deposit() {
    double amount;
    printf("\nEnter amount to deposit (ZMW): ");
    scanf("%lf", &amount);
    if (amount <= 0) {
        printf("Amount must be positive!\n");
    } else {
        accounts[logged_in_index].balance += amount;
        saveToFile();
        printf("Deposit successful!\n");
        printf("New balance: ZMW %.2lf\n", accounts[logged_in_index].balance);
    }
    pressEnter();
}

void withdraw() {
    double amount;
    printf("\nEnter amount to withdraw (ZMW): ");
    scanf("%lf", &amount);
    if (amount <= 0) {
        printf("Amount must be positive!\n");
    } else if (amount > accounts[logged_in_index].balance) {
        printf("Insufficient funds! Balance: ZMW %.2lf\n", accounts[logged_in_index].balance);
    } else {
        accounts[logged_in_index].balance -= amount;
        saveToFile();
        printf("Withdrawal successful!\n");
        printf("New balance: ZMW %.2lf\n", accounts[logged_in_index].balance);
    }
    pressEnter();
}

void transfer() {
    char target[20];
    double amount;
    printf("\nEnter recipient account number: ");
    scanf("%s", target);
    printf("Enter amount to transfer (ZMW): ");
    scanf("%lf", &amount);

    if (amount <= 0 || amount > accounts[logged_in_index].balance) {
        printf("Invalid amount or insufficient balance!\n");
        pressEnter();
        return;
    }

    int found = -1;
    for (int i = 0; i < total_accounts; i++) {
        if (strcmp(accounts[i].acc_number, target) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Recipient account not found!\n");
    } else if (found == logged_in_index) {
        printf("You cannot transfer to yourself!\n");
    } else {
        accounts[logged_in_index].balance -= amount;
        accounts[found].balance += amount;
        saveToFile();
        printf("Transfer successful!\n");
        printf("ZMW %.2lf sent to %s\n", amount, accounts[found].name);
        printf("Your new balance: ZMW %.2lf\n", accounts[logged_in_index].balance);
    }
    pressEnter();
}

void viewBalance() {
    clearScreen();
    printf("\n=== ACCOUNT DETAILS ===\n");
    printf("Bank: %s\n", BANK_NAME);
    printf("Name: %s\n", accounts[logged_in_index].name);
    printf("Account Number: %s\n", accounts[logged_in_index].acc_number);
    printf("Current Balance: ZMW %.2lf\n", accounts[logged_in_index].balance);
    printf("\nPassword: ********** (hidden for security)\n");
    pressEnter();
}

void changePassword() {
    char old[20], newp[20];
    printf("\nEnter current password: ");
    scanf("%s", old);
    if (strcmp(old, accounts[logged_in_index].password) != 0) {
        printf("Wrong password!\n");
    } else {
        printf("Enter new password: ");
        scanf("%s", newp);
        strcpy(accounts[logged_in_index].password, newp);
        saveToFile();
        printf("Password changed successfully!\n");
    }
    pressEnter();
}

// Check if account number already exists
int isUniqueAccount(char acc[]) {
    for (int i = 0; i < total_accounts; i++) {
        if (strcmp(accounts[i].acc_number, acc) == 0) return 0;
    }
    return 1;
}

// File handling - save all accounts
void saveToFile() {
    FILE *fp = fopen(FILENAME, "wb");
    if (fp == NULL) {
        printf("Error saving data!\n");
        return;
    }
    fwrite(&total_accounts, sizeof(int), 1, fp);
    fwrite(accounts, sizeof(Account), total_accounts, fp);
    fclose(fp);
}

// Load accounts from file
void loadFromFile() {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) return; // First run, no file yet

    fread(&total_accounts, sizeof(int), 1, fp);
    fread(accounts, sizeof(Account), total_accounts, fp);
    fclose(fp);
}

void clearScreen() {
    system("clear || cls");
}

void pressEnter() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}
