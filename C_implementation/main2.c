#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
//#include <crypt.h>

#define MAX_USERS 100
#define MAX_ATTEMPTS 5
#define RATE_LIMIT_TIME 60

typedef struct {
    char username[32];
    char hash[128];
} User;

User users[MAX_USERS] = {
    {"alice", "$6$ZIHuJ7VH$w15Kj7V62aChMvRV4f4Jpi4O4gg/zBllRbF5JvgS83OtzY81CGzOqxrknQ5yw.uAJf/iYwytCg7PpGyIsXN7V0"},
    {"bob", "$6$nnpD1iq5$5Y5qDAaF8.0oEzXoM/QpMfZc4f1v4m4zRcqswvlYavFzW8BvCwsr/K.tKM2qJq3dO8bZ6UJ.6HcAdzCx.ryW0"},
    {"charlie", "$6$hxjsa68s$l8ehvT18xNmycJeTHY.TwW/E56inIvNgs9X1L30iZaWUNP8TlZIiGBZH10YpY.rW.vM8rMEmr9X2iCuv3bg6d0"}
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
    char *result = crypt(password, hash);
    if (result == NULL) {
        return false;
    }
    return strcmp(result, hash) == 0;
}

void login() {
    char username[32];
    char password[32];
    int num_attempts = 0;
    time_t last_attempt_time = 0;
    
    printf("Username: ");
    scanf("%s", username);
    
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
        scanf("%s", password);
        
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
       
