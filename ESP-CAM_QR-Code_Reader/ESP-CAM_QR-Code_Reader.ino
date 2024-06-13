//ESP32-CAM QR code Reader

// Include necessary libraries

#include <WiFi.h>
#include "esp_camera.h"          // Library for camera module
#include "soc/soc.h"             //Library to solve problems related with unstable power supply (brown-out)
#include "soc/rtc_cntl_reg.h"    //Library to solve problems related with unstable power supply (brown-out)
#include "quirc.h"               //QR Code Library

//Defining Camera Pins
// These pins are used to connect the ESP32-CAM to the camera module
// and configure communication between them.
// Each pin corresponds to a specific function, like clock, data, etc.

#define PWDN_GPIO_NUM     32    // Power down pin
#define RESET_GPIO_NUM    -1    // Reset pin
#define XCLK_GPIO_NUM      0    // External clock pin
#define SIOD_GPIO_NUM     26    // Serial data pin
#define SIOC_GPIO_NUM     27    // Serial clock pin
#define Y9_GPIO_NUM       35    // Y9 data pin
#define Y8_GPIO_NUM       34    // Y8 data pin
#define Y7_GPIO_NUM       39    // Y7 data pin
#define Y6_GPIO_NUM       36    // Y6 data pin
#define Y5_GPIO_NUM       21    // Y5 data pin
#define Y4_GPIO_NUM       19    // Y4 data pin
#define Y3_GPIO_NUM       18    // Y3 data pin
#define Y2_GPIO_NUM        5    // Y2 data pin
#define VSYNC_GPIO_NUM    25    // Vertical synchronization pin
#define HREF_GPIO_NUM     23    // Horizontal synchronization pin
#define PCLK_GPIO_NUM     22    // Pixel clock pin


// Structure to hold QR code data
struct QRCodeData
{
  bool valid;                 // Flag indicating if the QR code data is valid
  int dataType;               // Type of data encoded in the QR code (e.g., text, link)
  uint8_t payload[1024];      // Payload data extracted from the QR code
  int payloadLen;             // Length of the payload data
};  

// Initialize global variables
struct quirc *q = NULL;       // Pointer to the quirc object for QR code processing
uint8_t *image = NULL;        // Pointer to image data buffer
camera_fb_t * fb = NULL;      // Pointer to camera frame buffer
struct quirc_code code;       // Structure to hold a detected QR code
struct quirc_data data;       // Structure to hold decoded data from a QR code
quirc_decode_error_t err;     // Error code for QR code decoding
struct QRCodeData qrCodeData; // Struct to hold processed QR code data 

camera_config_t config;       // Camera configuration settings

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout detection to handle unstable power supply
    
  Serial.begin(9600);           // Initialize serial communication
  Serial.setDebugOutput(true);  // Enable diagnostic output to serial monitor
  //Serial.println();
  
//Configuration of camera pins
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  //Video initialization
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {      // Handle camera initialization error
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Set camera frame size
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);

  // Initialize flash
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);
  //pinMode(1 ,INPUT);                                //need to insert the input pin
  //Serial.println("Starting QRCode Decoder");
}

// Function to display decoded data
void dumpData(const struct quirc_data *data)
{
  if (data->version == 1){
    //Serial.println("Type of data: Information");
  }
  else if (data->version == 2){
    //Serial.println("Type of data: Link");
  }
  ///*

  // Display data payload
  const uint8_t* output = data->payload;

  //Serial.printf("Length: %d\n", data->payload_len);
  //Serial.printf("Payload: %s\n", output);

  Serial.printf("%s\n", output);
  
  //int output = data->payload;
  //const uint8_t* output = data->payload;
  //Serial.printf("", output);
  
  //Serial.write(output, sizeof(output));
  
  //*/
  //Serial.printf(data->payload);


  //The LED flash when QR Code is decoded
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(4, LOW);
}

int toRead; 

void loop() {

  //toRead = digitalRead(1);    // Read input pin (button) to start decoder
  toRead = 0;                   // Change this to read the input pin state

  if (toRead == 0){
    q = quirc_new();                                //To start we will need to create an object that will start the detection of qrcodes
    if (q == NULL){
      // Handle object creation failure 
      //Serial.print("Can't create quirc object\r\n");   
    }

    //Capture images from camera
    fb = esp_camera_fb_get();                               
    if (!fb)
    {
      // Handle camera capture failure
      //Serial.println("Camera capture failed");
    }   
    
    quirc_resize(q, fb->width, fb->height);         //We will need to set that the decoder object has the same size than 
                                                    //the captured image
    image = quirc_begin(q, NULL, NULL);             //Will be created a pointer (image) for the memmory space pre allocated in the beginning
    memcpy(image, fb->buf, fb->len);                //The number of bytes will be filled with the content of the image captured for the camera
                                                    //the content marked with the pointer (fb->buf) - image captured from camera- will be copied to the image pointer location with the size
                                                    //of fb->len (size of image)
    quirc_end(q);                                   //Detection ended - the image is ready to be processed
    
    int count = quirc_count(q);                     //The output is the number of qr codes founded
    if (count > 0) {                                
      quirc_extract(q, 0, &code);                   //If there is some qr codes in the image, the qr code will be extracted
      err = quirc_decode(&code, &data);             //A message of error will be created if the decoding process fails

      if (err){
        //Serial.println("Decoding FAILED");
      } else {
        //Serial.printf("Decoding successful:\n");
        //Serial.print("Number of QR Codes detected: ");
        //Serial.println(count);
        dumpData(&data);
      } 
      //Serial.println();
    } 
    
    esp_camera_fb_return(fb);
    fb = NULL;
    image = NULL;  
    quirc_destroy(q);                          //Allocated memory is released
  }
  
  else{

    // Wait before checking again
    //Serial.println("No need to read! Waiting 1 second to check again");
    delay(1000);
  }
}

