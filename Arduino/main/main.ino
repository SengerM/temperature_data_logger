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
	if (!SD.exists(FILENAME)) {
		Serial.println("Creating file...");
		File dataFile = SD.open(FILENAME, FILE_WRITE);
		if (dataFile) { // if the file is available, write to it:
			dataFile.println("Year\tMonth\tDay\tHour\tMinute\tSecond\tTemperatures (C)");
			dataFile.close();
		} else { // if the file isn't open, pop up an error:
			Serial.println("ERROR: Cannot open file in SD card");
		}
	} else {
		Serial.println("File already exists, data will be appended");
	}
    // Write data to file -----------------------------------
    File dataFile = SD.open(FILENAME, FILE_WRITE);
	if (dataFile) { // if the file is available, write to it:
		digitalWrite(SD_BUSSY_LED_PIN, HIGH);
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
		digitalWrite(SD_BUSSY_LED_PIN, LOW);
		
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

void getFilename(char *filename) {
	DateTime now = rtc.now(); int year = now.year(); int month = now.month(); int day = now.day();
	filename[0] = '2';
	filename[1] = '0';
	filename[2] = (year-2000)/10 + '0';
	filename[3] = (year-2000)%10 + '0';
	filename[4] = month/10 + '0';
	filename[5] = month%10 + '0';
	filename[6] = day/10 + '0';
	filename[7] = day%10 + '0';
	filename[8] = '.';
	filename[9] = 't';
	filename[10] = 'x';
	filename[11] = 't';
	return;
}
