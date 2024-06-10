import socket
import random
import time
import RPi.GPIO as GPIO
def send_continuous_random_numbers():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_>    server_socket.bind(('192.168.4.1', 12345))  # Replace '0.0>    server_socket.listen()

    print("Server listening...")

    client_socket, client_address = server_socket.accept()
    print(f"Connection from {client_address}")
if GPIO.input(10) == GPIO.HIGH:
    print("Button was pushed!")
    while True:
      if GPIO.input(11) == GPIO.HIGH:
        random_number = str(random.randint(1, 1000))  # Genera>        print(random_number)
        client_socket.sendall(random_number.encode())
        time.sleep(0.4)  # Adjust the delay as needed

    client_socket.close()
    server_socket.close()

if _name_ == "_main_":
    send_continuous_random_numbers()