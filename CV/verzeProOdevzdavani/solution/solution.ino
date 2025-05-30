#include "funshield.h"
constexpr bool doYouWantToDebug = false;

constexpr int led[] {led1_pin, led2_pin, led3_pin, led4_pin};

constexpr int ledAmount = sizeof(led)/sizeof(led[0]);

constexpr int but[] {button1_pin, button2_pin, button3_pin};


constexpr int butAmount = sizeof(but)/sizeof(but[0]);

constexpr int debouncing = 20; //ms

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
constexpr int letterAmount = sizeof(LETTER_GLYPH)/sizeof(LETTER_GLYPH[0]);

constexpr byte displayBlack = 0xFF;
constexpr int displayAmount = 4;

constexpr int pinCountUp = 0;
constexpr int pinChangePositionOfDigit = 1;
constexpr int pinEvaluate = 2;

constexpr int baseWhat = 10;
constexpr int displayStartPos = 0x08;

constexpr int decimalDotPosition = 7;
constexpr int milisecInSecond = 1000;

constexpr int notValidNumForCounting = -1;
constexpr int timeHoldButtonCount = 1000;
constexpr int countInterval = 300;

constexpr int ConDecimalPlaces = 1;

constexpr int waitBeforeShow = 1; //in seconds
constexpr int timeToShow = 2;
constexpr int maxRndNumber = 10000;
constexpr int minLevel = 1;
constexpr int maxLevel = 15;


class LED{
  private:
  int pinId;
  bool isOn;
  public:
  LED(int id){
    pinId = id;
    isOn = false;
  }
  LED(){
    pinId = 0;
    isOn = false;
  }
  void setPin(int id){
    pinId = id;
  }
  void set_led(bool toOn){
    if (toOn){
      digitalWrite(led[pinId], LOW);
    }
    else{
      digitalWrite(led[pinId], HIGH);
    }
  }
};

class LEDs{
  public:
  void set_led(int which, bool toOn){
    if (toOn){
      digitalWrite(led[which], LOW);
    }
    else{
      digitalWrite(led[which], HIGH);
    }
  }
  void set_led_by_binary_number(int leds){
    for (int i = 0; i < ledAmount; ++i){
      int shifted = (leds >> i) & 1;
      set_led(i, shifted == 1);
    }
  }

};

class Timer {
  public:
  bool notBlockingDelay(unsigned long interval){
    unsigned long now = millis();

    if (now >= last_time_ + interval){
      last_time_ += interval;
      return true;
    }
    return false;
  }
  void reset_delay(){
    last_time_ = millis();
  }

  bool isItTrueFor(unsigned long interval, bool whatShould){
    auto now = millis();
    if (!whatShould || !was_true_before_){
      last_time_holding_true_ = now;
      was_true_before_ = whatShould;
    }
    else if (now >= last_time_holding_true_ + interval){
      last_time_holding_true_ += interval;
      last_time_ = last_time_holding_true_;
      return true;
    }
    return false;
  }

  private:
  unsigned long last_time_ = 0;
  unsigned long last_time_holding_true_ = 0;
  bool was_true_before_ = false;
};

class Digit{
  public:
  void writeGlyphBitmask( byte glyph, byte pos_bitmask) {
    digitalWrite( latch_pin, LOW);
    shiftOut( data_pin, clock_pin, MSBFIRST, glyph);
    shiftOut( data_pin, clock_pin, MSBFIRST, pos_bitmask);
    digitalWrite( latch_pin, HIGH);
  }
  void writeGlyph(byte glyph, int pos){
    writeGlyphBitmask(glyph, displayStartPos >> pos);
  }
  void writeDigit(int number, int pos){
    writeGlyph(digits[number], pos);
  }
  void writeLetter(char letter, int pos){
    writeGlyph(LETTER_GLYPH[letter - (isUpperCase(letter) ? 'A' : 'a') ], pos);
  }
};

class Button{
  public:
  Button(int which){
    pin = which;
    lastState = false;
  }
  Button(){
    pin = but[0];
    lastState = false;
  }


  bool Pressed(){
    return !digitalRead(pin);
  }

  bool RisingEdge(){
    bool pressed = Pressed();
    if (pressed && !lastState){
      lastState = true;
      return true;
    }
    lastState = pressed;
    return false;
  }

  bool FallingEdge(){
    bool pressed = Pressed();
    if (!pressed && lastState){
      lastState = false;
      return true;
    }
    lastState = pressed;
    return false;
  }

  private:
  bool lastState;
  int pin;
};

class SmartButton : Button{
  private:
  enum States {
    PRESS,
    HOLD
  };
  enum States state;
  Timer tim;

  public:
  SmartButton(int id){
    state = PRESS;
  }

  int handleButton(){
    if (state == PRESS){
      if (RisingEdge()){
        return 1;
      } else if (tim.isItTrueFor(timeHoldButtonCount, Pressed())){
        state = HOLD;
        return 1;
      }
    }
    else{
      if (Pressed()){
        if (tim.notBlockingDelay(countInterval)){
          return 1;
        }
      }
      else{
        state = PRESS;
      }
    }
    return 0;
  }
};

class Counter{
  private:
  Button changePosB = (but[pinChangePositionOfDigit]);
  int counter;
  int max;
  int position;
  int posExpBase;

  SmartButton up = (but[pinCountUp]);
  //SmartButton down;

  int buttonDifference(){
    return  (int)up.handleButton();
  }

  public:
  Counter(){
    // nevim, jak jde spocitat maximum lip a nazacatku to podle mne zas tak nevadi - cyklus probehne malokrat
    max = baseWhat;
    
    position = 0;
    posExpBase = 1;
    counter = 0;

    //up.setHwButton(pinCountUp);
  }

  int Count(){
    counter += buttonDifference();
    return counter;
  }
  
  int Count(int multiplier, int max){
    int diff = buttonDifference();
    counter +=  multiplier * diff;

    if (diff != 0 && (counter % (multiplier*max)) / multiplier == 0){
      counter -= (multiplier * max);
    }
    return counter;
  }

  void handleInput(){
    counter = Count(posExpBase, max);
    if (changePosB.RisingEdge()){
      position++;
      posExpBase *= baseWhat;
      if (position >= displayAmount){
        position = 0;
        posExpBase = 1;
      }
    }
  }
  void reset(){
    counter = 0;
    position = 0;
    posExpBase = 1;
  }
  int getCounter(){
    return counter;
  }
  int getPos(){
    return position;
  }
};

class Display{
  private:
  int pos;
  byte data[displayAmount];
  Digit digit;
  bool leadingZeros;
  int smallestToDisplay;

  public:
  Display(){
    pos = 0;
    leadingZeros = false;
    smallestToDisplay = 0;
  }
  int getGlyph(int ch){
    byte glyph = displayBlack;
    if (isAlpha(ch)) {
      glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
    }
    return glyph;
  }

  void set( int n, byte maskForDots ){
    for (int i = 0; i < displayAmount; i++){
      byte dot = (maskForDots & 0b1) << decimalDotPosition ^ displayBlack;
      
      //dot = dot ^ 0xff;
      maskForDots = maskForDots >> 1;
      int index = n % baseWhat;
      
      if (n == 0 && i > smallestToDisplay && !leadingZeros){
        data[i] = displayBlack & dot;
      }
      else{
        data[i] = digits[index] & dot;
      }
      n = n / baseWhat;
    }
  }
  void setBlack(){
    for (int i = 0; i < displayAmount; i++){
      data[i] = displayBlack;
    }
  }
  void setDecimal(int n, int decPlaces){
    smallestToDisplay = decPlaces;
    
    set(n, /*decPlaces == 0 ? 0b0 : */ 0b1 << decPlaces);
  }
  void loop(){
    displayOne(pos);
    pos = (pos + 1) % displayAmount;
  }
  void displayOne(int position){
    digit.writeGlyph(data[position], position);
  }
  void showLeadingZeros(bool show, int lastToAlwaysDisplay){
    leadingZeros = show;
    smallestToDisplay = lastToAlwaysDisplay;
  }
};

class MemoryGame{
  private:
  Display display;
  Button evalBtn = (but[pinEvaluate]);
  enum State {
    STRT,
    SHOW,
    ENTR
  };
  State stateNow;
  Timer tim;
  Counter cnt;
  int randomNumber;
  int level;
  LEDs levelShow;


  public:
  MemoryGame(){
    stateNow = STRT;
    randomNumber = 0;
    display.showLeadingZeros(true, 0);
    level = minLevel;
  }
  void play(){
    display.loop();
    levelShow.set_led_by_binary_number(level);

    handleStateCange();
  }
  void handleStateCange(){
    switch (stateNow){
      case STRT:
        start();
        break;
      case SHOW:
        show_rnd_number();
        break;
      case ENTR:
        enter_number();
        break;
    }
  }
  void start(){
    display.setBlack();
    if (tim.notBlockingDelay(milisecInSecond * waitBeforeShow)){
      randomNumber = random(maxRndNumber);
      stateNow = SHOW;
    }
  }
  void show_rnd_number(){
    display.set(randomNumber, 0);
    if (tim.notBlockingDelay(milisecInSecond * timeToShow)){
      stateNow = ENTR;
      cnt.reset();
    }
  }
  void enter_number(){
    cnt.handleInput();
    int counter = cnt.getCounter();
    display.setDecimal(counter, cnt.getPos());

    if (evalBtn.Pressed()){
      if (counter == randomNumber){
        level += 1;
        if (level >= maxLevel){
          level = minLevel;
        }
      }
      else{
        level = minLevel;
      }
      stateNow = STRT;
      tim.reset_delay();

    }
  }
};


MemoryGame memGame;


void setup() {
  Serial.begin(9600);
  for (int i = 0; i < butAmount; ++i){
    pinMode(but[i], INPUT);
  }
  for (int i = 0; i < ledAmount; ++i){
    pinMode(led[i], OUTPUT);
  }
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
}
void loop() {
  memGame.play();
}