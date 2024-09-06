#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

// WiFi credentials stored in config.h
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *computer_ip = COMPUTER_IP;
const int port = 80;

// Template for timer objects
typedef struct {
  unsigned long duration;
  unsigned long start_time;
} Timer;

// Global variables
int pir_pin = 2;
volatile bool motion_detected = false;  // Flag to trigger ISR handling routine
unsigned long seconds = 0;
Timer ISR_cooldown = {0, 0};
unsigned long cooldown_duration = MOTION_CD;  // Defined in config.h
WebSocketsClient ws;

// NTP timekeeping
WiFiUDP ntpUDP;
  // <UDP>, <server>, <offset s>, <update interval ms>
NTPClient timeClient(ntpUDP, "time.mikes.fi", 3*3600, 1000*3600);

/* Prints the current time into Serial every second. Used as a heartbeat. */
void print_time(unsigned long &seconds, NTPClient timeClient) {
  unsigned long prev_seconds = seconds;
  seconds = timeClient.getSeconds();
  if (prev_seconds != seconds) {
    Serial.print(timeClient.getFormattedTime());
    if (!timeClient.isTimeSet()) {
      Serial.print(" - TIME NOT SET");
    }
    Serial.println();
  }
}

/* Checks if ISR was triggered (i.e. motion was detected), and handles
the event appropriately. */
void ISR_handling() {
  if (motion_detected) {
    detachInterrupt(pir_pin);
    start_timer(ISR_cooldown, cooldown_duration);
    Serial.println("MOTION DETECTED");
    ws.sendTXT("!");
    motion_detected = false;
  }

  return;
}

/* Interrupt Service Routine from a sensor trigger. */
IRAM_ATTR void ISR() {
  motion_detected = true;
  return;
}

/* Starts the given timer */
void start_timer(Timer &timer, unsigned long duration) {
  timer.start_time = millis();
  timer.duration = duration;
}

/* Checks ISR cooldown timer and re-attaches interrupt if the timer has finished. */
void check_ISR_timer(Timer timer) {
  unsigned long current_time = millis();
  if (current_time - timer.start_time >= timer.duration) attachInterrupt(pir_pin, ISR, RISING);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[ws] Disconnected");
            break;

        case WStype_CONNECTED:
            Serial.print("[ws] Connected to url: ");
            Serial.println((char *)payload);
            ws.sendTXT("ESP8266 connected");
            break;

        case WStype_TEXT:
            Serial.print("[ws] Received: ");
            Serial.println((char *)payload);
            break;
    }
    return;
}

void setup() {
  // Open serial
  Serial.begin(115200);
  while(!Serial) {};    // Waits for Serial
  Serial.println();

  // Start WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start NTP time service
  timeClient.begin();

  // Open WebSocket
  ws.begin(computer_ip, port, "/");
  ws.onEvent(webSocketEvent);
  ws.setReconnectInterval(5000);
}

void loop() {
  timeClient.update();              // Update time
  print_time(seconds, timeClient);  // Print current time into Serial
  ISR_handling();                   // Run ISR handling routine if ISR was trigg'd
  check_ISR_timer(ISR_cooldown);    // Re-attach hw interrupt if cooldown has finished
  ws.loop();                        // Handle WebSocket events
}
