import serial
import time

# Configuration
PORT = '/dev/serial0'
BAUD_RATE = 9600
Tid = 21  # Example Timer ID
start_byte = 0xFA
end_byte = 0xFB

# Initialize serial communication
ser = serial.Serial(PORT, BAUD_RATE)

def calculate_checksum(Tid, a, b, c):
    return (Tid + 4 + 3 + a + b + c) % 10

def send_data(Tid, a, b, c):
    chk = calculate_checksum(Tid, a, b, c)
    message = [start_byte, Tid, a, b, c, chk, end_byte]
    ser.write(bytearray(message))
    print(f"Sent: {[hex(x) for x in message]}")

try:
    while True:
        # Example user inputs
        a = int(input("Enter first digit (a): "))
        b = int(input("Enter second digit (b): "))
        c = int(input("Enter third digit (c): "))
        send_data(Tid, a, b, c)
        time.sleep(2)

except KeyboardInterrupt:
    ser.close()
    print("Exiting...")