#include <stdio.h>
#include <string.h>

int main() {
    char receive_message[] = "key=value=another"; // Example string
    char *saveptr; // Pointer for strtok_r
    char *token;

    // Tokenize the string
    token = strtok_r(receive_message, "=", &saveptr);
    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok_r(NULL, "=", &saveptr); // Pass NULL to continue tokenizing
    }

    return 0;
}
