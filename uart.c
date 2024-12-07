#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_DEVICE "/dev/ttyAMA0" // For Raspberry Pi's default UART

int main()
{
    // Open the UART device
    int uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
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

    // Send data
    char write_buffer[] = "AT+SEND=8,6,Hello";
    char termination[] = {0x0D, 0x0A};
    int bytes_written = write(uart_fd, write_buffer, sizeof(write_buffer));
    // int bytes_written1 = write(uart_fd, termination, sizeof(termination));
    if (bytes_written < 0)
    {
        perror("Failed to write to UART");
        close(uart_fd);
        return -1;
    }
    printf("Sent: %s\n", write_buffer);

    int dataReady = -1;
    bool reading = false;
    char data[1024];
    while (1)
    {
        // Read data
        char read_buffer[256];
        int bytes_read = read(uart_fd, read_buffer, 256);
        if (dataReady < 50)
        {
            printf("Bytes Read: %d\n", bytes_read);
            dataReady++;
        }
        usleep(10000);

        // When bytes read more than 0 make the reading flag true
        if (bytes_read > 0)
        {
            reading = true;
            read_buffer[bytes_read] = '\0';
            strcat(data, read_buffer);
        }
        else
        {
            if (reading)
            {
                dataReady = 1;
                reading = false
            }
            else
            {
                dataReady = -1;
                data[0] = '\0';
            }
        }
        if (dataReady > 0)
        {
            printf("Received: %s\n", data);
        }

        // if (bytes_read > 0)
        // {
        //     read_buffer[bytes_read] = '\0';
        //     strcat(data, read_buffer);
        //     dataReady = -1;
        // }
        // else
        // {
        //     if (strlen(data) > 0)
        //     {
        //         dataReady = 1;
        //     }
        // }

        // if (dataReady > 0)
        // {
        //     // printf("Received: %s\n", data);
        //     dataReady = -1;
        //     data[0] = '\0';
        // }
    }

    // Close UART
    close(uart_fd);

    return 0;
}
