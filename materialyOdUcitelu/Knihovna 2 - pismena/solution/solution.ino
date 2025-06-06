#include "funshield.h"
#include "input.h"

// map of letter glyphs
constexpr byte LETTER_GLYPH[] {
  0b10001000,   // A
  0b10000011,   // b
  0b11000110,   // C
  0b10100001,   // d
  0b10000110,   // E
  0b10001110,   // F
  0b10000010,   // G
  0b10001001,   // H
  0b11111001,   // I
  0b11100001,   // J
  0b10000101,   // K
  0b11000111,   // L
  0b11001000,   // M
  0b10101011,   // n
  0b10100011,   // o
  0b10001100,   // P
  0b10011000,   // q
  0b10101111,   // r
  0b10010010,   // S
  0b10000111,   // t
  0b11000001,   // U
  0b11100011,   // v
  0b10000001,   // W
  0b10110110,   // ksi
  0b10010001,   // Y
  0b10100100,   // Z
};
constexpr byte EMPTY_GLYPH = 0b11111111;

constexpr int positionsCount = 4;
constexpr unsigned int scrollingInterval = 300;


/** 
 * Convert a chararcter to a glyph. If character is not letter, empty glyph is displayed instead.
 * @param ch character to be displayed
 * return value: a glyph to be displayed using time multiplex
 * hint: use/extend your class (or functions) for multiplexind the display developed in previous labs
 */

byte charToGlyph(char ch)
{
  byte glyph = EMPTY_GLYPH;
  if (isAlpha(ch)) {
    glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
  }
  // similarly, you can extend this function for digits
  // if (isDigit(ch))
  //   glyph = ....[ ch - '0' ];
  // ....

  return glyph;
}


SerialInputHandler input;

void setup() {
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  input.initialize();
}


void loop() {
  input.updateInLoop();


}
