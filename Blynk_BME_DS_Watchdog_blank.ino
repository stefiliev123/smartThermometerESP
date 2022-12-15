#define BLYNK_TEMPLATE_ID "xxx"
#define BLYNK_DEVICE_NAME "ESP32"
#define BLYNK_AUTH_TOKEN "xxx"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <esp_task_wdt.h>



char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "xxx";
char pass[] = "xxx";

RTC_DATA_ATTR int bootCount = 0;

#define BLYNK_PRINT Serial
Adafruit_BME280 bme;
unsigned long delayTime;
BlynkTimer timer;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */

//#include <Ticker.h>

//Ticker periodicTicker;
//Ticker onceTicker;

int last = millis();

//10 seconds WDT
#define WDT_TIMEOUT 10

//void periodicPrint() {
//  Serial.println("printing in periodic function.");
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//  Serial.println("Going to sleep now...");
//  esp_deep_sleep_start();
//}

BLYNK_CONNECTED(){
 
  bool status;
   status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  float temperature = bme.readTemperature();
  Serial.println("Sending data");

  Blynk.virtualWrite(V0, temperature );
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, pressure);
  Blynk.virtualWrite(V3, bootCount);

  Serial.println("Data Sent");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Going to sleep now...");
  esp_deep_sleep_start();

}


void setup(){
  Serial.begin(115200);
  Serial.println("Booting........");

  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

//Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
 // onceTicker.attach_ms(4000, periodicPrint);

Blynk.config(auth);
Blynk.connectWiFi(ssid, pass);
Blynk.connect();

//Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
 // onceTicker.attach_ms(4000, periodicPrint);

}


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void loop(){

    if (millis() - last >= 8000 ) {
      Serial.println("Resetting WDT...");
      esp_task_wdt_reset();
      last = millis();
      }
        Blynk.run();
  }
