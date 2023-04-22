#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <bcrypt.h>

#define MAX_USERS 100
#define MAX_ATTEMPTS 5
#define RATE_LIMIT_TIME 60

typedef struct {
    char username[32];
    char hash[64];
} User;

User users[MAX_USERS] = {
    {"alice", "$2b$10$OgLV1meFvy8Cfz.jnYbKpepBm6egbXH.kmIF2i.P1CjKZ2Mw8SdDS"},
    {"bob", "$2b$10$3X9UWjKu.7VmtfQzFw67VeWAdzyJjPJ6TgT.e3sU9XW16hjOuV7sK"},
    {"charlie", "$2b$10$rlKpDxHb51X8M9QcB0C1/OhyFZf./DZjK2gYt.s1SGmH1c5CKXn96"}
};

int num_users = 3;

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

bool verify_password(char *password, char *hash) {
    return bcrypt_checkpw(password, hash) == 0;
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
        if (rate_limit_exceeded(last_attempt_time)) {
            printf("Too many failed attempts. Please wait %d seconds before trying again.\n",
                   (int)(RATE_LIMIT_TIME - (time(NULL) - last_attempt_time)));
            return;
        }
        
        printf("Password: ");
        fgets(password, 32, stdin);
        password[strcspn(password, "\n")] = '\0';  // remove newline character
        
        if (verify_password(password, user->hash)) {
            printf("Welcome, %s!\n", user->username);
            return;
        } else {
            num_attempts++;
            last_attempt_time = time(NULL);
            printf("Incorrect password. %d of %d attempts remaining.\n", MAX_ATTEMPTS - num_attempts, MAX_ATTEMPTS);
            if (num_attempts >= MAX_ATTEMPTS) {
                printf("Too many failed attempts. Please wait %d seconds before trying again.\n", RATE_LIMIT_TIME);
                return;
            }
        }
    }
}

int main() {
    login();
    return 0;
}
