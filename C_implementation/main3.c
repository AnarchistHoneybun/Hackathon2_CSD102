#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define MAX_USERS 100

struct user {
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

struct user users[MAX_USERS];
int num_users = 0;

int encrypt(char *plaintext, char *key) {
  int i;
  for (i = 0; i < strlen(plaintext); i++) {
    plaintext[i] = plaintext[i] ^ key[i % strlen(key)];
  }
  return 0;
}

int add_user(char *username, char *password) {
  if (num_users >= MAX_USERS) {
    printf("Error: Maximum number of users reached.\n");
    return -1;
  }
  strcpy(users[num_users].username, username);
  encrypt(password, "secret_key");
  strcpy(users[num_users].password, password);
  num_users++;
  printf("User %s added successfully.\n", username);
  return 0;
}

int authenticate(char *username, char *password) {
  int i;
  for (i = 0; i < num_users; i++) {
    if (strcmp(users[i].username, username) == 0) {
      char temp_password[MAX_PASSWORD_LENGTH];
      strcpy(temp_password, password);
      encrypt(temp_password, "secret_key");
      if (strcmp(users[i].password, temp_password) == 0) {
        printf("User %s authenticated successfully.\n", username);
        return 0;
      } else {
        printf("Error: Incorrect password for user %s.\n", username);
        return -1;
      }
    }
  }
  printf("Error: User %s not found.\n", username);
  return -1;
}

int main() {
  printf("Program start?");
  add_user("Alice", "password123");
  printf("Program middle?");
  add_user("Bob", "password456");
  printf("Program middle?");
  authenticate("Alice", "password123");
  printf("Program middle?");
  authenticate("Bob", "password123");
  printf("Program middle?");
  authenticate("Eve", "password789");
  printf("Program end?");
  return 0;
}
