/*Author: binarymode.android@gmail.com
   MIT-Licence
   You will need to install this library on your PC before compiling this sketch:
   https://github.com/z3t0/Arduino-IRremote
*/
#include <SoftwareSerial.h>
#include <IRremote.h>
#include <stdio.h>
#include <stdlib.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

IRsend irsend;
//----------------------------------------------------------------
int dataPosition = 0;
const int dataUpperLimit = 300;
byte dataBytes[dataUpperLimit];
//-----------------------------------------------------------------
void setup() {
  //Serial.begin(9600);//Enable/disable console serial

  Serial.begin(9600);
  SerialBT.begin("BT2IR"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LED_BUILTIN, OUTPUT);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);

}
//-------------------------------------------------------------------
void loop() {
  /*if (Serial.available()) {
    SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    }
    delay(20);*/
  if (SerialBT.available() > 0) {
    dataBytes[dataPosition] = SerialBT.read();
    //stop reading at this point
    if (dataPosition > 0 && (dataPosition % 2) != 0 && dataBytes[dataPosition - 1] == 255 && dataBytes[dataPosition] == 255) {
      //create array
      unsigned int carrierFrequency = 0;
      unsigned int dataBuffer[(dataPosition / 2) - 1];
      int byteCounter = 0;
      int countadiloops = 0;
      for (int i = 0; i < dataPosition / 2; i++) {
        int currentInt = word(dataBytes[byteCounter + 1], dataBytes[byteCounter]);
        if (i == 0) {
          carrierFrequency = currentInt;
        } else {
          dataBuffer[i - 1] = currentInt;
        }
        byteCounter = byteCounter + 2;
        countadiloops ++;
      }
      //send IR
      Serial.println();
      Serial.println("Received:");
      for (int i = 0; i < countadiloops; i++)
      {
        Serial.print(dataBuffer[i], DEC);
        Serial.print(',');
      }
      Serial.println();
      
      digitalWrite(LED_BUILTIN, HIGH);
      int carrierKhz = carrierFrequency / 1000;
      Serial.print("Freq:");
      Serial.println(carrierKhz);

      //irsend.sendRaw(dataBuffer, dataPosition / 2, carrierKhz);
      irsend.sendRaw(dataBuffer, ((dataPosition / 2) - 1), carrierKhz);
      digitalWrite(LED_BUILTIN, LOW);
      //reset reading data
      dataPosition = 0;
      memset(dataBytes, 0, dataUpperLimit);
      SerialBT.write(1);
    } else {
      dataPosition++;
    }
  }
}
