#include "input.h"
//#include "funshield.h"
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
constexpr int pinChangePositionOfDigit = 2;
constexpr int pinCountUp = 0;
constexpr int pinCountDown = 1;
constexpr int baseWhat = 10;
constexpr int displayStartPos = 0x08;

constexpr int decimalDotPosition = 7;
constexpr int milisecInSecond = 1000;
constexpr int powMilis = 3;

constexpr int notValidNumForCounting = -1;
constexpr int timeHoldButtonCount = 1000;
constexpr int countInterval = 300;

constexpr int ConDecimalPlaces = 1;

class Debug{
  public:
  Debug(){
    enabled_ = doYouWantToDebug;
  }
  void out(byte variable){
    if (enabled_){
      Serial.println(variable);
    }
  }
  private:
  bool enabled_;
};

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
      set_led(ledAmount - 1 - i, shifted == 1);
    }
  }
  void set_exact_led(int ledNum){
    for (int i = 0; i < ledAmount; ++i){
      set_led(i, i == ledNum);
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

  bool isItTrueFor(unsigned long interval, bool whatShould){
    unsigned long now = millis();
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
  void timer_reset(){
    timer_ = millis();
  }
  unsigned long timer_get(){
    return lap_ + millis() - timer_;
  }
  void timer_remember(){
    lap_ = timer_get();
    timer_reset();
  }
  void timer_forget(){
    lap_ = 0;
    timer_reset();
  }
  private:
  unsigned long last_time_ = 0;
  unsigned long last_time_holding_true_ = 0;
  bool was_true_before_ = false;
  unsigned long timer_ = 0;
  unsigned long lap_ = 0;
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
  void setPin(int which){
    pin = but[which];
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

class ButtonsInterface{
  /**
  * is ment to create a mask of which buttons was pressed/hold based on previous setup
  */
  private:
  Button buttons[butAmount];
  enum ReactType {
    RISE,
    FALL,
    HOLD
  };
  ReactType reactions[butAmount];
  public:
  ButtonsInterface(){
    setHowWillReact(RISE);
  }
  void createButtons(){
    for (int i = 0; i < butAmount; i++){
      buttons[i] = (but[i]);
    }
  }
  void setHowWillReact(ReactType buttonReactions[butAmount]){
    for (int i = 0; i < butAmount; i++){
      reactions[i] = buttonReactions[i];
    }
  }
  void setHowWillReact(ReactType buttonReaction){
    for (int i = 0; i < butAmount; i++){
      reactions[i] = buttonReaction;
    }
  }
  void setHowWillReact(ReactType oneButtonReaction, int whichButton){
    setHowWillReact(oneButtonReaction, whichButton);
  }
  byte getMask(){
    byte result = 0;
    bool logicValue = 0;
    for (int i = 0; i < butAmount; i++){
      switch (reactions[i]){
        case RISE:
          logicValue = buttons[i].RisingEdge();
          break;
        case FALL:
          logicValue = buttons[i].FallingEdge();
          break;
        case HOLD:
          logicValue = buttons[i].Pressed();
          break;
      }
      byte shifted = (byte)logicValue << i;
      result = result | shifted;
    }
    return result;
  }

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

  SmartButton up = (pinCountUp);
  SmartButton down = (pinCountDown);

  int buttonDifference(){
    return  up.handleButton() - down.handleButton();
  }

  public:
  Counter(){
    //changePosB.setPin(pinChangePositionOfDigit);
    // nevim, jak jde spocitat maximum lip a nazacatku to podle mne zas tak nevadi - cyklus probehne malokrat
    max = 1;
    for (int i = 0; i < displayAmount; i++){
      max *= baseWhat;
    }
    position = 0;
    posExpBase = 1;
    counter = 0;

    // up.setHwButton(pinCountUp);
    // down.setHwButton(pinCountDown);
  }

  int Count(){
    counter += buttonDifference();
    return counter;
  }
  
  int Count(int multiplier, int max){
    counter +=  multiplier * buttonDifference();
    if (counter < 0){
      counter = max + counter;
    }
    counter = counter % max;
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
  int movingTextPos;
  Timer tim;
  int posInString;
  int leftToDisplay;
  public:
  Display(){
    pos = 0;
    leadingZeros = false;
    smallestToDisplay = 0;
    movingTextPos = 0;
    resetStrPosition();
  }
  int getGlyph(int ch){
    byte glyph = displayBlack;
    if (isAlpha(ch)) {
      glyph = LETTER_GLYPH[ ch - (isUpperCase(ch) ? 'A' : 'a') ];
    }
    return glyph;
  }

  void resetStrPosition(){
    posInString = -displayAmount;
    leftToDisplay = notValidNumForCounting;
  }
  bool movingText(const char* word, int time){
    // returns whether it needs new word to display
    
    if (tim.notBlockingDelay(time)){
      posInString++;

      if (leftToDisplay == 0){
        return true;
      }else if(leftToDisplay > 0){
        leftToDisplay--;
      }
      if(leftToDisplay == notValidNumForCounting && *(word + posInString + displayAmount) == '\0'){
        leftToDisplay = displayAmount -1;
      }
    }
    set(word, posInString);

    return false;
  }

  void set(const char* word, int ofset){
    bool isEnd = false;
    int i = 0;
    while(i < displayAmount){
      int indexToData = displayAmount - i - 1;
      if (*word == '\0'){
        isEnd = true;
      }
      
      if (ofset > 0 ){
        word++;
        ofset--;
      }
      else if (ofset == 0){
        if (isEnd){
          data[indexToData] = displayBlack;
        }
        else{
          data[indexToData] = getGlyph(*word);
        }
        i++;
        word++;
      }
      else{
        data[indexToData] = displayBlack;
        i++;
        ofset++;
      }
    }
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
  void setDecimal(int n, int decPlaces){
    smallestToDisplay = decPlaces;
    
    set(n, decPlaces == 0 ? 0b0 : 0b1 << decPlaces);
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

class StopWatch{
  private:
  //v zadani je stejne zadano jake cislo tlacitka dela co, takze masky pro tlacitka je podle ne uplne validni
  byte bRun = 0b1; //button from stopped to running
  byte bLap = 0b10; //button from running to lapped
  byte bRes = 0b100; //button to reset timer 
  Timer tim;
  int millisPerUnit;
  long timeToDisplay;
  enum State {
    STOP,
    RUNN,
    LAPP
  };
  State stateNow;
  public:
  StopWatch(){
    millisPerUnit = milisecInSecond;
    stateNow = STOP;
  }
  void setDecimalPlaces(int places){
    millisPerUnit = milisecInSecond;
    for (int i =0; i < places; i++){
      millisPerUnit /= baseWhat;
    }
  }
  void handleStateCange(byte buttonValues){
    switch (stateNow){
      case STOP:
        stopped(buttonValues);
        break;
      case RUNN:
        running(buttonValues);
        break;
      case LAPP:
        lapped(buttonValues);
        break;
    }
  }
  void stopped(byte but){
    if ((but & bRes) == bRes){
      timeToDisplay = 0;
      tim.timer_forget();
    }
    if ((but & bRun) == bRun){
      stateNow = RUNN;
      tim.timer_reset();
    }
  }
  void running(byte but){
    timeToDisplay = tim.timer_get();
    if ((but & bRun) == bRun){
      stateNow = STOP;
      tim.timer_remember();
    }
    else if((but & bLap) == bLap){
      stateNow = LAPP;
    }
  }
  void lapped(byte but){
    if((but & bLap) == bLap){
      stateNow = RUNN;
    }
  }
  long read(){
    return timeToDisplay;
  }
  long read(int decPlaces){
    setDecimalPlaces(decPlaces);
    return timeToDisplay  / millisPerUnit;
  }
  int usedDigits(){
    long time = read();
    int i = 0;
    while(time > 0){
      time /= baseWhat;
      i++;
    }
    return i;
  }
};

LED output[ledAmount];
Timer tim;
Counter counter;
Display displayAll;
ButtonsInterface butMask;
StopWatch stopwatch;
Digit digit;
//SerialInputHandler serialInput;
Debug debug;
//Digit digit;
//CounterButtons cnt;


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
  for (int i = 0; i < ledAmount; i++){
    output[i].setPin(ledAmount - i - 1);
    output[i].set_led(false);
  }
  displayAll.showLeadingZeros(false, ConDecimalPlaces + 1);
  butMask.createButtons();
  
  //serialInput.initialize();
}
void loop() {
  /*stopwatch.handleStateCange(butMask.getMask());
  
  int decPlaces = displayAmount + powMilis - (stopwatch.usedDigits());
  if (decPlaces > displayAmount - 1){
    decPlaces = displayAmount - 1;
  }
  displayAll.setDecimal(stopwatch.read(decPlaces), decPlaces);
  
  displayAll.loop();*/
  stopwatch.handleStateCange(butMask.getMask());
  displayAll.setDecimal(stopwatch.read(ConDecimalPlaces), ConDecimalPlaces);

  //displayAll.set(counter.getCounter(), 0);
  displayAll.loop();
}
/**
* v konstruktorech nevolat knihovni funkce co komunikuji s arduinem - to by se zavolalo, jeste nez se program spustil.
* nepouzivat globalni promenne
*/