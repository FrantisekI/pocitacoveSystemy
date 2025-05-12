constexpr bool doYouWantToDebug = false;

constexpr int led[] {13, 12, 11, 10};
constexpr int ledAmount = sizeof(led)/sizeof(led[0]);

constexpr int but[] {A1, A2, A3};
constexpr int butAmount = sizeof(but)/sizeof(but[0]);

constexpr int debouncing = 20; //ms

// 7-segs
constexpr int latch_pin = 4;
constexpr int clock_pin = 7;
constexpr int data_pin = 8;

constexpr byte glyphDigit[]
{ 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 
  0x92, 0x82, 0xf8, 0x80, 0x90
};
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

constexpr int decimalPlaces = 1;
constexpr int decimalDotPosition = 7;

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
    auto now = millis();
    if (now >= last_time_ + interval){
      last_time_ += interval;
      return true;
    }
    return false;
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
    writeGlyphBitmask(glyph, 0x08 >> pos);
  }
  void writeDigit(int number, int pos){
    writeGlyph(glyphDigit[number], pos);
  }
};

/*class Buttons{
  public:
  void initButtons(){
    for (int i = 0; i < butAmount; ++i){
      pinMode(but[i], OUTPUT);
    }
  }

  bool isPressed(int button){
    return !digitalRead(but[button]);
  }

  bool wasJustPressed(int button){
    bool pressed = isPressed(button);
    if (pressed && !lastState[button]){
      lastState[button] = true;
      return true;
    }
    lastState[button] = pressed;
    return false;
  }
  private:
  bool lastState[butAmount];
};*/

class Button{
  public:
  Button(int which){
    pin = but[which];
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
  * but it might cause preformence issues
  */
  private:
  Debug cc;
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
      buttons[i].setPin(i);
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
      cc.out(shifted);
      result = result | shifted;
    }
    return result;
  }

};

class SmartButton{
  private:
  enum States {
    PRESS,
    HOLD
  };
  enum States state;
  Button button;
  Timer tim;

  public:
  SmartButton(){
    state = PRESS;
  }
  void setHwButton(int id){
    button.setPin(id);
  }

  int handleButton(){
    if (state == PRESS){
      if (button.RisingEdge()){
        return 1;
      } else if (tim.isItTrueFor(1000, button.Pressed())){
        state = HOLD;
        return 1;
      }
    }
    else{
      if (button.Pressed()){
        if (tim.notBlockingDelay(300)){
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

class CounterButtons{
  private:
  int counter;
  SmartButton up;
  SmartButton down;

  int buttonDifference(){
    return  up.handleButton() - down.handleButton();
  }
  public:
  CounterButtons(){
    counter = 0;
    up.setHwButton(pinCountUp);
    down.setHwButton(pinCountDown);
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
};

class CouterDisplay{
  private:
  Digit digit;
  CounterButtons cnt;
  Button changePosB;
  int counter;
  int max;
  int position;
  int posExpBase;

  public:
  CouterDisplay(){
    changePosB.setPin(pinChangePositionOfDigit);
    // nevim, jak jde spocitat maximum lip a nazacatku to podle mne zas tak nevadi - cyklus probehne malokrat
    max = 1;
    for (int i = 0; i < displayAmount; i++){
      max *= baseWhat;
    }
    position = 0;
    posExpBase = 1;
    counter = 0;
  }
  void display(){
    int whatToDisplay = (counter % (posExpBase * baseWhat)) / posExpBase;
    digit.writeDigit(whatToDisplay, position);
  }
  void handleInput(){
    counter = cnt.Count(posExpBase, max);
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
  /*void set(int n){
    set(n, 0);
  }*/
  int getGlyph(int c){
    if( isalpha(c) && ((c - 'a') < letterAmount) && c >= 'a'){
      return LETTER_GLYPH[ c - 'a' ];
    }
    else {
      return displayBlack;
    }
  }
  void movungText(){

  }
  void set(const char* word){
    bool isEnd = false;
    for (int i = 0; i < displayAmount; i++){
      int indexToData = displayAmount - i - 1;
      if (word[i] == '\0'){
        isEnd = true;
      }
      if (isEnd){
        data[indexToData] = displayBlack;
      }
      else{
        data[indexToData] = getGlyph(word[i]);
      }
    }
  }
  void set( int n, byte maskForDots ){
    for (int i = 0; i < displayAmount; i++){
      byte dot = (maskForDots % 2) << decimalDotPosition ^ displayBlack; //jinou nez dvojkovou soustavu nebude nikdo potrebovat
      
      //dot = dot ^ 0xff;
      maskForDots /= 2;
      int index = n % baseWhat;
      
      if (n == 0 && i > smallestToDisplay && !leadingZeros){
        data[i] = displayBlack & dot;
      }
      else{
        data[i] = glyphDigit[index] & dot;
      }
      n = n / baseWhat;
    }
  }
  void loop(){
    digit.writeGlyph(data[pos], pos);
    pos++;
    if (pos >= displayAmount){
      pos = 0;
    }
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
  int timeToDisplay;
  enum State {
    STOP,
    RUNN,
    LAPP
  };
  State stateNow;
  public:
  StopWatch(){
    millisPerUnit = 1;
    for (int i =0; i < 3 - decimalPlaces; i++){
      millisPerUnit *= baseWhat;
    }
    stateNow = STOP;
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
    timeToDisplay = tim.timer_get() / millisPerUnit;
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
  int read(){
    return timeToDisplay;
  }
};

LED output[ledAmount];
Timer tim;
CouterDisplay counterDisplay;
Display displayAll;
ButtonsInterface butMask;
StopWatch stopwatch;
Digit digit;
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
  displayAll.showLeadingZeros(false, decimalPlaces);
  butMask.createButtons();
}
void loop() {
  //int counter = cnt.Count(1, baseWhat);
  //digit.writeGlyph(glyphDigit[counter%baseWhat], 0);
  stopwatch.handleStateCange(butMask.getMask());
  //counterDisplay.display();
  displayAll.set("NaZ");
  displayAll.loop();
}
/**
* v konstruktorech nevolat knihovni funkce co komunikuji s arduinem - to by se zavolalo, jeste nez se program spustil.
* nepouzivat globalni promenne
*/