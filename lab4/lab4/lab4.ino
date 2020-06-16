#define DDR_DISPLAY DDRC
#define PORT_DISPLAY PORTC
#define PIN_DISPLAY PINC
#include "7segment.h"

#define BUZ 13
#define BUT1 53
#define BUT2 52
#define BUT3 51
#define BUT4 50

byte mode = 0;
byte startBuzzing = 0;
int counter = 0;
bool turnOnSegment;

int counterSegments = 0;
unsigned int timeToWait = 0;

struct Time
{
  unsigned char second, minute, hour;
  bool start;
};
Time Time1 = {0, 0, 0, false};
Time Time2 = {0, 0, 0, false};

Time *currentTimer;

ISR(TIMER1_COMPA_vect)
{
  //  Display update every 1ms
  if (counterSegments != 0) {
    digitalWrite(digits[counterSegments - 1], LOW);
  } else {
    digitalWrite(digits[5], LOW);
  }
  setDisplayPort(toShow[counterSegments]);
  digitalWrite(digits[counterSegments], HIGH);
  if(counterSegments == 1 || counterSegments == 3){
    PORT_DISPLAY &= ~(1 << 7);
  }
  if (++counterSegments == 6)
    counterSegments = 0;
    
  //  Update my own delay
  if (timeToWait > 0) {
    timeToWait--;
  }
  
  //  Tick every second
  if (counter++ == 500) {
    counter = 0;
    if (Time1.start) {
      if (Time1.second != 0)
      {
        Time1.second--;
      } else {
        Time1.second = 59;
        if (Time1.minute != 0)
        {
          Time1.minute--;
        } else {
          
          if (Time1.hour != 0) {
            Time1.hour--;
            Time1.minute = 59;
          }
          else {
            Time1.second = 0;
            Time1.start = false;
            startBuzzing = 1;
          }
        }
      }
    }
    if (Time2.start) {
      if (Time2.second != 0)
      {
        Time2.second--;
      } else {
        Time2.second = 59;
        if (Time2.minute != 0)
        {
          Time2.minute--;
        } else {
          
          if (Time2.hour != 0) {
            Time2.hour--;
            Time2.minute = 59;
          }
          else {
            Time2.second = 0;
            Time2.start = false;
            startBuzzing = 1;
          }
        }
      }
    }
  }
  //  Bip code
  else if (counter % 200 == 0) {
    if (startBuzzing != 0) {
      if (++startBuzzing % 2 == 0) {
        digitalWrite(BUZ, HIGH);
      } else if (BUZ == 30) {
        startBuzzing = 0;
        digitalWrite(BUZ, LOW);
      } else {
        digitalWrite(BUZ, LOW);
      }
    }
  }
}

void setup() {
  noInterrupts();

  // Timer every 10ms
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); //CTC mode & Prescaler @ 64
  TIMSK1 = (1 << OCIE1A); // дозвіл на переривання по співпадінню
  OCR1A = 0x4C;

  byte digits[6] = {24, 25, 26, 27, 28, 29};
  displayInit(digits);

  interrupts();

  pinMode(BUZ, OUTPUT);
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(BUT3, INPUT_PULLUP);
  pinMode(BUT4, INPUT_PULLUP);

  currentTimer = &Time1;
}


void loop() {
  if (timeToWait == 0) {
    if (digitalRead(BUT1) == LOW) {
      if (++currentTimer->hour == 25)
        currentTimer->hour = 0;
        
    }
    if (digitalRead(BUT2) == LOW) {
      if (++currentTimer->minute == 61)
        currentTimer->minute = 0;
    }
    if (digitalRead(BUT3) == LOW) {
      if (currentTimer == &Time1) {
        currentTimer = &Time2;
      } else {
        currentTimer = &Time1;
      }
    }
    if (digitalRead(BUT4) == LOW) {
      if (currentTimer->start) {
        currentTimer->start = false;
      } else {
        currentTimer->start = true;
      }
      startBuzzing = 0;
      digitalWrite(BUZ, LOW);
    }
    timeToWait = 80;
  }
  setTimeDigit(0, currentTimer->hour);
  setTimeDigit(1, currentTimer->minute);
  setTimeDigit(2, currentTimer->second);
}
