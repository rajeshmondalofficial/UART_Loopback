#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define UART_DEVICE "/dev/ttyAMA0" // For Raspberry Pi's default UART

int main()
{
    // Open the UART device
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd == -1)
    {
        perror("Failed to open UART");
        return -1;
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


    printf("Listening on port %d\n", uart_fd);
    int dataReady = -1;
    bool reading = false;
    char data[1024];
    while (1)
    {
        // Read data
        char read_buffer[256];
        int bytes_read = read(uart_fd, read_buffer, 256);

        // usleep(10000);

        // When bytes read more than 0 make the reading flag true
        if (bytes_read > 0)
        {
            reading = true;
            read_buffer[bytes_read] = '\0';
            // strcat(data, read_buffer);
            printf("Receive: %s\n", read_buffer);
        }
        // else
        // {
        //     if (reading)
        //     {
        //         dataReady = 1;
        //         reading = false;
        //     }
        //     else
        //     {
        //         dataReady = -1;
        //         data[0] = '\0';
        //     }
        // }
        // if (dataReady > 0)
        // {
        //     printf("Received: %s\n", data);
        // }
    }

    // Close UART
    close(uart_fd);

    return 0;
}
