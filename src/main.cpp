// Arduino and mbed libaries. 
#include <Arduino.h>
#include <mbed.h>
#include "platform/Callback.h"
#include "events/EventQueue.h"
#include "platform/NonCopyable.h"
#include "rtos.h"

// Bluetooth libraries. 
#include "ble/BLE.h"
#include "ble/GattServer.h"
#include "BluetoothStatus.hpp"

// Driver librarie for all our different hardware peripherals. 
#include "Adafruit_IS31FL3731.h"
#include "AniMatrix.hpp"

#include "LSM9DS1.h"
#include "BARO.h"
#include "HTS.h"
#include "PDM.h"
#include "Arduino_APDS9960.h"

// If you're using the wing version
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731_Wing();
AniMatrix::MatrixAnimationHandler matrix_animation_handler; 

void setup() {
  // Setup the serial interface. 
  Serial.begin(9600);

  // Setting up the charlieplex led matrix
  if(!matrix.begin()){
    Serial.println("CharliePlex Matrix not found");
    while(1) 
      rtos::ThisThread::sleep_for(1000);
  }
  matrix_animation_handler.begin(&matrix);

}

void loop() {
  // Currenting doing nothing, other than waking up every second...
  matrix_animation_handler.set_next_animation(AniMatrix::SWIPE_DOWN);
  rtos::ThisThread::sleep_for(1000);
  // Currenting doing nothing, other than waking up every second...
  matrix_animation_handler.set_next_animation(AniMatrix::SWIPE_UP);
  rtos::ThisThread::sleep_for(1000);
  // Currenting doing nothing, other than waking up every second...
  matrix_animation_handler.set_next_animation(AniMatrix::SWIPE_LEFT);
  rtos::ThisThread::sleep_for(1000);

  // Currenting doing nothing, other than waking up every second...
  matrix_animation_handler.set_next_animation(AniMatrix::SWIPE_RIGHT);
  rtos::ThisThread::sleep_for(1000);
}