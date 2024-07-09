#include <Arduino.h>

// Define pins for UART communication
#define LORA_TX_PIN 16
#define LORA_RX_PIN 17
#define LED_PIN 2

const int PACKET_SIZE = 9;
uint8_t buffer[PACKET_SIZE];
int bufferIndex = 0;
int Tid, a, b, c, chk,funcode,length;
int myTid = 22; // Change to 22 for Node C, 23 for Node D

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

  Serial.print("Receiver ");
  Serial.print(myTid);
  Serial.println(" setup complete");
}

int calculateChecksum(int Tid, int a, int b, int c) {
  return (Tid + 4 + 3 + a + b + c) % 10;
}

void sendAck(int Tid, int fromNode) {
    int ackChk = (Tid + 4 + 1) % 10;
  Serial1.write(0xFC);
  Serial1.write(Tid);
  // Serial1.write(4);
  // Serial1.write(1);
  
  Serial1.write(ackChk);
  Serial1.write(fromNode);

  // Debugging the ACK sending

}

void forwardMessage(int destTid,int funcode,int length, int a, int b, int c, int fromNode) {
  // Forward the message to the specified destination Tid
  Serial.print("Forwarding message to Node ");
  Serial.print(destTid);
  Serial.println("...");

  // Example: Forwarding logic using direct write to Serial1
 Serial1.write(0xFA);
  Serial1.write(destTid);
    Serial1.write(funcode);
  Serial1.write(length);  // Forward to next Node
  Serial1.write(a);
  Serial1.write(b);
  Serial1.write(c);
  int chk = calculateChecksum(destTid, a, b, c);
  Serial1.write(chk);
  Serial1.write(0xFB);

  // Debugging the forwarding

}

void loop() {
  while (Serial1.available()) {
    uint8_t incomingByte = Serial1.read();
    // Serial.print("Received byte: 0x");
    // Serial.println(incomingByte, HEX);

    if (bufferIndex == 0 && incomingByte == 0xFA) { // Start byte
      buffer[bufferIndex++] = incomingByte;
    } else if (bufferIndex > 0) {
      buffer[bufferIndex++] = incomingByte;
      if (bufferIndex == PACKET_SIZE) {
        if (buffer[PACKET_SIZE - 1] == 0xFB) { // End byte
         Tid = buffer[1];
          funcode=buffer[2];
          length=buffer[3];
          a = buffer[4];
          b = buffer[5];
          c = buffer[6];
          chk = buffer[7];
          int calculatedChk = calculateChecksum(Tid, a, b, c);
          if (chk == calculatedChk) {
            Serial.println("Packet received correctly");

            // Print the received data to the serial monitor
       
            // // Blink the LED
            // digitalWrite(LED_PIN, HIGH);
            // delay(500);
            // digitalWrite(LED_PIN, LOW);
            // delay(500);

            // Send acknowledgment back to the transmitter
            // sendAck(Tid, myTid);

            // Check if the message is for this node
            if (Tid == myTid) {
              // Process the message intended for this node
            Serial.println("Processing message locally");
            Serial.print("Received: ");
            Serial.print(Tid);
            Serial.print(" ");
                    Serial.print(funcode);
            Serial.print(" ");
                    Serial.print(length);
            Serial.print(" ");
            Serial.print(a);
            Serial.print(" ");
            Serial.print(b);
            Serial.print(" ");
            Serial.print(c);
            Serial.print(" ");
            Serial.println(chk);
              // TODO: Add your processing logic here
            } else {
              // Forward the message to the next destination (e.g., Node C, D, or E)
              // Serial.print(myTid);
             if(Tid==23){

             int  nextNode=23;
            
              forwardMessage(nextNode,funcode,length, a, b, c, myTid);
             }
            }
          } else {
            Serial.println("Checksum mismatch");
          }
        } else {
          Serial.println("End byte not received correctly");
        }
        bufferIndex = 0; // Reset buffer index for the next packet
      }
    }
  }
}
