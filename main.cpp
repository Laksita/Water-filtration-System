// ENGR 111 Cornerstone Project Skeleton Code

#include <LiquidCrystal_I2C.h>

/**********************************************************/
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
/**********************************************************/

// Pin assignments
const int buttonPin = 3;
const int pumpPos = 4;
const int pumpNeg = 5;
const int valveNeg = 6;
const int valvePos = 7;
const int echoPin = 12;  // echo for ultrasonic sensor
const int trigPin = 11;  // trigger for ultrasonic sensor
const int turbPin = A0;
const int simTurbPin = A3;

// LCD settings
int displaySetting = 0;
const int maxDisplays = 5;
unsigned long lastDisplaySwitch = millis();
const int displayDelay = 250;

// YOUR GLOBAL VARIABLES SHOULD BE DECLARED HERE

float total_volume = 0.0;
float vol_of_rectangle = 0.0;
float vol_of_triangle = 0.0;
float length = 254;
float width = 152;
float c = 19;
float Height = 146;

float min_height = 121.92; //mm
float turbVal = 0.0;
float simTurbVal = 0.0;
float vaverage = 0.0;
float vinitial = 0.0;
float vfinal = 0.0;
unsigned long startTime = 0.0;
unsigned long endTime = 0.0;
int timerflag = 0;
int valveState = 0;


void setup() {
  // initialize LCD & its backlight
  lcd.init();
  lcd.backlight();

  // initialize pushbutton for LCD toggle
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeDisplaySetting, FALLING);

  // initialize pins for ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // initialize pins used for flow control
  pinMode(turbPin, INPUT);
  pinMode(pumpPos, OUTPUT);
  pinMode(pumpNeg, OUTPUT);
  pinMode(valvePos, OUTPUT);
  pinMode(valveNeg, OUTPUT);
  pinMode(simTurbPin, INPUT);

  // ALWAYS keep these low!!!
  digitalWrite(pumpNeg, LOW);
  digitalWrite(valveNeg, LOW);

  // IF you want these to turn on, write them HIGH
  digitalWrite(pumpPos, LOW);
  digitalWrite(valvePos, LOW);
}

void loop() {
  // YOUR CODE GOES HERE!!!!
  displayLCD();
  delay(500);

  waterTurbidity();
  simulatedTurbidity();
  motorizedValveState();
  dischargeFlowRate();

  if (checkDist() >= min_height)
  {
    digitalWrite(pumpPos, HIGH);
  }
  if (checkDist() < min_height)
  {
    digitalWrite(pumpPos, LOW);
  }
}

//Call this function when you want to update the LCD Display
//The ONLY changes you should make to this function are the variables inside the lcd.print for each case!
void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (displaySetting % maxDisplays) {
    case 0:
      lcd.print("Water Turbidity:");
      lcd.setCursor(0, 1);
      lcd.print(turbVal);
      break;
    case 1:
      lcd.print("Recirculation Tank Volume");
      lcd.setCursor(0, 1);
      lcd.print(total_volume);
      break;
    case 2:
      lcd.print("Simulated Turbidity");
      lcd.setCursor(0, 1);
      lcd.print(simTurbVal);
      break;
    case 3:
      lcd.print("Motorized Valve State");
      lcd.setCursor(0, 1);
      lcd.print(valveState);
      break;
    case 4:
      lcd.print("Discharge Flow rate");
      lcd.setCursor(0, 1);
      lcd.print(vaverage);
      break;
    default:
      lcd.print("Unknown Setting!");
  }
}

void changeDisplaySetting() {
  if (lastDisplaySwitch + displayDelay < millis()) {  // this limits how quickly the LCD Display can switch
    lastDisplaySwitch = millis();
    displaySetting++;
  }
}

//Call this function to get a distance measurement in cm from the Ultrasonic sensor
float checkDist()  //TLI code
{
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  float d = duration * 0.034 / 2;  // Speed of sound wave divided by 2 (go and back)
  // Returns the distance in cm
  return d;

}

float waterTurbidity()
{
  turbVal = analogRead(turbPin);
  return turbVal;
}

float volumeofWater() {
  vol_of_triangle = length * c * width * 0.5;
  vol_of_rectangle = length * width * (Height - (10*checkDist()));
  total_volume = vol_of_rectangle - vol_of_triangle;
  return total_volume;
}

float simulatedTurbidity()
{
  simTurbVal = analogRead(simTurbPin);
  return simTurbVal;
}


int motorizedValveState()
{
  //  based on simulated turbidity from potentiometer 
  if(simulatedTurbidity() >= 550) //water is clean 
  {    
    digitalWrite(valvePos, HIGH); //discharge state
    valveState = 1;
    vinitial = volumeofWater();    
    startTime = millis();
    timerflag = 1;
  }
  if (simulatedTurbidity() < 550) //dirty water
  { //dirty water
    digitalWrite(valvePos, LOW); //recirculation mode
    valveState = 0;
    vfinal = volumeofWater();
    if (timerflag == 1 and millis() != startTime) //timer has already started
    {
      endTime = millis();      
    }
  }
  return valveState;
}
float dischargeFlowRate()
{
  //use flag
  vaverage = (vfinal - vinitial) / (endTime - startTime);
  return vaverage;

}
