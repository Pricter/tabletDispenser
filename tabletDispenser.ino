/**
  Tablet dispenser
  Copyright (C) Veekshith Kodithyala 2023
*/

#include <DS3232RTC.h>
#include <Streaming.h>
#include <Servo.h>

Servo s1, s2, s3;
DS3232RTC myRTC;

void setup()
{
    pinMode(A0, INPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    s1.attach(9);
    s2.attach(8);
    s3.attach(7);
    Serial.begin(9600);
    Serial << F( "\n" __FILE__ "\n" __DATE__ " " __TIME__ "\n" );
    myRTC.begin();
  
    setSyncProvider(myRTC.get);
    Serial << F("RTC Sync");
    if (timeStatus() != timeSet) Serial << F(" FAIL!");
    Serial << endl;
}

void tabletDispense() {
    s3.write(90);

    s1.write(110);
    delay(200);
    s1.write(0);
    delay(1000);

    s2.write(130);
    delay(300);
    s2.write(0);
    delay(1000);

    s3.write(0);
}

void loop()
{
    int pot = analogRead(A0);
    int mappedValue = map(pot, 0, 1200, 1, 6);

    s1.write(0);
    s2.write(0);
    s3.write(0);

    static time_t tLast;
    time_t t;
    tmElements_t tm;

    if (Serial.available() >= 12) {
        // note that the tmElements_t Year member is an offset from 1970,
        // but the RTC wants the last two digits of the calendar year.
        // use the convenience macros from the Time Library to do the conversions.
        int y = Serial.parseInt();
        if (y >= 100 && y < 1000)
            Serial << F("Error: Year must be two digits or four digits!") << endl;
        else {
            if (y >= 1000)
                tm.Year = CalendarYrToTm(y);
            else    // (y < 100)
                tm.Year = y2kYearToTm(y);
            tm.Month = Serial.parseInt();
            tm.Day = Serial.parseInt();
            tm.Hour = Serial.parseInt();
            tm.Minute = Serial.parseInt();
            tm.Second = Serial.parseInt();
            t = makeTime(tm);
            myRTC.set(t);   // use the time_t value to ensure correct weekday is set
            setTime(t);
            Serial << F("RTC set to: ");
            printDateTime(t);
            Serial << endl;
            // dump any extraneous input
            while (Serial.available() > 0) Serial.read();
        }
    }

    t = now();
    if (t != tLast) {
        tLast = t;
        printDateTime(t);
        if(second(t) == 0) {
          if(hour(t) >= 9) {
            digitalWrite(2, HIGH);
          }
          if(hour(t) >= 14) {
            digitalWrite(3, HIGH);
          }
          if(hour(t) >= 17) {
            digitalWrite(4, HIGH);
          }
          if(hour(t) >= 21) {
            digitalWrite(5, HIGH);
          }
          Serial.print("Dispensing tablets: ");
          Serial.print(mappedValue);
          for(int i = 0; i < mappedValue; ++i) {
            tabletDispense();
          }
          digitalWrite(2, HIGH);
        }
        if(hour(t) == 9 && minute(t) == 0 && second(t) == 0) {
          Serial.print("Dispensing tablets: ");
          Serial.print(mappedValue);
          for(int i = 0; i < mappedValue; ++i) {
            tabletDispense();
          }
        }
        if(hour(t) == 14 && minute(t) == 0 && second(t) == 0) {
          Serial.print("Dispensing tablets: ");
          Serial.print(mappedValue);
          for(int i = 0; i < mappedValue; ++i) {
            tabletDispense();
          }
        }
        if(hour(t) == 17 && minute(t) == 0 && second(t) == 0) {
          Serial.print("Dispensing tablets: ");
          Serial.print(mappedValue);
          for(int i = 0; i < mappedValue; ++i) {
            tabletDispense();
          }
        }
        if(hour(t) == 21 && minute(t) == 0 && second(t) == 0) {
          Serial.print("Dispensing tablets: ");
          Serial.print(mappedValue);
          for(int i = 0; i < mappedValue; ++i) {
            tabletDispense();
          }
        }
        if(hour(t) == 0 && minute(t) == 0 && second(t) == 0) {
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
          digitalWrite(4, LOW);
          digitalWrite(5, LOW);
        }
        Serial << endl;
    }
}

// print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

// print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

// print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}
