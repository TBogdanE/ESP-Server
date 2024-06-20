void setup() {
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);

  // Wait for serial to initialize
  while (!Serial) {
    ; // Wait for Serial port to connect
  }
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read incoming data
    String data = Serial.readStringUntil('\n');
    
    // Print received data
    Serial.print("Received: ");
    Serial.println(data);
  }
}
