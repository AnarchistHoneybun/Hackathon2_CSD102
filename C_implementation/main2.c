#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
    unsigned char *input_data = "Hello, world!";
    unsigned char hash_value[SHA256_DIGEST_LENGTH];

    SHA256(input_data, strlen(input_data), hash_value);

    printf("SHA-256 hash of \"%s\":\n", input_data);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash_value[i]);
    }
    printf("\n");

    return 0;
}
