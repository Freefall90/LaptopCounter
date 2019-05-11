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
const byte rows[2] = {TOP_ROW, BOT_ROW};

Encoder dial(7, 6);
tmElements_t tm;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
bool parse=false;
bool config=false;

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

const int errorArray[2][8] = {    {0, 1, 0, 0, 0, 1, 0, 0}, //"E"
                                  {1, 1, 1, 0, 0, 1, 1, 0}}; //"r"

//This is basically a hack to use a number indexed array for displaying "Err"
const int errorValues[3] = {1, 1, 0};

int dayCount;
int today;
int yesterday;
bool resetDay = false;

void setup()
{
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

  pinMode(buttonPin, INPUT);

  pinMode(TOP_ROW, OUTPUT);
  pinMode(BOT_ROW, OUTPUT);

  // Open serial port
  Serial.begin(9600);

  //while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet)
  {
    setTime();
    Serial.println("Didn't have time from RTC. Setting...");
  }
  else
  {
    config = true;
    Serial.println("Time synced with RTC");
    Serial.println(config);
  //  dayCount
  }
  if(!config)
  {
    Serial.println("Error: Cannot set time");
    dayCount = 100; //force drawing all 3 digits
    while(true)
    {
      drawRow(errorValues, errorArray);
    }
  }



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
    while (true)
    {
      drawRow(errorValues, errorArray);
      delay(1000);
      turnAllOff();
      delay(500);
    }
  }

//Reset the day count when someone hits the button
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


//TURNING OFF FOR NOW - THIS IS WHAT DISPLAYS THE NUMBER
  drawNumber(dayCount); //testing

}

void drawNumber(int wholeNumber)
{
  int original = wholeNumber;
  int values[3];
  //int thousands = 0;
  int hundreds = 0;
  int tens = 0;
  int ones = 0;

  //Figure out the value of each place
  for(int i = 0; i < 3; i++)
  {
    values[i] = wholeNumber % 10;
    wholeNumber /= 10;
  }

  drawRow(values, numberArray);

}

//Drawing a row of LEDs at once
//This helps with brightness by lighting up
//multiple LEDs at once, rather than one at a time.
void drawRow(int valueArray[], int charArray[][8])
{
  //Basically, we're going to loop through the values array and set all the
  //LEDs for each bit
  for(int i = 0; i < 8; i++)
  {
    if(i != 3)
    {
      if(dayCount > 99)
        digitalWrite(DIG2[i % 4], charArray[valueArray[2]][i]);
      if(dayCount > 9)
        digitalWrite(DIG3[i % 4], charArray[valueArray[1]][i]);
    }
    if(i != 7)
      digitalWrite(DIG4[i % 4], charArray[valueArray[0]][i]);

    if(i < 4)
    {
      digitalWrite(TOP_ROW, HIGH);
      digitalWrite(BOT_ROW, LOW);
    }
    else
    {
      digitalWrite(TOP_ROW, LOW);
      digitalWrite(BOT_ROW, HIGH);
    }
  //  Serial.print("Loop: ");
  //  Serial.println(i);
  //  Serial.print("Segment value: ");
  //  Serial.println(numberArray[1][i]);
    //delay(500);
    if(i % 4 == 3)
    {
      delay(2);
      //delayMicroseconds(500);
      turnAllOff();
    }
  }
}

void turnAllOff()
{
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
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void setTime()
{
  if (getDate(__DATE__) && getTime(__TIME__))
  {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm))
    {
      config = true;
    }
  }
}
