#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
    char password[100];
    printf("Enter your password: ");
    fgets(password, 100, stdin);
    password[strcspn(password, "\n")] = '\0'; // remove newline character from input
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash);
    
    printf("Hashed value of '%s': ", password);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
    
    return 0;
}
