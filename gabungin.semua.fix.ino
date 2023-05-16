#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <SD.h>
//#include <Ubidots.h>
#include <NewPing.h>

//PENDEKLARASIAN VARIABEL
#define TRIGGER_PIN  17
#define ECHO_PIN     16
#define MAX_DISTANCE 50
long durasi;
int jarak;
int volume;
File file; //sd card
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

const int CS = 5;
String formattedDate;
String dayStamp;
String timeStamp;
String dataMessage;

// WiFi credentials
const char* ssid = "3l3c7r0_80";
const char* password = "elistrum80";

void setup() {
  Serial.begin(115200);
  //pinMode(trig, OUTPUT);
  //pinMode(echo, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  configTime(7 * 3600, 0, "pool.ntp.org");

  // Initialize SD card
  SD.begin(CS);  
  if(!SD.begin(CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  
  Serial.println("Initializing SD card...");
  if (!SD.begin(CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  file = SD.open("/coba.txt", FILE_WRITE);
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/coba.txt", "Date, Time, Distance (cm)\r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
}

void loop() {
  /*digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  */
  int distance = sonar.ping_cm()+2; // JGN DIUBAH (TOLERANSI 2CM)
  int height = MAX_DISTANCE - distance;
  float volume = height * 3.14 * pow(11.5,2) / 1000; //jari jari tangki
  
  Serial.print("| Height = ");
  Serial.print(height);
  Serial.print(" cm");

  Serial.print("\t\t volume = ");
  Serial.print(volume);
  Serial.print(" L |");

  // Get date and time
  getTimeStamp();

  dataMessage =  "Waktu : " + String(dayStamp) + "," + String(timeStamp) + "," + " | Jarak : " + String(jarak) + " cm" + "\t volume = " + String(volume) + "| \r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/coba.txt", dataMessage.c_str());
  //delay(500);
}

// Function to get date and time
void getTimeStamp() {
  formattedDate = String("");
  dayStamp = String("");
  timeStamp = String("");
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
  dayStamp = String(buffer);
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  timeStamp = String(buffer);
}
// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message))
   {
    Serial.println("Data appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
