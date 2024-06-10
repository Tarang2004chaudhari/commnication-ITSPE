import time
from digi.xbee.devices import XBeeDevice, RemoteXBeeDevice, XBee64BitAddress
from digi.xbee.exception import TransmitException

# Router XBee device settings
PORT = "/dev/ttyUSB0"
BAUD_RATE = 9600

# Create XBee device object and open the connection
device = XBeeDevice(PORT, BAUD_RATE)
device.open()

# Coordinator XBee's MAC address
COORDINATOR_MAC = "0013A20041E1E5B5"  # Replace with the actual MAC address
remote_device = RemoteXBeeDevice(device, XBee64BitAddress.from_hex_string(COORDINATOR_MAC))

def calculate_checksum(Tid, a, b, c):
    return (Tid + 4 + 3 + a + b + c) % 10

def send_data():
    Tid = 21  # Timer ID
    a = 0     # a is always 0

    for b in range(9, -1, -1):
        for c in range(9, -1, -1):
            chk = calculate_checksum(Tid, a, b, c)
            message = f"{250} {Tid} {a} {b} {c} {chk} {251}"
            try:
                device.send_data(remote_device, message)
                print(f"Sent: {message}")
            except TransmitException as e:
                print(f"Failed to send message: {e}")
            time.sleep(0.1)  # Small delay to avoid overwhelming the receiver

try:
    send_data()
except KeyboardInterrupt:
    print("\nExiting...")
finally:
    device.close()