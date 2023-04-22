#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100

// Define a struct to represent a user
typedef struct {
    char username[20];
    char hashed_password[20];
} User;

// Define a Caesar cipher function that shifts characters by a given amount
char* caesar_cipher(char* plaintext, int shift) {
    char* ciphertext = strdup(plaintext); // Duplicate the plaintext string
    int length = strlen(ciphertext);
    for (int i = 0; i < length; i++) {
        if (ciphertext[i] >= 'a' && ciphertext[i] <= 'z') { // Shift lowercase letters
            ciphertext[i] = 'a' + ((ciphertext[i] - 'a' + shift) % 26);
        }
        else if (ciphertext[i] >= 'A' && ciphertext[i] <= 'Z') { // Shift uppercase letters
            ciphertext[i] = 'A' + ((ciphertext[i] - 'A' + shift) % 26);
        }
    }
    return ciphertext;
}

// Define a function to hash passwords using a Caesar cipher
char* hash_password(char* password) {
    return caesar_cipher(password, 3); // Shift each character by 3 positions
}

// Define a function to check if a password matches a hashed password
int verify_password(char* password, char* hashed_password) {
    char* password_hash = hash_password(password);
    int result = strcmp(password_hash, hashed_password) == 0;
    free(password_hash);
    return result;
}

// Define a global array to store registered users
User users[MAX_USERS];
int num_users = 0;

// Define a function to register a new user
void register_user(char* username, char* password) {
    if (num_users < MAX_USERS) {
        strcpy(users[num_users].username, username);
        strcpy(users[num_users].hashed_password, hash_password(password));
        num_users++;
        printf("Registration successful!\n");
    }
    else {
        printf("Maximum number of users reached.\n");
    }
}

int main() {
    char username[20];
    char password[20];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    register_user(username, password);
    printf("Number of registered users: %d\n", num_users);
    for (int i = 0; i < num_users; i++) {
        printf("User %d: %s %s\n", i+1, users[i].username, users[i].hashed_password);
    }
    printf("Login:\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    int user_index = -1;
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            user_index = i;
            break;
        }
    }
    if (user_index >= 0 && verify_password(password, users[user_index].hashed_password)) {
        printf("Login successful!\n");
    }
    else {
        printf("Invalid username or password.\n");
    }
    return 0;
}
