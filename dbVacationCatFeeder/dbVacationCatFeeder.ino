/*-----------------------------------------------------------------------------------------------------------
dbBoardsVacationCatFeeder.ino

  Summary:
    This is the edited firmware for the DB Boards Alarm Clock Kit. This adjusted program is used to set
    and dispence pet food portions on the given dates and times.
  
  Utilizing:
    Sparkfun RealTimeClock Library: https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library
    (LCD) Liquid Crystal Display Library: Standard in the Arduino environment
   
  Programmer:
    Duncan Brandt @ DB Boards, LLC
    Created: Jan 3, 2019
  
  Development Environment Specifics:
    Arduino 1.6.11
  
  Hardware Specifications:
    DB Alarm Clock Kit, DB5005

  Beerware License:
    This program is free, open source, and public domain. The program is distributed as is and is not
    guaranteed. However, if you like the code and find it useful I would happily take a beer should you 
    ever catch me at the local.
*///---------------------------------------------------------------------------------------------------------
#include <LiquidCrystal.h>                 // https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library
LiquidCrystal lcd(8,11,5,4,3,2);           // Initialize the LCD for the LCD RTC UNO Configuration
#include <SparkFunDS1307RTC.h>             // Standard in Arduino
#include <Wire.h>                          // We also need the Wire.h library for the Real Time Clock
#include <Servo.h>                         // This library controls the servo motors 
//-----------------------------------------------------------------------------------------------------------
// Instance names for the six trap door servos
Servo servo1, servo2, servo3, servo4, servo5, servo6;
// These boolean variables are used to designate the state of the menu
boolean menu = false, alarmSet = false, alarmMax = false, alarmSound = false, snooze = false;
int aCheck[] = {0,0,0,0,0,0};
// These are the digital pin numbers for the three buttons and the buzzer
const int up = 16, select = 15, down = 14, buzzer = 10, servos = 0;
int lastButton = 0, lastMinute = 90;       // Stop the program from updating the screen and double clicks
int alarmCnt = 0;
int aHour[] = {1, 1, 1, 1, 1, 1};
int aMin[] = {1, 1, 1, 1, 1, 1};
int aPM[] = {1, 1, 1, 1, 1, 1};
int aMonth[] = {1, 1, 1, 1, 1, 1};
int aDay[] = {1, 1, 1, 1, 1, 1};
int menuState = 0, menuPosition = 0;       // Also used to navigate the menu program
int alarms[] = {5,0,0,1,1};                // Preset array used to create alarm {H,M,PM,M,D}
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
void setup(){                              // Initiate the LCD, the RTC, and the button and buzzer pins
  lcd.begin(16, 2);                        // LCD is 16x2 characters
  lcd.clear();                             // Clear the screen so it is blank
  rtc.begin();                             // Call rtc.begin() to initialize the library 
  rtc.set24Hour();                         // Use rtc.set24Hour to set to 24-hour mode
  pinMode(up, INPUT_PULLUP);               // Use internal pullup resistor for up button
  pinMode(select, INPUT_PULLUP);           // Use internal pullup resistor for select button
  pinMode(down, INPUT_PULLUP);             // Use internal pullup resistor for down button
  pinMode(buzzer, OUTPUT);                 // Initiate buzzer pin for digital output
  pinMode(servos, OUTPUT);                 // The MOSFET that controls Servo power for all of them
  servo1.attach(12);                       // Attaches the servos to their control pins
  servo2.attach(1);
  servo3.attach(13);
  servo4.attach(7);
  servo5.attach(9);
  servo6.attach(6);
  digitalWrite(servos, HIGH);              // Turn servos on and set them all to the open position
  servo1.write(135); 
  delay(2000);       
  servo2.write(45); 
  delay(2000);  
  servo3.write(45); 
  delay(2000);   
  servo4.write(45); 
  delay(2000);  
  servo5.write(45); 
  delay(2000);    
  servo6.write(45); 
  delay(2000); 
  digitalWrite(servos, LOW);
  while(digitalRead(select));             // After the middle button is selected, close the doors in order
  digitalWrite(servos, HIGH);
  servo1.write(45); 
  delay(2000);       
  servo2.write(135); 
  delay(2000);  
  servo3.write(135); 
  delay(2000);   
  servo4.write(135); 
  delay(2000);  
  servo5.write(135); 
  delay(2000);    
  servo6.write(135); 
  delay(2000); 
  digitalWrite(servos, LOW);
}
//-----------------------------------------------------------------------------------------------------------
void loop(){                               // Uses the menu states to update the display 
  if(!menu){                               // If the menu is not open
    showTime();                            // Show the clock display
    if(btnCheck() == 2) menu = true;       // If the select button is pressed change the menu state to true
  }
  else{                                    // If the menu state is true
    int button = btnCheck();               // Store the current button press in button
    updateMenu(button);                    // Use button to update the menu
  }
  alarmCheck();
}
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
void alarmCheck(){                // Conditions for sounding the Alarm Clock
  int servoMove = 0;
  if(alarmSet){                           // If the user has the alarm turned on
    for(int a = 0; a < 6; a++){
      if(rtc.hour() == aHour[a] && rtc.minute() == aMin[a] && rtc.month() == aMonth[a]
      && rtc.date() == aDay[a] && !aCheck[a]){
         aCheck[a] = 1;
         servoMove = a + 1;
      }
    }
    if(servoMove > 0){
      switch (servoMove){                 // Check all of the alarms and trigger when met
        case 1:
          digitalWrite(servos, HIGH);     // Turn on servos
          delay(10); 
          servo1.write(135);              // Open Servo 1
          delay(2000);
          digitalWrite(servos, LOW);      // Pull power from servos to avoid jitter.
          break;
        case 2:
          digitalWrite(servos, HIGH);
          delay(10);
          servo2.write(45); 
          delay(2000);
          digitalWrite(servos, LOW);
          break;
        case 3:
          digitalWrite(servos, HIGH);
          delay(10);
          servo3.write(45); 
          delay(2000);
          digitalWrite(servos, LOW);
          break;
        case 4:
          digitalWrite(servos, HIGH);
          delay(10);
          servo4.write(45); 
          delay(2000);
          digitalWrite(servos, LOW);
          break;
        case 5:
          digitalWrite(servos, HIGH);
          delay(10);
          servo5.write(45); 
          delay(2000);
          digitalWrite(servos, LOW);
          break;
        case 6:
          digitalWrite(servos, HIGH);
          delay(10);
          servo6.write(45); 
          delay(2000);
          digitalWrite(servos, LOW);
          break;
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void soundAlarm(){                        // Sound the buzzer like an alarm
  tone(buzzer, 262, 300);                 // Play a C note for 300 milliseconds
  delay(600);                             // Rest for an additional 300 milliseconds after the tone
}
//-----------------------------------------------------------------------------------------------------------
void showTime(){                          // Show the clock display and selected alarm options
  rtc.update();                           // Request the current time from the RTC unit
  if (rtc.minute() != lastMinute)         // If the minute has changed
  {
    lcd.clear();                          // Clear the display of old numbers
    lastMinute = int(rtc.minute());       // Update lastMinute value
    lcd.setCursor(0,0);                   // Set the cursor in the top left cell
    if(rtc.hour() > 12) lcd.print(String(rtc.hour() - 12)); // Fix 24 hour time for display
    else if(rtc.hour() == 0) lcd.print("12"); // If the clock is 0:00 make it 12:00
    else lcd.print(String(rtc.hour()));   // If it is AM display the hour as is
    lcd.print(":");                       // Print a ":" to seperate the hours and minutes
    if(rtc.minute() < 10) lcd.print("0"); // Add a 0 if the minutes are single digit
    lcd.print(String(rtc.minute()));      // Print the minutes
    if (rtc.hour() > 12) lcd.print("PM ");// Print PM for afternoon times
    else lcd.print("AM ");                // Print AM for morning times
    if(alarmSet){                         // If the alarm has been set by the user
      if(rtc.month() < 10) lcd.print("0");// Print the month next to the time with an extra 0 for single date
      lcd.print(String(rtc.month()));     // Print the actual month
      lcd.print("/");                     // Print "/" to seperate month and day
      if(rtc.date() < 10) lcd.print("0"); // Print extra zero for single digit day
      lcd.print(String(rtc.date()));      // Print date day
      lcd.print("/");                     // Print "/" to seperate day and year
      if(twoDigitYear().toInt() < 10) lcd.print("0"); // Print extra zero if year is single digit
      lcd.print(twoDigitYear());          // Print Year as last two digits
      lcd.setCursor(0,1);                 // Move down to the left side of the second/bottom line
      lcd.print(alarmCnt);                // Print the current number of feed times
      lcd.print(" Feed Times");           // Explain the digit
    }
    else{                                 // If the alarm is not set
      lcd.setCursor(0,1);                 // Set the cursor to the left edge of the second/bottom row
      if(rtc.month() < 10) lcd.print("0");// If the month is single digit print a leading zero
      lcd.print(String(rtc.month()));     // Print the month
      lcd.print("/");                     // Print "/" to seperate month and day
      if(rtc.date() < 10) lcd.print("0"); // If day is single digit, print a leading zero
      lcd.print(String(rtc.date()));      // Print the day
      lcd.print("/");                     // Print "/" to seperate day and year
      if(twoDigitYear().toInt() < 10) lcd.print("0"); // If year is single digit add a leading zero
      lcd.print(twoDigitYear());          // Print last two digits of year
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
String twoDigitYear(){                    // Change the 4 digit year down to 2
  int years = int(rtc.year());            // Store the 4 digit year in years
  while(years > 1000) years -= 1000;      // Subtract 1000 at a time until you have a 3 digit number
  while(years > 100) years -= 100;        // Subtract 100 at a time, until you have a two digit number
  return String(years);                   // Return the two digit number as a String
}
//-----------------------------------------------------------------------------------------------------------
int btnCheck(){                           // Return which button has been pressed without repeating press
  if(!digitalRead(up)){                   // If the up button is pressed
    if(lastButton == 1) return 0;         // If it was already pressed , return no button pressed "0"
    else{                                 // If this is a new press of the up button
      lastButton = 1;                     // Store it as the last button pressed
      return 1;                           // Return 1 for up
    }
  }
  else if(!digitalRead(select)){          // If the select button has been pressed
    if(lastButton == 2) return 0;         // If it was already pressed, return no button pressed "0"
    else{                                 // If this is a new press of the select button
      lastButton = 2;                     // Store it as the last button pressed
      return 2;                           // Return 2 for select
    }
  } 
  else if(!digitalRead(down)){            // If the down button has been pressed
    if(lastButton == 3) return 0;         // If it was already pressed, return no button pressed "0"
    else{                                 // If this is a new press of the down button
      lastButton = 3;                     // Store it as the last button pressed
      return 3;                           // Return 3 for down
    }
  }
  else{                                   // If nothing is pressed
    lastButton = 0;                       // The last button is 0 and ready for a new press
    return 0;                             // Return zero for no button press
  }
}
//-----------------------------------------------------------------------------------------------------------
void updateMenu(int btn){                 // Use the current button pressed to update the menu display
  String menuItems[] = {"Adjust Clock","Set Alarm","Show Clock"," "}; // These are the menu items + a blank
  int trigger = 0, selection = 0;         // Reset adjust menu counters
  boolean pmCheck = false;                // Create temporary pm check
  int timeNow[] = {0,int(rtc.minute()),int(rtc.hour()),int(rtc.day()),int(rtc.date()),
  int(rtc.month()),twoDigitYear().toInt()}; // Store the current time in the new time array
  delay(100);                             // Let the LCD breathe so it does not flicker
  lcd.clear();                            // Clear the Screen
  switch (menuState){                     // Depending on which menuState we are in
    case 0:                               // For the main menu display
      lcd.setCursor(0,0);                 // Set the cursor in the top left
      lcd.print(">");                     // Print a selection indicator/pointer ">"
      lcd.print(menuItems[menuPosition]); // Print the menu according to the users position
      lcd.setCursor(1,1);                 // Go to the start of the second/bottom line
      lcd.print(menuItems[menuPosition+1]);//Print the next menu item
      trigger = btnCheck();               // Save the latest button press as trigger
      if(trigger == 1 && menuPosition > 0) menuPosition--; // If down is clicked and an option, go down one
      else if(trigger == 2){              // If select is chosen
        menuState = menuPosition + 1;     // Change the current menu state to the current selected menu item
        menuPosition = 0;                 // Reset the menu position to the top
        if(menuState == 3){               // If Show Clock was chosen
          menu = false;                   // Turn the menu off
          menuState = 0;                  // Reset the menu to main for next time
          lastMinute--;                   // Change lastMinute to update clock display
        }
      }
      else if(trigger == 3 && menuPosition < 2) menuPosition++; // If up is hit and we can go up, go up one
      break;                              // End menu cycle
    case 1:                               // If the Adjust Clock menu has been selected
      selection = 0;                      // Reset the selection variable to the beginning
      while(selection < 3){               // While in the first screen
        delay(100);                       // Let the screen breath so it does not flicker
        lcd.clear();                      // Clear the display
        lcd.setCursor(0,0);               // Start in the top left
        if(timeNow[2] == 0) lcd.print("12"); // Make 0:00 12:00
        else if(timeNow[2] < 10) lcd.print("0"); // If the selected hour is less then 10 add a leading zero
        if(timeNow[2] > 12) lcd.print(timeNow[2] - 12); // Correct for 24 hour time
        else if(timeNow[2] != 0) lcd.print(timeNow[2]);       // Print the selected hour
        lcd.print(":");                   // Print ":" to seperate hour and minute
        if(timeNow[1]<10) lcd.print("0"); // If the selected minute is single digit print a leading zero
        lcd.print(timeNow[1]);            // Print selected minute
        if(timeNow[2] > 12) lcd.print(" PM"); // Print PM if the time is over 12
        else lcd.print(" AM");            // Print AM if before that
        lcd.setCursor(3*selection+1,1);   // Set the cursor below the current adjustment selection
        lcd.print("^");                   // Indicate selection with arrow up at it
        trigger = btnCheck();             // Store the current button in trigger
        if(trigger > 0){                  // If a button has been hit
          if(selection == 0){             // If the user is on the first selection, hours
            if(trigger == 1){             // If down is hit
              if(timeNow[2] > 1) timeNow[2]--; // Subtract one from the hours if it is not already 1
              else timeNow[2] = 12;       // If it is 1, make it 12
            }
            else if(trigger == 2) selection++; // If th select button is hit move to the next adjust item
            else{                         // If up is hit
              if(timeNow[2] < 12) timeNow[2]++; // Add one to hours if it is less the 12
              else timeNow[2] = 1;        // If hours is 12, make it 1
            }
          }
          else if(selection == 1){        // If the user is on the second selection, minutes
            if(trigger == 1){             // If down is hit
              if(timeNow[1] > 0) timeNow[1]--; // Subtract one from minutes if it is not zero
              else timeNow[1] = 59;       // If minutes are zero make them 59
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next adjust item
            else{                         // If up is hit
              if(timeNow[1] < 59) timeNow[1]++; // Add one to minutes if they are not 59
              else timeNow[1] = 0;        // If miniutes are 59 make them 0
            }
          }
          else if(selection == 2){        // If the user is on the third selection, AM/PM
            if(trigger == 1 || trigger == 3){ // If up or down are hit
              if(timeNow[2] > 12) timeNow[2] -= 12; // If PM time, make AM time
              else timeNow[2] += 12;      // If AM time, make PM time
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next screen on the first item
          }
        }
      }
      while(selection < 7){               // While we are on the second screen adjust items
        String days[] = {" ","MO","TU","WE","TH","FR","SA","SU"}; // Abreviations for days of the week
        delay(100);                       // Let the display breath so it does not flicker
        lcd.clear();                      // Clear the display
        lcd.setCursor(0,0);               // Start in the top left
        lcd.print(days[timeNow[3]]);      // Print the day of the week abreviation
        lcd.setCursor(3,0);               // Move to the next position
        if(timeNow[5]<10) lcd.print("0"); // If the month is single digit, print a leading zero
        lcd.print(timeNow[5]);            // Print the selected month
        lcd.print("/");                   // Seperate the month and day
        if(timeNow[4]<10) lcd.print("0"); // If the day is single digit, print a leading zero
        lcd.print(timeNow[4]);            // Print the day
        lcd.print("/");                   // Print "/" to seperate the day and the year
        if(timeNow[6]<10) lcd.print("0"); // If the year is single digit, print a leading zero
        lcd.print(timeNow[6]);            // Print the year
        lcd.setCursor(3*(selection-3)+1,1); // Set the cursor under the current adjustment item
        lcd.print("^");                   // Print an arrow to show the current selection
        trigger = btnCheck();             // Store the current button in trigger
        if(trigger > 0){                  // If a button has been hit
          if(selection == 3){             // If the user is currently adjusting the day of the week
            if(trigger == 1){             // If down is hit
              if(timeNow[3] > 1) timeNow[3]--; // Subtract one from the day of the week if it is not Monday
              else timeNow[3] = 7;        // If it is Monday, make it Sunday
            }
            else if(trigger == 2) selection++; // If select is hit go to the month selection
            else{                         // If up is hit
              if(timeNow[3] < 7) timeNow[3]++; // Add one to the day unless it is Sunday
              else timeNow[3] = 1;        // If it Sunday, make it Monday
            }
          }
          else if(selection == 4){        // If the user is currently adjusting the month
            if(trigger == 1){             // If down is hit
              if(timeNow[5] > 1) timeNow[5]--; // Subtract one from the month unless it is Jan
              else timeNow[5] = 12;       // If it is Jan make it Dec
            }
            else if(trigger == 2) selection++; // If select is hit, move to the day
            else{                         // If up is hit
              if(timeNow[5] < 12) timeNow[5]++; // Add one to the month if it is not Dec
              else timeNow[5] = 1;        // If it is Dec, make it Jan
            }
          }
          else if(selection == 5){        // If the user is currently adjusting the day
            if(trigger == 1){             // If down is hit
              if(timeNow[4] > 1) timeNow[4]--; // Subtract one from the day unless it is the first
              else timeNow[4] = 31;       // If it is the first, make it the 31st
            }
            else if(trigger == 2) selection++; // If select is hit, move to the year
            else{                         // If up is hit
              if(timeNow[4] < 31) timeNow[4]++; // Add one to the day unless it is the 31st
              else timeNow[4] = 1;        // If it is the 31st, make it the 1st
            }
          }
          else if(selection == 6){        // If the user is currently adjusting the year
            if(trigger == 1){             // If down is hit
              if(timeNow[6] > 0) timeNow[6]--; // Subtract one from the year unless it is 0
              else timeNow[6] = 99;       // If the year is 0 make it 99
            }
            else if(trigger == 2) selection++; // If select is hit, complete the clock adjustment process
            else{                         // If up is hit
              if(timeNow[6] < 99) timeNow[6]++; // Add one to the year if it is not 99
              else timeNow[6] = 0;        // If it is 99, make it 0
            }
          }
        }
      }
      menu = false;                       // Turn the menu off
      menuState = 0;                      // Reset the menu state to main
      menuPosition = 0;                   // Reset to the first menu position
      lastMinute--;                       // Adjust lastMinute to update the time display
      rtc.setTime(0,timeNow[1],timeNow[2],timeNow[3],timeNow[4],timeNow[5],timeNow[6]); // Set new time
      break;                              // End menu cycle
    case 2:                               // If Set Alarm was selected
      selection = 0;                      // Reset the selection item to the beginning
      if(alarms[0] > 12){
        alarms[0] -= 12; // Adjust alarm to 12 hour to edit
        alarms[2] = 1;
      }
      while(selection < 5){               // While the user is in the editing screen still
        delay(100);                       // Let the LCD breath so it does not flicker
        lcd.clear();                      // Clear the screen
        lcd.setCursor(0,0);               // Start in the top left
        if(alarms[0] < 10) lcd.print("0");// If the alarm hours are single digit, print a leading zero
        lcd.print(alarms[0]);             // Print alarm hours
        lcd.print(":");                   // Print ":" to sperate alarm hours and minutes
        if(alarms[1] < 10) lcd.print("0");// If the minutes are single digit, print a leading zero
        lcd.print(alarms[1]);             // Print the selected alarm minutes
        if(alarms[2]>0) lcd.print(" PM"); // If PM is selected print PM
        else lcd.print(" AM");            // If AM is selected print AM
        lcd.print(" ");               // Print snz: to display snooze selection
        if(alarms[3] < 10) lcd.print("0");
        lcd.print(alarms[3]);    
        lcd.print("/");         
        if(alarms[4] < 10) lcd.print("0");
        lcd.print(alarms[4]); 
        lcd.setCursor(3*selection+1,1);   // Set the cursur under the current adustment selection
        lcd.print("^");                   // Print an arrow to show the user their current selection
        trigger = btnCheck();             // Store the current button in trigger
        if(trigger > 0){                  // If a button has been pressed
          if(selection == 0){             // If we are on the alarm hours selection
            if(trigger == 1){             // If down is hit
              if(alarms[0]>1) alarms[0]--;// If the hours are not 1, subtract one from the hours
              else alarms[0] = 12;        // If the hours are 1, make them 12
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next adjustment item
            else{                         // If up is hit
              if(alarms[0] < 12) alarms[0]++; // If the hours are not 12, add one to the hours
              else alarms[0] = 1;         // If the hours are 12 make them 1
            }
          }
          else if(selection == 1){        // If we are on the alarm minutes selection
            if(trigger == 1){             // If down is hit
              if(alarms[1] > 0) alarms[1]--; // If minutes are not 0, subrtact one from minutes
              else alarms[1] = 59;        // If minutes are 0, make them 59
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next adjustment item
            else{                         // If up is hit
              if(alarms[1] < 59) alarms[1]++; // If minutes are not 59, add one to minutes
              else alarms[1] = 0;         // If minutes are 59, make them 0
            }
          }
          else if(selection == 2){        // If we are on the alarm AM/PM selection
            if(trigger == 1 || trigger == 3){ // If up or down are hit
              if(alarms[2] == 0) alarms[2] = 1; // If alarm is AM make it PM
              else alarms[2] = 0;         // If alarm is PM make it AM
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next adjustment item
          }
          else if(selection == 3){            // If we are on the alarm snooze selection
            if(trigger == 1){             // If down is hit
              if(alarms[3] > 1) alarms[3]--; // If month is not 1, subrtact one from minutes
              else alarms[3] = 12;        // If month is 0, make 31
            }
            else if(trigger == 2) selection++; // If select is hit, move to the next adjustment item
            else{                         // If up is hit
              if(alarms[3] < 12) alarms[3]++; // If minutes are not 59, add one to minutes
              else alarms[3] = 1;         // If minutes are 59, make them 0
            }
          }
          else if(selection == 4){            // If we are on the alarm snooze selection
            if(trigger == 1){             // If down is hit
              if(alarms[4] > 1) alarms[4]--; // If month is not 1, subrtact one from minutes
              else alarms[4] = 31;        // If month is 0, make 31
            }
            else if(trigger == 2){
              selection++; // If select is hit, move to the next adjustment item
              alarmCnt++;
            }
            else{                         // If up is hit
              if(alarms[4] < 31) alarms[4]++; // If minutes are not 59, add one to minutes
              else alarms[4] = 1;         // If minutes are 59, make them 0
            }
          }
        }
      }
      if(alarms[2] > 0) alarms[0] += 12;  // If alarm is pm, add 12 to the hours
      aHour[alarmCnt-1] = alarms[0];
      aMin[alarmCnt-1] = alarms[1];
      aPM[alarmCnt-1] = alarms[2];
      aMonth[alarmCnt-1] = alarms[3];
      aDay[alarmCnt-1] = alarms[4];
      menu = false;                       // Turn the menu off
      menuState = 0;                      // Reset to main menu
      menuPosition = 0;                   // Reset menu to fisr position for next time
      lastMinute--;                       // Adjust lastMinute to update time display
      alarmSet = true;                    // Change the state of the alarm to set
      if(alarmCnt > 6) alarmCnt = 6;
      break;                              // End menu cycle
  }
}
//-----------------------------------------------------------------------------------------------------------
