#include <Encoder.h>

#include <Time.h>
#include <TimeLib.h>

#include <DS1307RTC.h>
#include <Wire.h>

#define TOP_ROW A0
#define BOT_ROW A1

//#define DIG1A 11 //led pin 7
#define DIG2L A2 //led pin 8
#define DIG2R A3 //led pin 9
#define DIG2TB 4 //led pin 10
#define DIG2M 5 //led pin 11
#define DIG3L A4 //led pin 12
#define DIG3R 11 //led pin 13
#define DIG3TB 8 //led pin 14
#define DIG34M 9 //led pin 15
#define DIG4L 10 //led pin 16
#define DIG4R 13 //led pin 17
#define DIG4TB 12 //led pin 18
#define buttonPin 1

const byte DIG1[] = {};
const byte DIG2[] = {DIG2L, DIG2R, DIG2TB, DIG2M};
const byte DIG3[] = {DIG3L, DIG3R, DIG3TB, DIG34M};
const byte DIG4[] = {DIG4L, DIG4R, DIG4TB, DIG34M}; 

Encoder dial(7, 6);

//Order of segments is as follows:
//In most cases, the middle segment should be specified twice (stupid LED configuration in my particular clock)
//Also, it's common anode, so 0 is on and 1 is off
//         3
//      ------   
//     |      | 
//  1  |   4  | 2
//     |------|
//     |   8  |
//   5 |      | 6
//      ------
//         7

/*
const int zero[] =  {0, 0, 0, 1, 0, 0, 0, 1};
const int one[] =   {1, 0, 1, 1, 1, 0, 1, 1};
const int two[] =   {1, 0, 0, 0, 0, 1, 0, 0};
const int three[] = {1, 0, 0, 0, 1, 0, 0, 0};
const int four[] =  {0, 0, 1, 0, 1, 0, 1, 0};
const int five[] =  {0, 1, 0, 0, 1, 0, 0, 1};
const int six[] =   {0, 1, 0, 1, 0, 0, 0, 0};
const int seven[] = {1, 0, 0, 1, 1, 0, 1, 1};
const int eight[] = {0, 0, 0, 0, 0, 0, 0, 0};
const int nine[] =  {0, 0, 0, 0, 1, 0, 1, 0};
*/

const int numberArray[10][8] = {  {0, 0, 0, 1, 0, 0, 0, 1}, //zero
                                  {1, 0, 1, 1, 1, 0, 1, 1}, //one
                                  {1, 0, 0, 0, 0, 1, 0, 0}, //two
                                  {1, 0, 0, 0, 1, 0, 0, 0}, //three
                                  {0, 0, 1, 0, 1, 0, 1, 0}, //four
                                  {0, 1, 0, 0, 1, 0, 0, 0}, //five
                                  {0, 1, 0, 0, 0, 0, 0, 0}, //six
                                  {1, 0, 0, 1, 1, 0, 1, 1}, //seven
                                  {0, 0, 0, 0, 0, 0, 0, 0}, //eight
                                  {0, 0, 0, 0, 1, 0, 1, 0} }; //nine

int dayCount = 0;
int today;
int yesterday;
bool resetDay = false;

void setup() 
{
    // Open serial port
    Serial.begin(9600);
    
    while (!Serial) ; // wait until Arduino Serial Monitor opens
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet) 
      Serial.println("Unable to sync with the RTC");
    else
      Serial.println("RTC has set the system time");      

    // Set all used pins to OUTPUT
    // This is very important! If the pins are set to input
    // the display will be very dim.

    pinMode(DIG1, OUTPUT);
    pinMode(DIG2L, OUTPUT);
    pinMode(DIG2R, OUTPUT);
    pinMode(DIG2TB, OUTPUT);
    pinMode(DIG2M, OUTPUT);
    pinMode(DIG3L, OUTPUT);
    pinMode(DIG3R, OUTPUT);
    pinMode(DIG3TB, OUTPUT);
    pinMode(DIG34M, OUTPUT);
    pinMode(DIG4L, OUTPUT);
    pinMode(DIG4R, OUTPUT);
    pinMode(DIG4TB, OUTPUT);

//    pinMode(6, OUTPUT);
//    pinMode(7, OUTPUT);

    pinMode(buttonPin, INPUT);
  //  pinMode(i, OUTPUT);
    pinMode(TOP_ROW, OUTPUT);
    pinMode(BOT_ROW, OUTPUT);

//    int today = day();
//    int yesterday = day() - 1;
    today = day();
    yesterday = day() - 1;
    resetDay = false;
}

long positionDial = -999;
int buttonState = 0;

void loop()
{
  if (timeStatus() != timeSet)
  {
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
    delay(4000);
  }

  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH)
  {
    resetDay = true;
  }

  if(resetDay)
  {
    dayCount = 0;
    resetDay = false;
  }
  
  
  //Set up button and dial functionality
  long newPos = dial.read();
  if(newPos - 3 >  positionDial || newPos + 3 < positionDial)
  {
    Serial.print("Dial = ");
    Serial.println(newPos);

    if(newPos > positionDial + 3){dayCount++;}
    if(newPos < positionDial - 3){dayCount--;}
    
    positionDial = newPos;
  }

  //Check to see if we've advanced a day, and if so, update 'yesterday'
  //today = day();
  //Test with minutes
  today = second();
  
  if(today != yesterday + 1)
  {
    dayCount++;
    yesterday = today - 1;
    Serial.print("Incremented to ");
    Serial.println(dayCount);
  }

  

  
  //TURN EVERYTHING OFF
  digitalWrite(TOP_ROW, LOW);
  digitalWrite(BOT_ROW, LOW);

  digitalWrite(DIG1, HIGH);
  digitalWrite(DIG2L, HIGH);
  digitalWrite(DIG2R, HIGH);
  digitalWrite(DIG2TB, HIGH);
  digitalWrite(DIG2M, HIGH);
  digitalWrite(DIG3L, HIGH);
  digitalWrite(DIG3R, HIGH);
  digitalWrite(DIG3TB, HIGH);
  digitalWrite(DIG34M, HIGH);
  digitalWrite(DIG4L, HIGH);
  digitalWrite(DIG4R, HIGH);
  digitalWrite(DIG4TB, HIGH);
  

//TURNING OFF FOR NOW - THIS IS WHAT DISPLAYS THE NUMBER
//  drawNumber(dayCount);

//  drawDigit(numberArray[(dayCount % 10)], DIG2);
//  drawDigit(numberArray[(dayCount % 10)], DIG3);
//  drawDigit(numberArray[(dayCount % 10)], DIG4);

}

void drawNumber(int wholeNumber)
{
  int original = wholeNumber;
  int values[4];
  int thousands = 0;
  int hundreds = 0;
  int tens = 0;
  int ones = 0;

  //Figure out the value of each place
  for(int i = 0; i < 4; i++)
  {
    values[i] = wholeNumber % 10;
    wholeNumber /= 10;
  }
  
  //Thousands
  if(original > 999)
    drawDigit(numberArray[values[3]], DIG1);

  //Hundreds
  if(original > 99)
    drawDigit(numberArray[values[2]], DIG2);

  //Tens
  if(original > 9)
    drawDigit(numberArray[values[1]], DIG3);

  //Ones
  drawDigit(numberArray[values[0]], DIG4);
}


void drawDigit(int number[], byte digit[])
{
for (int j = 0; j < 8; j++)
  {
   // Serial.print("j=");
   // Serial.print(j);
   // Serial.println(number[j]);

   //Thousands case
   //Super special case here, because the display can ONLY display a 1 in the thousands place
   //Disabling. I need the IO pins, and we're never hitting 1000 days for this.
  /* if(digit == DIG1)
   {
    digitalWrite(TOP_ROW, HIGH);
    digitalWrite(BOT_ROW, LOW);
    digitalWrite(DIG1A, LOW);
    delayMicroseconds(100);
    digitalWrite(TOP_ROW, LOW);
    digitalWrite(BOT_ROW, HIGH);
    delayMicroseconds(100);
    digitalWrite(DIG1A, HIGH);
    continue;
   }
   */

    //Ones case
    if(digit == DIG4 && j == 7) {break;}

    //Tens and hundreds case
    if((digit == DIG3 || digit == DIG2) && j == 3) {j++;}

    if(j < 4)
    {
      digitalWrite(TOP_ROW, HIGH);
      digitalWrite(BOT_ROW, LOW);
    }
    else
    {
      digitalWrite(TOP_ROW, LOW);
      digitalWrite(BOT_ROW, HIGH);
    }
    digitalWrite(digit[j % 4], number[j]);
    
    delayMicroseconds(100);
    //delay(1000);
    
    
    digitalWrite(digit[j % 4], HIGH);
  }
}


void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}