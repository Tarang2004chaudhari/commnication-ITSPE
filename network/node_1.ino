#include <Arduino.h>

// Define pins for UART communication
#define LORA_TX_PIN 16
#define LORA_RX_PIN 17
#define LED_PIN 2

const int PACKET_SIZE = 7;
uint8_t buffer[PACKET_SIZE];
int bufferIndex = 0;
int Tid = 20; // Node A's ID

void setup() {
  // Initialize UART communication with the LoRa module
  Serial1.setRX(LORA_RX_PIN);
  Serial1.setTX(LORA_TX_PIN);
  Serial1.begin(9600);

  // Initialize the serial monitor for debugging
  Serial.begin(9600);

  // Initialize the LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Transmitter A setup complete");
}

int calculateChecksum(int Tid, int a, int b, int c) {
  return (Tid + 4 + 3 + a + b + c) % 10;
}

void sendPacket(int destTid,int funcode,int length, int a, int b, int c) {
  Serial1.write(0xFA); // Start byte
  Serial1.write(destTid);
  Serial1.write(funcode);
  Serial1.write(length);
  Serial1.write(a);
  Serial1.write(b);
  Serial1.write(c);
  int chk = calculateChecksum(destTid, a, b, c);
  Serial1.write(chk);
  Serial1.write(0xFB); // End byte

  // Debugging the packet sending
  Serial.print("Sent packet: ");
  Serial.print(0xFA, HEX);
  Serial.print(" ");
  Serial.print(destTid, HEX);
  Serial.print(" ");
  Serial.print(a, HEX);
  Serial.print(" ");
  Serial.print(b, HEX);
  Serial.print(" ");
  Serial.print(c, HEX);
  Serial.print(" ");
  Serial.print(chk, HEX);
  Serial.print(" ");
  Serial.println(0xFB, HEX);
}

void loop() {
  // Example: Send data to Node B (Tid = 21)
  int a = 0;
  int b = 0;
  int c = 0;
  for(int i = 9; i > 0; i--) {
    for(int j = 9; j >= 0; j--) {
      sendPacket(21, 4,3,a, i, j);
     
      sendPacket(22, 4,3,a, i, j);
      
      sendPacket(23, 4,3,a, i, j);
      delay(1000);
      // sendPacket(24, a, 9 - i, 9 - j);
      // delay(1000);
    
  // Check for acknowledgments from Nodes
  while (Serial1.available()) {
    uint8_t incomingByte = Serial1.read();
    if (incomingByte == 0xFA) { // Start of ACK packet
      int ackTid = Serial1.read();
      int ackChk = Serial1.read();
      int pathTid = Serial1.read();
      if (ackTid >= 21 && ackTid <= 24) { // Check if ACK is from Nodes B, C, D, or E
        // Validate ACK checksum
        int calculatedChk = (ackTid + 4 + 1) % 10; // Adjust this calculation as per your protocol
        if (ackChk == calculatedChk) {
          Serial.print("Received ACK from Node ");
          Serial.println(ackTid);
          Serial.print("Path: ");
          Serial.println(pathTid, HEX);
          // Optionally, you can take further actions upon receiving ACK
        } else {
          Serial.println("Invalid ACK checksum");
          Serial.println(ackTid);
        }
      } else {
        Serial.println("Unexpected ACK recipient");
      }
    }
  }
}
  }
}
