//todo Create a primitive login system

//todo 0. create a menu based system for these actions
    //todo 0.1 login
    //todo 0.2 signup
    //todo 0.3 exit
    //todo 0.4 admin login (hidden)
//todo 1. login system that takes in username and password
    //todo 1.1 implement a rate limiter
    //todo 1.2 implement some inane thing to do after logging in
    //todo 1.3 add a logout option
//todo 2. signup system that takes in username and password
    //todo 2.1 check new username against existing usernames
    //todo 2.2 encrypt password
    //todo 2.3 add new user to the user array
    //todo 2.4 add a new user to the user file (extended functionality, check by end)
//todo 3. implement a rate limiter for the login system
//todo 4. admin login system
    //todo 4.1 keep this primitive, and keep option hidden
    //todo 4.2 allow access to database from admin log


//>include libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

//>defining constants
#define MAX_USERS 100
#define MAX_ADMINS 5
#define MAX_ATTEMPTS 5
#define RATE_LIMIT_TIME 5
#define SHIFT 3


//>user struct, storing username and hashed password
typedef struct {
    char username[32];
    char hash[64];
} User;


//>predefined user array
//todo add an admin login, so will need another array for the admins
User users[MAX_USERS] = {
    {"alice", "$2b$10$OgLV1meFvy8Cfz.jnYbKpepBm6egbXH.kmIF2i.P1CjKZ2Mw8SdDS"},
    {"bob", "$2b$10$3X9UWjKu.7VmtfQzFw67VeWAdzyJjPJ6TgT.e3sU9XW16hjOuV7sK"},
    {"charlie", "$2b$10$rlKpDxHb51X8M9QcB0C1/OhyFZf./DZjK2gYt.s1SGmH1c5CKXn96"}
}; //!recompute this one asap

//> admin database
User admins[MAX_ADMINS] = {
    {"sudo", "froxper"},
    {"root", "exfnvkrw"}
};

int num_users = 3; //>  initial user number
int num_admins = 2; //> number of admins

//> function to implement a rate limiter
bool rate_limit_exceeded(time_t last_attempt_time) {
    time_t now = time(NULL);
    return now < last_attempt_time + RATE_LIMIT_TIME;
}


//> function to find username while logging in
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

//> function to encrypt password using the caesar cipher
//! need to replace shift with a defined constant, or randomize it
// void caesar_cipher(char *str, int shift) {
//     for (int i = 0; i < strlen(str); i++) {
//         str[i] = (str[i] - 'a' + shift) % 26 + 'a';
//     }
// }

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


//> function to verify password when logging in
//! modify this to check against the caesar hash, not from the bcrypt data
bool verify_password(char *password, char *hash) {
    return strcmp(hash, password) == 0;
}


//> function to login
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
        //>  rate limiter check
        if (rate_limit_exceeded(last_attempt_time)) {
            sleep(1);
            printf("Too many failed attempts. Please wait %d seconds before trying again.\n",
                   (int)(RATE_LIMIT_TIME - (time(NULL) - last_attempt_time)));
            continue;
        }
        
passentry: 
        printf("Password: ");
        num_attempts = 0;
        fgets(password, 32, stdin);
        password[strcspn(password, "\n")] = '\0';  // remove newline character
        
        caesar_cipher(password, SHIFT);
        if (verify_password(password, user->hash)) {
            printf("Welcome, %s!\n", user->username);
            return;
        } else {
            num_attempts++;
            last_attempt_time = time(NULL);
            printf("Incorrect password. %d of %d attempts remaining.\n", MAX_ATTEMPTS - num_attempts, MAX_ATTEMPTS);
            if(num_attempts < MAX_ATTEMPTS){
                printf("Please try again.\n");
                goto passentry;
            }
            if (num_attempts > MAX_ATTEMPTS) {
                printf("Too many failed attempts. Please wait %d seconds before trying again.\n", RATE_LIMIT_TIME);
                return;
            }
        }
    }
}

//> function to login the admin
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
        //>  rate limiter check
        if (rate_limit_exceeded(last_attempt_time)) {
            sleep(1);
            printf("Too many failed attempts. Please wait %d seconds before trying again.\n",
                   (int)(RATE_LIMIT_TIME - (time(NULL) - last_attempt_time)));
            continue;
        }
        
adminpass: 
        printf("Password: ");
        num_attempts = 0;
        fgets(password, 32, stdin);
        password[strcspn(password, "\n")] = '\0';  // remove newline character
        
        caesar_cipher(password, SHIFT);
        if (verify_password(password, admin->hash)) {
            printf("Welcome, %s!\n", admin->username);
            printf("Access user database? (y/n): ");

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
            if (num_attempts > MAX_ATTEMPTS) {
                printf("Too many failed attempts. Please wait %d seconds before trying again.\n", RATE_LIMIT_TIME);
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


int main() {
    

    //> menu based login/signup

    char choice[32];

    printf("Welcome to the login/signup system!\n");
    printf("Your options are:\n");
    printf("1. Login\n2. SignUp\n3. Exit\n");

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
        else{
            printf("Invalid choice. Please try again.\n");
            continue;
        }

    }    
    
    
    //! debugging data
    // //>  print out the users for debugging
    // printf("original users:\n");
    // for (int i = 0; i < num_users; i++) {
    //     printf("%s %s\n", users[i].username, users[i].hash);
    // }

    // signup();

    // //>  print out the users for debugging
    // printf("users after signup operation:\n");
    // for (int i = 0; i < num_users; i++) {
    //     printf("%s %s\n", users[i].username, users[i].hash);
    // }
    // sleep(2);
    // login();
    return 0;
}
