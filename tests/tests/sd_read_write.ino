/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

File myFile;

#define SDCARD_SS_PIN 21

void setup() {
  // Open Serial2 communications and wait for port to open:
  Serial2.begin(115200);
  while (!Serial2) {
    ; // wait for Serial2 port to connect. Needed for native USB port only
  }


  Serial2.print("Initializing SD card...");

  if (!SD.begin(SDCARD_SS_PIN)) {
    Serial2.println("initialization failed!");
    return;
  }
  Serial2.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial2.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial2.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial2.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial2.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial2.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial2.println("error opening test.txt");
  }
}

void loop() {
  // nothing happens after setup
}
