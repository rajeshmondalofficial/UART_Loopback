#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_DEVICE "/dev/serial0" // For Raspberry Pi's default UART

int main() {
    // Open the UART device
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Failed to open UART");
        return -1;
    }
    
    // Configure UART settings
    struct termios options;
    tcgetattr(uart_fd, &options);
    
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Baud rate: 9600, 8 data bits, no parity, 1 stop bit
    options.c_iflag = IGNPAR;                      // Ignore framing errors
    options.c_oflag = 0;
    options.c_lflag = 0;                           // Non-canonical mode

    tcflush(uart_fd, TCIFLUSH);                    // Flush the input buffer
    tcsetattr(uart_fd, TCSANOW, &options);         // Apply the configuration

    // Send data
    char write_buffer[] = "Hello, UART Loopback Test!";
    int bytes_written = write(uart_fd, write_buffer, sizeof(write_buffer));
    if (bytes_written < 0) {
        perror("Failed to write to UART");
        close(uart_fd);
        return -1;
    }
    printf("Sent: %s\n", write_buffer);

    // Read data
    char read_buffer[256];
    int bytes_read = read(uart_fd, read_buffer, sizeof(read_buffer));
    if (bytes_read < 0) {
        perror("Failed to read from UART");
        close(uart_fd);
        return -1;
    }

    read_buffer[bytes_read] = '\0'; // Null-terminate the string
    printf("Received: %s\n", read_buffer);

    // Close UART
    close(uart_fd);

    return 0;
}
