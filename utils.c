#include <stdio.h>
#include <string.h>

char* receive_message = "+RCV=50,5,Hello,-99,40";

char *saveptr;
char *token;

// Tokenize the string
token = strtok_r(receive_message, "=", &saveptr);

printf("%s", token);