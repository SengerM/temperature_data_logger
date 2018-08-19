#include <OneWire.h> // This is for the temperature sensors
#include <DallasTemperature.h> // This is for the temperature sensors
#include <Wire.h> // This is for the RTC
#include "RTClib.h" // This is for the RTC
#include <SD.h>

#define SENSORS_PIN 9
#define N_SENSORS 4
#define SD_BUSSY_LED_PIN 8
#define MEASURE_AND_STORE_SWITCH_PIN 6
#define SD_CAN_BE_REMOVED_LED_PIN 7
#define DATA_FILE_NAME "datalog.txt" // Max 8 chars of name + 3 of extension!!!

OneWire oneWireObject(SENSORS_PIN);
DallasTemperature sensorDS18B20(&oneWireObject);
// Declaramos un RTC DS3231
RTC_DS3231 rtc;

void SD_is_busy(int i);
void SD_can_be_removed(bool i);

void setup() {
	pinMode(SD_BUSSY_LED_PIN, OUTPUT);
	pinMode(SD_CAN_BE_REMOVED_LED_PIN, OUTPUT);
	pinMode(MEASURE_AND_STORE_SWITCH_PIN, INPUT);
    Serial.begin(9600);
    sensorDS18B20.begin();
	// RTC initialization ----------
	Serial.print("Initializing RTC... ");
	delay(100);
	if (! rtc.begin()) {
		Serial.println("ERROR: Cannot find RTC module!");
		while (1);
	}
	//~ rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Uncomment this line to set the RTC time to the compilation time.
	Serial.println("OK");
	// SD card initialization ------
	Serial.print("Initializing SD card... ");
	delay(100);
	if (!SD.begin(10)) {
		Serial.println("ERROR: SD card failed, or not present");
		while (1);
	}
	Serial.println("OK");
	delay(1000);
}
 
void loop() {
	int k;
	int current_day = -1;
	DateTime now;
	File dataFile;
	
	SD_can_be_removed(false);
	SD_is_busy(false);
	while (1) {
		while (!digitalRead(MEASURE_AND_STORE_SWITCH_PIN))
			SD_can_be_removed(true);
		SD_can_be_removed(false);
		// Take measurements ------------------------------------
		sensorDS18B20.requestTemperatures();
		now = rtc.now();
		// Create the data file ------------
		SD_is_busy(true);
		if (!SD.exists(DATA_FILE_NAME)) {
			Serial.print("Creating file... ");
			dataFile = SD.open(DATA_FILE_NAME, FILE_WRITE);
			if (dataFile) { // If the file is available, write to it:
				dataFile.println("Year\tMonth\tDay\tHour\tMinute\tSecond\tTemperatures (C)");
				dataFile.close();
				SD_is_busy(false);
				Serial.println("File has been created");
			} else { // If the file isn't open, pop up an error:
				Serial.println("ERROR: Cannot open file in SD card");
				SD_can_be_removed(true);
				while (1);
			}
		}
		// Write data to file -----------------------------------
		SD_is_busy(true);
		dataFile = SD.open(DATA_FILE_NAME, FILE_WRITE);
		if (dataFile) { // If the file is available, write to it:
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
			SD_is_busy(false);
			
			Serial.print("Saved data: ");
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
			Serial.print(now.second());
			Serial.print(' ');
			for (k=0; k<=N_SENSORS-1; k++) {
				Serial.print(sensorDS18B20.getTempCByIndex(k));
				Serial.print(' ');
			}
			Serial.println(' ');
		} else { // if the file isn't open, pop up an error:
			Serial.println("ERROR: Cannot open file in SD card");
			SD_can_be_removed(true);
			while (1);
		}
		delay(1000); 
	}
}

void SD_is_busy(bool i) {
	if (i == true)
		digitalWrite(SD_BUSSY_LED_PIN, HIGH);
	else
		digitalWrite(SD_BUSSY_LED_PIN, LOW);
}

void SD_can_be_removed(bool i) {
	if (i == true) {
		digitalWrite(SD_CAN_BE_REMOVED_LED_PIN, HIGH);
		SD_is_busy(false);
	} else
		digitalWrite(SD_CAN_BE_REMOVED_LED_PIN, LOW);
}
