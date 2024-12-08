#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>

#define UART_DEVICE "/dev/ttyAMA0"
#define BAUD_RATE B9600

int uart_fd = -1;

int uart_init(const char* device) {
    uart_fd = open(device, O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Error opening UART device");
        return -1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);
    options.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    return 0;
}

ssize_t uart_read(int fd, void* buf, size_t size, int timeout_sec) {
    fd_set read_fds;
    struct timeval timeout;
    ssize_t total_read = 0;
    ssize_t bytes_read;
    char* buffer = (char*)buf;

    while (total_read < size) {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = 0;

        int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno == EINTR) {
                continue; // Interrupted, retry
            }
            perror("select failed");
            return -1;
        } else if (ret == 0) {
            break; // Timeout
        }

        if (FD_ISSET(fd, &read_fds)) {
            bytes_read = read(fd, buffer + total_read, size - total_read);
            if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                    continue; // Non-blocking or interrupted, retry
                }
                perror("UART read failed");
                return -1;
            } else if (bytes_read == 0) {
                fprintf(stderr, "Device reports readiness to read but returned no data (device disconnected or multiple access on port?)\n");
                return -1;
            }
            total_read += bytes_read;
        }
    }

    return total_read;
}

int main(int argc, char *argv[]) {
    if (uart_init(UART_DEVICE) < 0) {
        return 1;
    }

    char send_data[256];
    snprintf(send_data, sizeof(send_data), "AT\r\n");

    if (write(uart_fd, send_data, strlen(send_data)) < 0) {
        perror("UART write failed");
        close(uart_fd);
        return 1;
    }
    printf("Sent: %s\n", send_data);

    char received_data[1024];
    ssize_t bytes_read = uart_read(uart_fd, received_data, sizeof(received_data) - 1, 5);
    if (bytes_read > 0) {
        received_data[bytes_read] = '\0';
        printf("Received: %s\n", received_data);
    } else {
        printf("No data received or read failed\n");
    }

    close(uart_fd);
    return 0;
}