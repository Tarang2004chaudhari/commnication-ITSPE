#include <SoftwareSerial.h>

// Define pins for SoftwareSerial
#define RX_PIN 0
#define TX_PIN 1
#define LED_PIN 2

SoftwareSerial XBee(RX_PIN, TX_PIN); // RX, TX
int a,b,c,chk=0;
int val=0;
String receivedBuffer = ""; // Buffer to store received data

void setup() {
  // Initialize both ports at 9600 baud
  XBee.begin(9600);
  Serial.begin(9600);
  
  // Initialize the LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Check if data is available from XBee and store it in the buffer
  if (XBee.available()) {
    char c = XBee.read();
    if (isPrintable(c)) {
      receivedBuffer += c;
    }
    // Check if we have received a complete message
    if (receivedBuffer.endsWith("251")) {
      Serial.println("Received: " + receivedBuffer);
      
      // Process received data
      processReceivedData(receivedBuffer);
      receivedBuffer = ""; // Clear the buffer for the next message
    }
  }
  
  // Check if data is available from Serial monitor and send it to XBee
  if (Serial.available()) {
    char c = Serial.read();
    XBee.write(c);
  }
}

// Helper function to check if a character is printable
bool isPrintable(char c) {
  return (c >= 32 && c <= 59); // ASCII printable characters range from 32 to 126
}

void processReceivedData(String data) {
  // Check for start and end bytes
  int startIdx = data.indexOf("250");
  int endIdx = data.indexOf("251");

  if (startIdx != -1 && endIdx != -1) {
    String payload = data.substring(startIdx + 3, endIdx);
    int Tid = payload.substring(0, 2).toInt();
     a = payload.substring(2, 3).toInt();
     b = payload.substring(3, 4).toInt();
     c = payload.substring(4, 5).toInt();
     chk = payload.substring(5, 6).toInt();
     val=b*10+c;
    // Validate checksum
    int calculatedChk = (Tid + 4 + 3 + a + b + c) % 10;
    if (chk == calculatedChk) {
      Serial.println(val);
      // Blink the LED to indicate successful reception
      // digitalWrite(LED_PIN, HIGH);
      // delay(500);
      // digitalWrite(LED_PIN, LOW);
      // delay(500);
      
      // Send acknowledgment back to the transmitter
      String ackData = "250" + String(Tid) + "41" + String(calculatedChk) + "251";
      XBee.println(ackData);
    } else {
      Serial.println("Checksum mismatch");
    }
  } else {
    Serial.println("Invalid data format");
  }
}
