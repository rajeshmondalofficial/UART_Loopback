import serial
import sys
# Configure the serial port
port = "/dev/ttyAMA0"  # Replace with the correct port for your setup (e.g., COM3 on Windows)
baud_rate = 9600

# Initialize the serial connection
ser = serial.Serial(port, baud_rate, timeout=1)

try:
    # Data to send
    send_data = f"{sys.argv[1]}\r\n"

    # Write data to UART
    ser.write(send_data)
    print(f"Sent: {send_data}")

    # Read the data back
    while True:
        received_data = ser.read(1024).decode('utf-8')
        if received_data:
            print(f"Received: {received_data}")
except KeyboardInterrupt:
    print("\nInterrupted by user. Exiting...")
finally:
    # Close the serial connection
    ser.close()
