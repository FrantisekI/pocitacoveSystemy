#include "funshield.h"

// setup is called just once at the beginning
void setup() {
  // Always use constants from funshield whenever possible...
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, OFF);
}


// the main loop is called repeatedly by the bootstrap (main) code
void loop() {

  /*
   * Here goes your main code that controls the Arduino.
   * Do not use delay() nor block the main loop by other means.
   */

}
