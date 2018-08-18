#include <OneWire.h> // This is for the temperature sensors
#include <DallasTemperature.h> // This is for the temperature sensors
#include <Wire.h> // This is for the RTC
#include "RTClib.h" // This is for the RTC
#include <SD.h>

#define SENSORS_PIN 9
#define N_SENSORS 4
#define SD_BUSSY_LED_PIN 8
#define FILENAME "data.txt"

OneWire oneWireObject(SENSORS_PIN);
DallasTemperature sensorDS18B20(&oneWireObject);
// Declaramos un RTC DS3231
RTC_DS3231 rtc;

void SD_indicate_busy(int i);

void setup() {
	pinMode(SD_BUSSY_LED_PIN, OUTPUT);
    Serial.begin(9600);
    sensorDS18B20.begin();
	// RTC initialization ----------
	Serial.println("Initializing RTC...");
	delay(3000);
	if (! rtc.begin()) {
		Serial.println("ERROR: Cannot find RTC module!");
		while (1);
	}
	//~ rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Uncomment this line to set the RTC time to the compilation time.
	// SD card initialization ------
	Serial.println("Initializing SD card...");
	if (!SD.begin(4)) {
		Serial.println("ERROR: SD card failed, or not present");
		while (1);
	}
	delay(1000);
}
 
void loop() {
	int k;
	int current_day = -1;
	DateTime now;
	
	// Take measurements ------------------------------------
	sensorDS18B20.requestTemperatures();
    now = rtc.now();
    // Create the data file ------------
    SD_indicate_busy(1);
	if (!SD.exists(FILENAME)) {
		Serial.println("Creating file...");
		File dataFile = SD.open(FILENAME, FILE_WRITE);
		if (dataFile) { // if the file is available, write to it:
			dataFile.println("Year\tMonth\tDay\tHour\tMinute\tSecond\tTemperatures (C)");
			dataFile.close();
			SD_indicate_busy(0);
		} else { // if the file isn't open, pop up an error:
			Serial.println("ERROR: Cannot open file in SD card");
			SD_indicate_busy(0);
			while (1);
		}
	}
    // Write data to file -----------------------------------
    SD_indicate_busy(1);
    File dataFile = SD.open(FILENAME, FILE_WRITE);
	if (dataFile) { // if the file is available, write to it:
		dataFile.print(now.year());
		dataFile.print('\t');
		dataFile.print(now.month());
		dataFile.print('\t');
		dataFile.print(now.day());
		dataFile.print('\t');
		dataFile.print(now.hour());
		dataFile.print('\t');
		dataFile.print(now.minute());
		dataFile.print('\t');
		dataFile.print(now.second());
		dataFile.print('\t');
		for (k=0; k<=N_SENSORS-1; k++) {
			dataFile.print(sensorDS18B20.getTempCByIndex(k));
			dataFile.print('\t');
		}
		dataFile.print('\n');
		dataFile.close();
		SD_indicate_busy(0);
		
		Serial.print("Data has been saved! ");
		Serial.print(now.year());
		Serial.print(" ");
		Serial.print(now.month());
		Serial.print(" ");
		Serial.print(now.day());
		Serial.print(" ");
		Serial.print(now.hour());
		Serial.print(" ");
		Serial.print(now.minute());
		Serial.print(" ");
		Serial.println(now.second());
	}
	else { // if the file isn't open, pop up an error:
		Serial.println("ERROR: Cannot open file in SD card");
	}
	delay(3000); 
}

void SD_indicate_busy(int i) {
	if (i!=0)
		digitalWrite(SD_BUSSY_LED_PIN, HIGH);
	else
		digitalWrite(SD_BUSSY_LED_PIN, LOW);
}
