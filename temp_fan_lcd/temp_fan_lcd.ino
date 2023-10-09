/*
This is a program for a temperaturesensor and fan/cooling system.
The temperature will be monitored and LEDs will turn on when
certain threashholds are reached. 
If the temperature become critical the fan will start and cool the 
system down. The fan runs for a minimum amount of time even if the 
temperature moves below the threashhold.
Also, the data will be presented on a fancy LCD screen.
*/



// LCD library and the pin layout it will use. 
#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,10,11,12,13);

// Time taking stuff. For making the fan run even if the temp drops.
// For the millis() function
  unsigned long delayStart = 0; // an unsigned long to hold the millis() later
  unsigned long DELAY_TIME = 5000; // Minimum period of the the fan will run
  bool delayRunning = false; // To keep track on if the delay is running
  bool fanOn = false; // To keep track on if the fan is running
  
  /*Honestly, realized now that I just need one of the bools above.
  They keep track of the same thing. delayRunning==fanOn
  However I keep both for clarity in the code (and maybe laziness..)
  */
// Assign analog pin for temp-sensor
  const int tempPin = A0;
  
// Temperature limits, made them const for safety. Can't be changed.
// Arbitrary values, made up for trials in my kitchen and with body heat.
  const float tempSafe = 23.0;
  // const float tempWarn = 23.5; Realized I did not need this for yellow led: green<yellow<red
  const float tempHigh = 24.5;
  const int fanThreshhold = 25;

  
void setup() {
  
  int pinNr; // Just an integer variable to use in the set-pin-loop
  Serial.begin(9600);
  
  // LCD setups.
  lcd.begin(16,2); // Declare the size
  pinMode(6,OUTPUT); // Pin for Contrast, PWM instead of Potentiometer
  analogWrite(6,20); // Value of contrast
  
// Set all digital pins used(LEDs + fan) to output and low
  for(pinNr=2;pinNr<=5;pinNr++){
    pinMode(pinNr, OUTPUT);
    digitalWrite(pinNr,LOW);
  }
}


void loop() {
  
// Calculate temperature from sensor, in Celsius.
  int sensorVal = analogRead(tempPin); 
  float voltage = (sensorVal/1024.0)*5.0;
  float temperature = (voltage - 0.5)*100;

//Check if the delay/fan is running and if the delay is done. 
//and if that's the case, stop the delay/fan.
  if(delayRunning == true && (millis() >= (delayStart + DELAY_TIME))){
    delayRunning = false;
    digitalWrite(5,LOW);
    fanOn = false;
   }

// Set up the LEDs and the "fan" to react on the temperature readings
  if(temperature<tempSafe){
    digitalWrite(2,HIGH); // Temp Safe, green LED on,  and the others off
    digitalWrite(3,LOW);
    digitalWrite(4,LOW);
  }
  // Temp higher than tempSafe and lower than tempHigh
   else if(temperature<tempHigh && temperature>tempSafe){
    digitalWrite(2,LOW);
    digitalWrite(3,HIGH); // Yellow LED on, and the others off
    digitalWrite(4,LOW);
  }
  
  else if(temperature>tempHigh){
    digitalWrite(2,LOW);
    digitalWrite(3,LOW);
    digitalWrite(4,HIGH); //Temp over tempHigh, Red LED ON and the others off
  }
  //Checks if the fan should be started
  if((temperature>fanThreshhold) && (fanOn == false)){
    delayStart = millis(); // Sets delay start to the present time
    delayRunning = true; // Tracks that the delay is running
    digitalWrite(5,HIGH); // Starts the fan
    fanOn = true; // Tracks that the fan is running
  }
 
// Print temperature to monitor for insights and debugging
Serial.println(temperature);
Serial.println(fanOn);
/*
LCD operations
To display the temp and if the fan is running or not.
*/
lcd.clear(); // Clearing the leftover characters from last loop
lcd.setCursor(0,0); // Resets where we start writing, as we changed it beneath 
lcd.print(temperature); // writes the latest temp-reading
lcd.print("C");
// print which level of warning the temperature indicates.
lcd.setCursor(6,0);
  if(temperature<tempSafe){
    lcd.print(" Temp Safe");
  }
  else if(temperature<tempHigh && temperature>tempSafe){
    lcd.print(" Temp Warn");
  }
  else if(temperature>tempHigh && temperature<fanThreshhold){
    lcd.print(" Too High!");
  }
  else if(temperature>fanThreshhold){
    lcd.print(" IT BURNS!");
  }

lcd.setCursor(0,1); // set print-start at first column, second row
lcd.print("Fan ");
  if(fanOn==1){ // Get Bool-value to check if the fan is running
    lcd.print("running");
  }
  else{
    lcd.print("not running"); // If the fan isn't running
  }
/*
Only read/loop every 1 sec to prevent fast fluctuations 
which will flicker the lights and mess with the lcd.print due
to fast changes.
*/
delay(1000);

}
