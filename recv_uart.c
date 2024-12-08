#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define UART_DEVICE "/dev/ttyAMA0"
#define BAUD_RATE B9600

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <data_to_send>\n", argv[0]);
        return 1;
    }

    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Error opening UART device");
        return 1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    char send_data[256];
    snprintf(send_data, sizeof(send_data), "%s\r\n", argv[1]);

    int bytes_written = write(uart_fd, send_data, strlen(send_data));
    if (bytes_written < 0) {
        perror("UART write failed");
        close(uart_fd);
        return 1;
    }
    printf("Sent: %s\n", send_data);

    char received_data[1024];
    char data[1024];
    while (1) {
        int bytes_read = read(uart_fd, received_data, sizeof(received_data) - 1);
        if (bytes_read < 0) {
            perror("UART read failed");
            break;
        } else if (bytes_read > 0) {
            printf("Received RAW Bytes: %c\n", received_data);
            received_data[bytes_read] = '\0';
            
            strcat(data, received_data);
            // printf("Received: %s\n", data);
            // printf("Hexadecimal (Last): %X\n", data[strlen(data)-1]);
            // printf("String (Last): %c\n", data[strlen(data)-1]);
            if(data[strlen(data) -1] == 0xA) {
                printf("Received Full: %s\n", data);
                data[0] = '\0';
            }
        }

        // size_t len = strlen(data);
        // if(len > 0 && data[len - 1] == '\n') {
        //     printf("Received After Cleanup: %s\n", received_data);
        //     data[0] = '\0';
        // }
    }

    close(uart_fd);
    return 0;
}