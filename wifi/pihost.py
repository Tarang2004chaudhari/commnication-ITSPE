import socket
import random
import time
import RPi.GPIO as GPIO

SERVER_IP = '192.168.4.1'
SERVER_PORT = 12345

# GPIO setup
BUTTON_PIN_START = 17  # Button to start communication
BUTTON_PIN_SEND = 27   # Button to send a random number
LED_PIN = 22           # Optional: LED to indicate activity

GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_PIN_START, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BUTTON_PIN_SEND, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(LED_PIN, GPIO.OUT)

def wait_for_button_press(button_pin):
    while GPIO.input(button_pin) == GPIO.HIGH:
        time.sleep(0.1)  # Debounce delay

def start_communication():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((SERVER_IP, SERVER_PORT))
        server_socket.listen(1)
        print(f"Server listening on {SERVER_IP}:{SERVER_PORT}")
        
        while True:
            wait_for_button_press(BUTTON_PIN_START)
            print("Start button pressed, waiting for connection...")
            conn, addr = server_socket.accept()
            with conn:
                print('Connected by', addr)
                GPIO.output(LED_PIN, GPIO.HIGH)  # Turn on LED
                
                try:
                    while True:
                        if GPIO.input(BUTTON_PIN_SEND) == GPIO.LOW:
                            random_number = random.randint(1, 100)
                            conn.sendall(str(random_number).encode('utf-8'))
                            print(f"Sent: {random_number}")
                            time.sleep(0.2)  # Avoid bouncing
                except Exception as e:
                    print(f"Connection error: {e}")
                finally:
                    GPIO.output(LED_PIN, GPIO.LOW)  # Turn off LED
                    conn.close()
                    print("Connection closed")

def main():
    try:
        start_communication()
    except KeyboardInterrupt:
        print("Server stopped by user")
    finally:
        GPIO.cleanup()

if _name_ == "_main_":
    main()