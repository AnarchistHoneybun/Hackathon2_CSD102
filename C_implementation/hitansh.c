#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <time.h> 
#include <ctype.h> 


#define MAX_USERS 100
#define MAX_ADMINS 5
#define MAX_ATTEMPTS 5
#define RATE_LIMIT_TIME 5
#define SHIFT 3



typedef struct {
    char username[32];
    char hash[64];
} User;


User users[MAX_USERS] = {
    {"alice", "sdvvzrug1"},
    {"bob", "sdvvzrug2"},
    {"charlie", "sdvvzrug3"}
};


User admins[MAX_ADMINS] = {
    {"sudo", "froxper"},
    {"root", "exfnvkrw"}
};

int num_users = 3; 
int num_admins = 2;


bool rate_limit_exceeded(time_t last_attempt_time) {
    time_t now = time(NULL);
    return now < last_attempt_time + RATE_LIMIT_TIME;
}



User *find_user_by_username(char *username) { 
    for (int i = 0; i < num_users; i++) { 
        if (strcmp(username, users[i].username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

User *find_admin_by_username(char *username) {
    for (int i = 0; i < num_admins; i++) {
        if (strcmp(username, admins[i].username) == 0) {
            return &admins[i];
        }
    }
    return NULL;
}


void caesar_cipher(char *string, int shift) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (isalpha(string[i])) {
            if (islower(string[i])) {
                string[i] = (string[i] - 'a' + shift) % 26 + 'a';
            } else {
                string[i] = (string[i] - 'A' + shift) % 26 + 'A';
            }
        }
    }
}


bool verify_password(char *password, char *hash) {
    return strcmp(hash, password) == 0;
}


void login() {
    char username[32];
    char password[32];
    int num_attempts = 0;
    time_t last_attempt_time = 0;
    
    printf("Username: ");
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = '\0';  // remove newline character
    
    User *user = find_user_by_username(username);
    if (user == NULL) {
        printf("Invalid username\n");
        return;
    }
    
    while (true) {
        
        num_attempts = 0;

passentry: 
        printf("Password: ");
        fgets(password, 32, stdin);
        password[strcspn(password, "\n")] = '\0';  // remove newline character
        
        caesar_cipher(password, SHIFT);
        if (verify_password(password, user->hash)) {
            printf("Welcome, %s!\n", user->username);
            printf("Do you want to change your password? (y/n): ");
            char choice;
            scanf("%c", &choice);
            if (choice == 'y') {
                printf("Enter new password: ");
                scanf("%s", password);
                caesar_cipher(password, SHIFT);
                strcpy(user->hash, password); //>takes input, encrypts it, and rewrites the current password
                printf("Password changed successfully!\n");
                printf(".\n.\n.\nExiting...\n");
                fflush(stdin); //>flushing the standard input buffer so that it doesn't overflow to the next input in main                
            }else{
                printf("Exiting...\n");
                fflush(stdin);
            }
            return;
        } else {
            num_attempts++;
            last_attempt_time = time(NULL);
            printf("Incorrect password. %d of %d attempts remaining.\n", MAX_ATTEMPTS - num_attempts, MAX_ATTEMPTS);
            if(num_attempts < MAX_ATTEMPTS){
                printf("Please try again.\n");
                goto passentry;
            }
            if (num_attempts >= MAX_ATTEMPTS) {
                printf("Too many failed attempts. User %s has been locked.\n", user->username);
                return;
            }
        }
    }
}

void adminLogin() {
    char username[32];
    char password[32];
    int num_attempts = 0;
    time_t last_attempt_time = 0;
    
    printf("admin_tag: ");
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = '\0';  // remove newline character
    
    User *admin = find_admin_by_username(username);
    if (admin == NULL) {
        printf("Unauthorized\n");
        return;
    }
    
    while (true) {
                
        num_attempts = 0;
adminpass: 
        printf("Password: ");
        fgets(password, 32, stdin);
        password[strcspn(password, "\n")] = '\0';  // remove newline character
        
        caesar_cipher(password, SHIFT);
        if (verify_password(password, admin->hash)) {
            printf("Welcome, %s!\n", admin->username);
            printf("Access user database? (y/n): "); //>prints out all the usernames in the database

            char choice;
            scanf("%c", &choice);
            if (choice == 'y') {
                printf("User database:\n");
                for (int i = 0; i < num_users; i++) {
                    printf("%s  \n", users[i].username);
                }
                printf(".\n.\n.\nExiting...\n");
                fflush(stdin);
                return;
            }else{
                printf("Exiting...\n");
                fflush(stdin);
                return;
            }

        } else {
            num_attempts++;
            last_attempt_time = time(NULL);
            printf("Incorrect password. %d of %d attempts remaining.\n", MAX_ATTEMPTS - num_attempts, MAX_ATTEMPTS);
            if(num_attempts < MAX_ATTEMPTS){
                printf("Please try again.\n");
                goto adminpass;
            }
            if (num_attempts >= MAX_ATTEMPTS) {
                printf("Too many failed attempts. Admin %s has been locked.\n",admin->username);
                return;
            }
        }
    }
}


//> function to signup
void signup() {
    char username[32];
    char password[32];
    
    while (true) {
        printf("Enter a username: ");
        fgets(username, sizeof(username), stdin);
        // Remove trailing newline character from the username
        username[strcspn(username, "\n")] = '\0';

        if (find_user_by_username(username) != NULL) {
            printf("That username is already taken.\n");
        } else {
            break;
        }
    }

    printf("Enter a password: ");
    fgets(password, sizeof(password), stdin);
    // Remove trailing newline character from the password
    password[strcspn(password, "\n")] = '\0';
    
    // Encrypt the password with a simple Caesar cipher
    caesar_cipher(password, SHIFT);

    // Add the user to the array of users
    if (num_users >= MAX_USERS) {
        printf("Cannot add more users.\n");
        return;
    }
    strcpy(users[num_users].username, username);
    strcpy(users[num_users].hash, password);
    num_users++;
    
    printf("User %s created successfully.\n", username);
}


//> main function
int main() {
    

    //> menu based login/signup
    
    
    char choice[32];

    printf("Welcome to the login/signup system!\n");
    printf("Your options are:\n");
    printf("1. Login\n2. SignUp\n3. Exit\n4. Clear Screen(cls)\n5.Admin Login(admlog)\n");

    //* input is taken in natural language- login, signup, exit, cls, admlog

    while(true){

        printf("->");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = '\0';

        if(strcmp(choice, "login") == 0){
            login();
            continue;
        }
        else if(strcmp(choice, "signup") == 0){
            signup();
            continue;
        }
        else if(strcmp(choice, "exit") == 0){
            printf("Exiting...\n");
            break;
        }
        else if(strcmp(choice, "admlog") == 0){
            adminLogin();
            continue;
        }
        else if(strcmp(choice, "cls") == 0){
            system("cls");
            continue;
        }
        else{
            printf("Invalid choice. Please try again.\n");
            continue;
        }

    }    
    
    
    return 0;
}
