
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <EEPROM.h>
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>


#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

int ldrPin = 33;
int speedPin = 34;
int PowerPin = 35;




int ldrVlue = 0;
float speedVal = 0;
float fVlue = 0;
float hVlue = 0;
int preVlue = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(512);  //Initialize EEPROM
  pinMode(powerPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(speedPin, INPUT);

  digitalWrite(powerPin, HIGH);


  delay(5000);     // Give time to power up the devicers.
  //***********************************************************

  ldrPin = 34;
  ldrVlue = analogRead(potPin);

  //************************************************************

  speedPin = 35;
  speedVal = analogRead(speedPin);
  speedVal = map(speedVal, 0, 4095, 0, 32.4);


  //************************************************************
  Serial.println(F("BME280 test"));
  bme.begin(0x76);
  preVlue = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(preVlue);
  Serial.println(" hPa");




  //**************************************************************
  Serial.println(F("DHTxx test!"));
  dht.begin();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hVlue = dht.readHumidity();  // Reading humidity

  fVlue = dht.readTemperature(true);  // Read temperature as Fahrenheit (isFahrenheit = true)

  // Check if any reads failed and exit early (to try again).
  if (isnan(hVlue) || isnan(fVlue)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(hVlue);
  Serial.print("Temperature: ");
  Serial.print(fVlue);

  //*****************************************************************************
  int count = 0;
  int countSave = 0;
  int countAdd = 0;

  count =  EEPROM.read(0);
  EEPROM.write(0, count++);


  countAdd = EEPROM.read(1);
  EEPROM.write(countAdd++, ldrVlue);
  EEPROM.write(countAdd++, speedVal);
  EEPROM.write(countAdd++, fVlue);
  EEPROM.write(countAdd++, hVlue);
  EEPROM.write(countAdd++, preVlue);
  EEPROM.write(1, countAdd);

  // This means after 15 minutes, (3 min * 5 )
  if (count == 5) {
    if (WiFi.status() == WL_CONNECTED)
    {
      float ldrArray[5] = {0, 0, 0, 0, 0};
      float speedArray[5] = {0, 0, 0, 0, 0};
      float fArray[5] = {0, 0, 0, 0, 0};
      float hArray[5] = {0, 0, 0, 0, 0};
      float preArray[5] = {0, 0, 0, 0, 0};
      int addressCount = 1;

      for (int i = 0; i <= 5; i++) {
        ldrArray[i]   = EEPROM.read(1addressCount++);
        speedArray[i] = EEPROM.read(1addressCount++);
        fArray[i]     = EEPROM.read(1addressCount++);
        hArray[i]     = EEPROM.read(1addressCount++);
        preArray[i]   = EEPROM.read(1addressCount++);
      }

      //################################### send to the server ##########################################

      float average(float arr[], int leng) {
        float sum = 0;
        for (int a = 0; a < len; a++) {
          sum += arr[i];
        }
        return ((float) sum) / len;
      }

      float std(float a[], int n) {
        float sum = 0;

        for (int i = 0; i < n; i++) {
          sum += a[i];
        }

        float mean = (float)sum / (float)n;
        float std = 0;
        for (int i = 0; i < n; i++)
          std += (a[i] - mean) * (a[i] - mean);
        std = (float)std / n;
        return sqrt(std);
      }

      float temp = average(fArray);
      float pressure = average(preArray);
      float humidity = average(hArray);
      float w_speed = average(speedArray);
      float l_level = average(ldrArray);

      float temp_std = std(fArray);
      float pressure_std = std(preArray);
      float humidity_std = std(hArray);
      float w_speed_std = std(speedArray);
      float l_level_std = std(ldrArray);

      // create CAP XML string
      String temp_s = String(temp);
      String pressure_s = String(pressure);
      String humidity_s = String(humidity);
      String w_speed_s = String(w_speed);
      String l_level_s = String(l_level);


      String temp_s_std = String(temp_std);
      String pressure_s_std = String(pressure_std);
      String humidity_s_std = String(humidity_std);
      String w_speed_s_std = String(w_speed_std);
      String l_level_s_std = String(l_level_std);

      String capString = "<?xml version = \"1.0\" encoding = \"utf8\"?>" + "<alert>" + "<identifier>%s</identifier>" + "<sender> NODE_MCU</sender>" + "<sent> %s</sent>" + "<status> Actual</status>" + "<msgType> Alert</msgType>" + "<scope> Public</scope>"
                         + "<info>" + "<category> Env</category>" + "<event> Readings</event>" + "<urgency> Past</urgency>" + "<severity> Minor</severity>" + "<certainty> Observed</certainty>" + "<senderName> NODE_MCU Device</senderName>"
                         + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>"
                         + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>" +
                         "<parameter><valueName>%s</valueName><value>%s</value></parameter>" + "<parameter><valueName>%s</valueName><value>%s</value></parameter>"
                         + "</info>" + "</alert>";
      sprintf(
        capxml, capProtocol, "temp", temp_s.c_str(), "pressure", pressure_s.c_str(), "humidity", humidity_s.c_str(), "w_speed", w_speed_s.c_str(),
        "l_level", l_level_s.c_str(), "temp_std", temp_s_std.c_str(), "pressure_std", pressure_s_std_sd.c_str(), "humidity_std", humidity_s_std.c_str()), "w_speed_std", w_speed_s_std.c_str(), "l_level_std", l_level_s_std.c_str());

      WiFiClient wclient;
      HTTPClient  clientt;

      clientt.begin(wclient, "http://localhost:5000/add");
      int res = clientt.POST(capxmlstring);
      if (res != HTTP_CODE_OK)
        {
      http.errorToString(res).c_str();
        http.end();
       }
      http.end();


      EEPROM.write(0, 0);
      EEPROM.write(1, 1);
    }
    else
    {
      Serial.println("WIFI not connected");
    }

  }



  //EEPROM.writeInt(eepromAddr1, intData);
  delay(1000);
  EEPROM.commit();    //Store data to EEPROM
  delay(1000);


  digitalWrite(powerPin, LOW); //power off the sensors
  delay(2000);
  esp_sleep_enable_timer_wakeup(180000000); // Set the time to sleep in microseconds
  delay(2000);
  esp_deep_sleep_start();                  //ESP32 goes to sleep
}

void loop() {
  // no loop here because deep sleep mode of the esp8266 is using and powering down the sensors at that time (after every 3 minutes)
}
