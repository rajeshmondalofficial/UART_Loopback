#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main() {
    int uart_fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("UART open failed");
        return 1;
    }

    // Configure UART settings
    struct termios options;
    tcgetattr(uart_fd, &options);

    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Baud rate: 9600, 8 data bits, no parity, 1 stop bit
    options.c_iflag = IGNPAR;                       // Ignore framing errors
    options.c_oflag = 0;
    options.c_lflag = 0; // Non-canonical mode

    tcflush(uart_fd, TCIFLUSH);            // Flush the input buffer
    tcsetattr(uart_fd, TCSANOW, &options); // Apply the configuration


    char buffer[BUFFER_SIZE];
    int index = 0;

    printf("Reading UART data...\n");
    while (1) {
        fd_set read_fds;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_SET(uart_fd, &read_fds);

        timeout.tv_sec = 2;  // Timeout of 2 seconds
        timeout.tv_usec = 0;

        if (select(uart_fd + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(uart_fd, &read_fds)) {
                char temp;
                if (read(uart_fd, &temp, 1) > 0) {
                    if (temp == '\n') { // End of packet
                        buffer[index] = '\0';
                        printf("Received packet: %s\n", buffer);
                        index = 0;
                    } else if (index < BUFFER_SIZE - 1) {
                        buffer[index++] = temp;
                    }
                }
            }
        } else {
            printf("No data received within timeout\n");
        }
    }

    close(uart_fd);
    return 0;
}
