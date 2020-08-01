#include <SPI.h>
#include <WiFi.h> //Connect to WiFi Network
#include <string.h>  //used for some string handling and processing.
#include <mpu6050_esp32.h>  // you should have the improved dat
#include <math.h>
#include <WifiLocation.h>

#define IDLE 0
#define CHECK_FALL 1
#define WAIT 2
#define FUNNY 3
#define DANGER 4
#define LONGTIME 5
#define CLEANUP 6

//--------------- N E T W O R K -------------------------------
char network[] = "RATONES-X5";  //SSID CHANGE!!
char password[] = "moderato"; //Password for WiFi CHANGE!!!
char host[] = "608dev-2.net";

const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
const char googleApiKey[] = "AIzaSyCs-oYxiui65Y1ZSebjkZVRpyq-Z-5qdBg";
WifiLocation location(googleApiKey);
location_t loc;
char loc_data[50];

//-------------- T I M E R S --------------------------------------------
const uint8_t LOOP_PERIOD = 10; // used for timing data acquisition
uint32_t loop_timer = 0;
uint32_t record_timer = 0;
long check_timer;
long fall_timer;
long danger_time = 30000;
const int UPDATE_PERIOD = 20;
int sleep_timer;
int time_to_sleep = 30; //seconds
int us_to_s_conversion = 1000000;

//------------------- D A T A ------------------------------------------
float x, y, z;
float old_acc = 0;
float older_acc = 0;
float highest_acc = 0;

uint16_t raw_reading;
uint16_t scaled_reading;
uint16_t old_reading = 0;
uint16_t older = 0;
float avg_reading = 0;
float imu_reading;

//--------------------- M I S C -------------------------------
char twitter_user[] = "bleaoh_";  // Bonk user's twitter handle  // shoutout Rachel Liu for the username
char emergency_contact[] = "harshal13c";   // twitter handle of the user's emergency contact

float threshold = 3.8;   // good IMU reading for a fall
float sleep_threshold = 0.55;   // if IMU reading is below this number, there is no movement

int state = 0;    // controls bonk_fsm
int sleep_state = 0;  // controls sleep_fsm

const uint8_t PIN_1 = 26; //button 1
MPU6050 imu; // imu object called, appropriately, imu
char outcome[20];   // tells python script which to tweet to post -> "funny", "danger", or "longtime"
bool danger_sent = false;   // true after we have sent a "danger" tweet

//--------------------------------- S E T U P ------------------------------------------

void setup() {
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  
  Wire.begin();
  delay(50); //pause to make sure comms get set up
  if (imu.setupIMU(1)) {
    //Serial.println("IMU Connected!");
  } else {
    //Serial.println("IMU Not Connected :/");
    //Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }
  
  setup_user();   // links user and their emergency contact
  pinMode(PIN_1, INPUT_PULLUP); //set input pin as an input!
  state = IDLE;
  esp_sleep_enable_timer_wakeup(time_to_sleep * us_to_s_conversion);  // enable timer induced wake-up for the ESP32 in deep sleep mode
  //sleep_timer = millis();
}

// ---------------------------------- M A I N -------------------------------------

void loop() {
  imu_reading = measure_fall();   // get an IMU reading
  //Serial.println(imu_reading);
  bonk_fsm(imu_reading);  // send IMU reading into main state machine
  while (millis() - loop_timer < LOOP_PERIOD);  //control loop period
  loop_timer = millis();
}
