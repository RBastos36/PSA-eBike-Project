#include <HardwareSerial.h>
#include <String.h>
#include <TinyGPSPlus.h>
#include <stdio.h>

// Define pins for the GPS module Serial communication
#define RXD2 16
#define TXD2 17

// Define pins for the GPRS modem Serial communication
#define RXD1 18
#define TXD1 19

// Define relay pin
#define RELAY_PIN 27

// Task handle for sensor loop
TaskHandle_t Task1;

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// MQTT connect packet
uint8_t connect[] = {
    0x10, // Connect Fixed Header
    0x12, // Remaining Length (18 bytes)
    0x00, 0x04, // Protocol Length (4 bytes)
    0x4D, 0x51, 0x54, 0x54, // Protocol Name ("MQTT")
    0x04, // Protocol Level (default is 4 (0x04))
    0x02, // Connect Flags
    0x00, 0x3C, // Keep Alive (60 seconds)
    0x00, 0x06, // Client Identifier Length (6 bytes)
    0x31, 0x30, 0x33, 0x39, 0x38, 0x33 // Client Identifier (103983)
};

// MQTT publish packet to send ebike data
uint8_t publish[] = {
    0x30,	// Publish Fixed Header
    0x1E,	// Remaining Length (30 bytes)
    0x00, 0x05,	// Topic Filter Length (5 bytes)
    0x65, 0x62, 0x69, 0x6B, 0x65,	// // Topic Filter "ebike"
    // Payload added later (with a size of 23 bytes)
};

// MQTT publish packet to send QR data
uint8_t publish_qrdata[] = {
    0x30, // Publish Fixed Header
    0x0F, // Remaining Length (15 bytes)
    0x00, 0x06, // Topic Filter Length (6 bytes)
    0x71,	0x72,	0x64,	0x61,	0x74,	0x61, // Topic Filter "qrdata"
    // Payload added later (with a size of 7 bytes)
};

// MQTT subscribe packet to read message topic
uint8_t subscribe_message[] = {
    0x82,	// Subcribe Fixed Header
    0x0C,	// Remaining Length (12 bytes)
    0x00,	0x01,	// Packet Identifier (0x0001)
    0x00,	0x07,	// Topic Filter Length (7 bytes)
    0x6D,	0x65,	0x73,	0x73, 0x61,	0x67,	0x65,	// // Topic Filter "message"
    0x00 // Requested Quality of Service (QoS 0: At most once delivery)
};

// MQTT unsubscribe packet to message topic
uint8_t unsubscribe_message[] = {
    0xA2, // Unsubcribe Fixed Header
    0x0B, // Remaining Length (11 bytes)
    0x00, 0x01, // Packet Identifier (0x0001)
    0x00, 0x07, // Topic Filter Length (7 bytes)
    0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65 // Topic Filter "message"
};


const int ARRAY_SIZE = 30; // Array size of the coordinates

// Initialize array with invalid coordinates
char coordinates[ARRAY_SIZE] = "INVINVINV,INVINVINV,INV"; 

// Array of possible responses for the different AT commands
char* responses[] = {"OK,ERROR", "SHUT OK,ERROR", "CONNECT OK,ERROR", "SEND OK,ERROR", ">,ERROR", "CLOSE OK,ERROR"};

int state;    // Variable to store the state of the command list
int response; // Variable to store the response of a command
int standby_response; // Variable to store the first response of the AT+CIFSR command
bool send_data = false; // Variable to allow data to be sent
String receivedWord = "";  // String to hold received information


// Function to wait for a specific response from the modem
int16_t wait(char* Values, uint16_t timeout) {

  uint16_t Length = strlen(Values);   // Determine the length of the Values string

  // Create a buffer to hold the input values and copy the Values string into it
  char InputBuffer[Length + 1];
  strcpy(InputBuffer, Values);

  // Create a buffer to hold the comparison values and initialize it to zero
  char CompareBuffer[Length + 1];
  memset(CompareBuffer, 0, sizeof(CompareBuffer));

  // Initialize Quantity to 1 to count the number of tokens
  uint16_t Quantity = 1;
  
  // Count the number of tokens in InputBuffer by counting commas
  for (int16_t n = 0; n < Length; n++) {
    if (InputBuffer[n] == ',')
      Quantity++;
  }

  // Create arrays of pointers for the input and comparison tokens
  /* By manipulating pointers, the function can directly compare 
  substrings without allocating additional memory */
  char* InputTokens[Quantity];
  memset(InputTokens, 0, sizeof(InputTokens));
  char* CompareTokens[Quantity];
  memset(CompareTokens, 0, sizeof(CompareTokens));

  // Initialize the first token pointers
  InputTokens[0] = InputBuffer;
  CompareTokens[0] = CompareBuffer;

  // Split the InputBuffer into tokens and initialize corresponding CompareTokens
  uint16_t TokenPosition = 1;
  for (int16_t n = 0; n < Length; n++) {
    if (InputBuffer[n] == ',') {
      InputBuffer[n] = 0;
      InputTokens[TokenPosition] = &InputBuffer[n + 1];
      CompareTokens[TokenPosition] = &CompareBuffer[n + 1];
      TokenPosition++;
    }
  }

  // Start the timer to measure the timeout period
  uint64_t timer = millis();
  char c;

  // Loop until the timeout period has elapsed
  while (millis() - timer < timeout) {
    // Check if there is data available on Serial2
    while (Serial2.available()) {
      c = Serial2.read();   // Read a character from Serial2

      // Loop through each token
      for (int16_t n = 0; n < Quantity; n++) {
        Length = strlen(CompareTokens[n]);    // Get the length of the current CompareToken

        // If the current character matches the next character in InputToken
        if (c == InputTokens[n][Length])
          CompareTokens[n][Length] = c;   // Append the character to the CompareToken
        else
          memset(CompareTokens[n], 0, Length);    // If the character does not match, reset the CompareToken

        // Check if the InputToken matches the CompareToken
        if (!strcmp(InputTokens[n], CompareTokens[n]))
          return n;   // Return the index of the matching token (usually, 0 or 1, depending on the responses possible)
      }
    }
  }
  return -1;    // Return -1 if no match is found within the timeout period
}


// Function to wait for the AT+CIFSR command response
int16_t wait_CIFSR(uint16_t timeout) {
  uint64_t startTime = millis();    // Record the start time
  
  // Loop until the timeout period has elapsed
  while (millis() - startTime < timeout) {
    // Check if there is data available on Serial2 (GPRS module)
    if (Serial2.available()) {
      String input = Serial2.readStringUntil('\n');   // Read a line of input from the GPRS module
      
      // Check if input is the string "ERROR"
      if (input.equals("ERROR")) {
        return 1;   // Return 1 to indicate an error response
      }
      
      // Check if the input is of sufficient length to be an IPv4 address
      if (input.length() >= 7) {
        return 0;   // Return 0 to indicate a valid IP address was received
      }
    }
  }
  // Return -1 if timeout occurs (no input received within timeout period)
  return -1;
}


// Task function to continuously read GPS data
void sensorLoop(void * parameter) {
  // Infinite loop to keep the task running
  for( ;; ) {
    // Check if there is data available on Serial1 (GPS module)
    while (Serial1.available() > 0) {
      // Read a character from the GPS module
      char gpsChar = Serial1.read();

      // Feed the character to the TinyGPSPlus library for processing
      if (gps.encode(gpsChar)) {
        // Check if both location and speed data are valid
        if (gps.location.isValid() && gps.speed.isValid()) {
          int roundedSpeed = round(gps.speed.kmph());   // Round the speed to the nearest integer
          snprintf(coordinates, ARRAY_SIZE, "%.6f,%.6f,%03d", gps.location.lat(), gps.location.lng(), roundedSpeed);    // Format the latitude, longitude, and speed into the coordinates array
        }
        else {
          strncpy(coordinates, "INVINVINV,INVINVINV,INV", ARRAY_SIZE);    // If the GPS data is invalid, set coordinates to predefined invalid values
        }
      }
    }
    // Wait for 1 second before the next iteration
    delay(1000);
  }
}

void setup() {
  // Setup code to run once:  
  Serial.begin(9600);  // the ESP-CAM's serial port baud rate
  Serial.setTimeout(10);  // Sets the maximum milliseconds to wait for serial data
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);   // the GPRS's serial port baud rate
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);   // the GPS's serial port baud rate
  pinMode(RELAY_PIN, OUTPUT);    // sets the digital pin of the Opto Relay as output
  digitalWrite(RELAY_PIN, HIGH);    // Initialize relay to HIGH
  delay(10000);   // Delay to allow the GPRS modem to be powered on

  // Create the sensor loop task for the GPS
  xTaskCreatePinnedToCore(
    sensorLoop,   // Function to implement the task (sensorLoop)
    "sensorLoop",   // Name of the task
    10000,    // Stack size in words (2000 works fine for at least 60+ minutes)
    NULL,   // Task input parameter - No input
    0,    // Priority of the task 
    &Task1,   // Task handle
    0);   // Task assigned to Core 0
  
  delay(500);  // needed to start-up sensorLoop task
}

void loop() {
  // Main code to run repeatedly:
  if (Serial.available() > 0 && send_data == false) { // Check if there is data available to read from Serial and send_data is false
    receivedWord = Serial.readString(); // Read the incoming data as a String

    send_data = true;   // Set send_data to true to start the state machine process
  }

  // Execute the state machine only when send_data is true
  while (send_data == true) {
    if (state == 0) {
      Serial2.println("AT");    // Check if modem is responding
      response = wait(responses[0], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }

    if (state == 1) {
      Serial2.println("AT+CIPSHUT");    // Ensure there are no other active connections
      response = wait(responses[1], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }

    if (state == 2) {
      Serial2.println("AT+CGATT=1");    // Attach to GPRS service
      response = wait(responses[0], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }

    if (state == 3) {
      Serial2.println("AT+CSTT=\"internet\",\"\",\"\"");    // Set the APN
      response = wait(responses[0], 5000);    // Wait for a response

      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }

    if (state == 4) {
      Serial2.println("AT+CIICR");    // Bring up the wireless connection
      response = wait(responses[0], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }
    
    if (state == 5) {
      Serial2.println("AT+CIFSR");    // Get the local IP address
      response = wait_CIFSR(5000);    // Wait for an IP address response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "OK"
      }
    }
    
    if (state == 6) {
      Serial2.println("AT+CIPSTART=\"TCP\",\"193.137.172.20\",\"85\"");   // Start the TCP connection
      standby_response = wait(responses[0], 5000);    // Wait for a response
      if (standby_response == 0) {    // Proceed if response is "OK"
        response = wait(responses[2], 5000);    // Wait for a response
      }
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "CONNECT OK"
      }
    }

    if (state == 7) {
      Serial2.println("AT+CIPSEND");    // Prepare to send data
      response = wait(responses[4], 5000);    // Wait for a ">" prompt
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is ">"
      }
    }

    if (state == 8) {
      Serial2.write(connect, sizeof(connect));    // Send the connect message
      Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
      response = wait(responses[3], 5000);
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "SEND OK"
      }
    }

    if (state == 9) {
      Serial2.println("AT+CIPSEND");    // Prepare to send data again
      response = wait(responses[4], 5000);    // Wait for a ">" prompt
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is ">"
      }
    }

    if (state == 10) {
      // Convert the String to a char array
      char receivedChars[receivedWord.length() + 1]; // +1 for null terminator
      receivedWord.toCharArray(receivedChars, receivedWord.length() + 1);

      Serial2.write(publish_qrdata, sizeof(publish_qrdata));    // Send the publish QR data message
      Serial2.write(receivedChars, sizeof(receivedChars));    // Send the received data
      Serial2.write(0x1A); // the ASCII code of the ctrl+z is 26 / HEX code is 0x1A
      response = wait(responses[3], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "SEND OK"
      }
    }

    if (state == 11) {
      Serial2.println("AT+CIPCLOSE");   // Close the TCP connection (because of connection issues - optional)
      response = wait(responses[5], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "CLOSE OK"
      }
    }

    if (state == 12) {
      Serial2.println("AT+CIPSTART=\"TCP\",\"193.137.172.20\",\"85\"");   // Start the TCP connection again
      standby_response = wait(responses[0], 5000);    // Wait for a response
      if (standby_response == 0) {
        response = wait(responses[2], 5000);    // Wait for another response if previous response was "OK"
      }
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "CONNECT OK"
      }
    }

    if (state == 13) {
      Serial2.println("AT+CIPSEND");    // Prepare to send data again
      response = wait(responses[4], 5000);    // Wait for a ">" prompt
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is ">"
      }
    }

    if (state == 14) {
      Serial2.write(connect, sizeof(connect));    // Send the connect message
      Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
      response = wait(responses[3], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "SEND OK"
      }
    }

    if (state == 15) {
      delay(1000);    // Wait for a second to allow connection to be established
      Serial2.println("AT+CIPSEND");    // Prepare to send data again
      response = wait(responses[4], 5000);    // Wait for a ">" prompt
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is ">"
      }
    }

    if (state == 16) {
      Serial2.write(subscribe_message, sizeof(subscribe_message));    // Send the subscribe message
      Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
      response = wait(responses[3], 5000);    // Wait for a response
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is "SEND OK"
      }
    }

    if (state == 17) {
      while (Serial2.available() > 0) {
        byte receivedByte = Serial2.read(); // Read a byte from Serial
        
        
        // Check if the final byte is 1 or 2 (1: authorized | 2: not authorized)
        if (receivedByte == 0x31) {   // If authorized
          Serial2.println("AT+CIPSEND");    // Prepare to send data
          response = wait(responses[4], 5000);    // Wait for a ">" prompt
          if (response == 0) {
            Serial2.write(unsubscribe_message, sizeof(unsubscribe_message));    // Send the unsubscribe message
            Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
            digitalWrite(RELAY_PIN, LOW);   // Switch the Opto Relay to allow the e-bike to move
            state = state + 1;    // Move to the next state if response is "SEND OK"
          }
        }

        if (receivedByte == 0x32) {   // If not authorized
          Serial2.println("AT+CIPSEND");    // Prepare to send data
          response = wait(responses[4], 5000);    // Wait for a ">" prompt
          if (response == 0) {
            Serial2.write(unsubscribe_message, sizeof(unsubscribe_message));    // Send the unsubscribe message
            Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
            send_data = false;    // Set send_data to false to close the state machine process
            state = 0;    // Reset the state if response is "SEND OK"
          }
        }
      }
    }

    if (state == 18) {
      Serial2.println("AT+CIPSEND");    // Prepare to send data
      response = wait(responses[4], 5000);    // Wait for a ">" prompt
      if (response == 0) {
        state = state + 1;    // Move to the next state if response is ">"
      }
    }

    if (state == 19) {
      Serial2.write(publish, sizeof(publish));  // Send the publish packet for the ebike topic
      Serial2.write(coordinates);   // Send the ebike coordinates and velocity
      Serial2.write(0x1A); // Send the ASCII code of the Ctrl+z is 26 to end message (HEX code is 0x1A)
      response = wait(responses[3], 5000);    // Wait for a response
      if (response == 0) {
        state = 18;   // Reset the state to 18 if response is "OK", to loop the publishing of ebike data
        delay(1000);    // Delay to slow down to data rate sent to the server
      }
    }
  }
}

