constexpr int led[] {13, 12, 11, 10};
constexpr int ledAmount = sizeof(led)/sizeof(led[0]);

constexpr int but[] {A1, A2, A3};
constexpr int butAmount = sizeof(but)/sizeof(but[0]);

constexpr int debouncing = 20; //ms

// 7-segs
constexpr int latch_pin = 4;
constexpr int clock_pin = 7;
constexpr int data_pin = 8;

constexpr int glyphDigit[]
{ 0xc0, 0xf9, 0xa4, 0xB0, 0b10011001, 
  0x92, 0x82, 0xf8, 0x80, 0x90
};



/*bool nowGoingLeft = true; //for bounce() function
int bounce(int lastState, int period){
  if (nowGoingLeft){
    lastState++;
  }
  else{
    lastState--;
  }
  if (lastState == period - 1 || lastState == 0){
    nowGoingLeft = !nowGoingLeft;
  }
  return lastState;
}*/

class Debug{
  public:
  Debug(bool en){
    enabled_ = en;
  }
  void out(){}
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

/*class LEDs{
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

};*/

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
  private:
  unsigned long last_time_ = 0;
  unsigned long last_time_holding_true_ = 0;
  bool was_true_before_ = false;
};

class Cislice{
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
    up.setHwButton(0);
    down.setHwButton(1);
  }

  int Count(){
    counter += buttonDifference();
    return counter;
  }
  
  int Count(int multiplier, int max){
    counter += buttonDifference();
    if (counter < 0){
      counter = max - 1;
    }
    counter = counter % max;
    return counter;
  }
};

LED output[ledAmount];
Timer tim;
Cislice cislice;
CounterButtons cnt;


void setup() {
  Serial.begin( 9600);
  for (int i = 0; i < ledAmount; i++){
    output[i].setPin(ledAmount - i - 1);
  }
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
  int counter = cnt.Count(1, 10);
  cislice.writeGlyph(glyphDigit[counter%10], 0);
  if (tim.notBlockingDelay(200)){
    Serial.println(counter%10);
  }

}
/**
* v konstruktorech nevolat knihovni funkce co komunikuji s arduinem - to by se zavolalo, jeste nez se program spustil.
* nepouzivat globalni promenne
*/