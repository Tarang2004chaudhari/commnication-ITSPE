#include <Arduino.h>

// Define pins for UART communication
// pico tx rx
#define LORA_TX_PIN 16
#define LORA_RX_PIN 17
#define LED_PIN 2

const int PACKET_SIZE = 7;
uint8_t buffer[PACKET_SIZE];
int bufferIndex = 0;
 int Tid,a,b,c,chk;
 int val=0;
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

  Serial.println("Setup complete");
}

int calculateChecksum(int Tid, int a, int b, int c) {
  return (Tid + 4 + 3 + a + b + c) % 10;
}

void loop() {
  while (Serial1.available()) {
    uint8_t incomingByte = Serial1.read();
    Serial.print("Received byte: 0x");
    Serial.println(incomingByte, HEX);

    if (bufferIndex == 0 && incomingByte == 0xFA) { // Start byte
      buffer[bufferIndex++] = incomingByte;
    } else if (bufferIndex > 0) {
      buffer[bufferIndex++] = incomingByte;
      if (bufferIndex == PACKET_SIZE) {
        if (buffer[PACKET_SIZE - 1] == 0xFB) { // End byte
           Tid = buffer[1];
           a = buffer[2];
           b = buffer[3];
           c = buffer[4];
           chk = buffer[5];
           val=a*100+b*10+c;
          int calculatedChk = calculateChecksum(Tid, a, b, c);
          if (chk == calculatedChk) {
            Serial.println("Packet received correctly");

            // Print the received data to the serial monitor
            Serial.print("Received: ");
            Serial.print(Tid);
            Serial.print(" ");
            Serial.print(a);
            Serial.print(" ");
            Serial.print(b);
            Serial.print(" ");
            Serial.print(c);
            Serial.print(" ");
            Serial.println(chk);
            Serial.print(" ");
             Serial.println(val);

            // Blink the LED
            digitalWrite(LED_PIN, HIGH);
            delay(500);
            digitalWrite(LED_PIN, LOW);
            delay(500);

            // Send acknowledgment back to the transmitter
            int ackChk = (Tid + 4 + 1) % 10;
            Serial1.write(0xFA);
            Serial1.write(Tid);
            Serial1.write(4);
            Serial1.write(1);
            Serial1.write(ackChk);
            Serial1.write(0xFB);
            
            // Debugging the ACK sending
            Serial.print("Sent ACK: ");
            Serial.print(0xFA, HEX);
            Serial.print(" ");
            Serial.print(Tid, HEX);
            Serial.print(" ");
            Serial.print(4, HEX);
            Serial.print(" ");
            Serial.print(1, HEX);
            Serial.print(" ");
            Serial.print(ackChk, HEX);
            Serial.print(" ");
            Serial.println(0xFB, HEX);
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
