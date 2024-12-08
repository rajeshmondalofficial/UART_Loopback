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
    char buffer[1024];
    int buffer_index = 0;

    while (1) {
        int bytes_read = read(uart_fd, buffer + buffer_index, sizeof(buffer) - buffer_index - 1);
        if (bytes_read < 0) {
            perror("UART read failed");
            break;
        } else if (bytes_read > 0) {
            buffer_index += bytes_read;
            buffer[buffer_index] = '\0';

            // Check if we have a complete message
            char *newline_pos = strchr(buffer, '\n');
            if (newline_pos != NULL) {
                *newline_pos = '\0'; // Null-terminate the message
                printf("Received: %s\n", buffer);

                // Move remaining data to the beginning of the buffer
                buffer_index = strlen(newline_pos + 1);
                memmove(buffer, newline_pos + 1, buffer_index);
            }
        }
    }

    close(uart_fd);
    return 0;
}