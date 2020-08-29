/*
* Code submitted for the Table Top Lift project
* Course: Mechatronic Engineering
* Unit: Microprocessors and Microcontrollers
*/

#include <Keypad.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd (A0,A1,A2,A3,A4,A5);

const byte ROWS = 4; 
const byte COLS = 3; 

/*
* Define the key map
*/
char keys[ROWS][COLS] = { {'1','2','3'}, {'4','5','6'}, {'7','8','9'}, {'*','0','#'} };


/*
*Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
*/
byte rowPins[ROWS] = {12,11,10,9};

/*
*Connect keypad COL0, COL1 and COL2 to these Arduino pins.
*/
byte colPins[COLS] = {8,7,6};
Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

/*
*button pins connected to interrupts pin of the mega
*/
const byte groundup=18;
const byte oneup=20;
const byte onedown=19;
const byte twodown=3;

int motorpin1=48;
int motorpin2=49;
int enablePin1=4;

int state[4]={0,0,0,0};

int trigPin=52;
int echoPin=53;

int currentfloor=0;
int calledfloor;
int cm;
int height=20;
int sentinel;
int steps;

int destinationfloor;

void setup() {
 
pinMode(twodown,INPUT);
pinMode(oneup,INPUT);
pinMode(onedown,INPUT);
pinMode(groundup,INPUT);

Serial.begin(9600);

lcd.begin (16,2);
lcd.setCursor(0,0);

digitalWrite(enablePin1,0);

interrupts();
attachInterrupt(digitalPinToInterrupt(groundup),InterruptOne ,RISING);
attachInterrupt(digitalPinToInterrupt(onedown), InterruptTwo ,RISING);
attachInterrupt(digitalPinToInterrupt(oneup),InterruptThree ,RISING);
attachInterrupt(digitalPinToInterrupt(twodown),InterruptFour   ,RISING);

}

void loop() {
request_floor();
}




/*
* check the state of each button to determine the floor
*/
void request_floor() {
  if (state[3]==1) {
    Serial.println("groundfloor up");
    lcd.print("ground floor");

    calledfloor=0;
  
    state[3]=0;
    go_to_floor(); 
  }
  else if (state[1]==1) {
    Serial.println("first floor up");
    lcd.print("first floor");
    calledfloor=1;

    state[1]=0;
    go_to_floor();
  }
  else if (state[0]==1) {
    Serial.println("first floor down");
    lcd.print("first floor");
    calledfloor=1;


    state[0]=0;
    go_to_floor();
  }
  else if (state[2]==1){
    Serial.println("second floor down");
    lcd.print("second floor");
    calledfloor=2;

    state[2]=0;
    go_to_floor();
  }
  else{
    Serial.println("waiting");  
  }
}



/*
* Go to the request floor
*/
void go_to_floor() {
  steps=calledfloor-currentfloor;
  if(steps>0){
    distance();
    clockwise(calledfloor);
    keypadselect();
  }
  else if(steps<0){
    distance();
    anticlockwise(calledfloor);
    keypadselect();
  }
  else{
    stall();
    keypadselect();
  }

  currentfloor=calledfloor;
}

/*
* move motor clockwise
*/
void anticlockwise(int floor_val) {
  switch (floor_val) {
    case 0:
      distance();
      while(cm<2*height) {//subject to change
        distance();
        analogWrite(enablePin1,130);
        digitalWrite(motorpin1,HIGH);
        digitalWrite(motorpin2,LOW);
      }
      analogWrite(enablePin1,0);
    break;
    
    case 1:
      distance();
      while(cm<height*floor_val) {//height*floor_val subject to change
        distance();
        analogWrite(enablePin1,130);
        digitalWrite(motorpin2,HIGH);
        digitalWrite(motorpin1,LOW);
      }
      analogWrite(enablePin1,0);
    break;
  }
} 

/*
* move motor anticlockwise
*/
void clockwise(int floor_val) {
  switch (floor_val) {
    case 1:
      distance();
      while(cm>height*floor_val) {//height*floor_val subject to change
        distance();
        analogWrite(enablePin1,130);
        digitalWrite(motorpin2,HIGH);
        digitalWrite(motorpin1,LOW);
      }
      analogWrite(enablePin1,0);
    break;
    
    case 2:
      distance();
      while(cm>3) { // subject to change
        distance();
        analogWrite(enablePin1,130);
        digitalWrite(motorpin2,HIGH);
        digitalWrite(motorpin1,LOW);
      }
      analogWrite(enablePin1,0);
    break;
  }
}
    
/*
* stop the  motor
*/
void  stall(){
 digitalWrite( motorpin1,HIGH);
 digitalWrite( motorpin2,HIGH);
}
 
/*
* calculate the distance using the ultrasonic sensor
*/
int distance() {
  digitalWrite(trigPin,HIGH);
  delay(100);
  digitalWrite(trigPin,LOW);
  int duration = pulseIn (echoPin, HIGH);
  cm= duration/29 /2;
  Serial.println(cm);
  delay(100);
  return cm;
}
  
/*
* check the floor selected using the keypad 
* 0 for ground, 1 for first fllor, 2 for second floor
*/
int keypadselect() {
  sentinel=1; 
  while(sentinel>0) {
    int key = keypad.getKey();
    if(key) {
      if(key<=3) {
        destinationfloor= key-currentfloor;
        break;
      }
      else {
        lcd.print("Invalid key");
        Serial.println("Invalid key");
      }
    }
    if(destinationfloor>0) {
      distance();
      clockwise(key);
    }
    else {
      distance();
      anticlockwise(key);
    }
    break;
  }
  sentinel=-1;
}

/*
* Interrupt handlers
*/
void InterruptOne() {
  state[3]=digitalRead(groundup);
}

void InterruptTwo() {
  state[0]=digitalRead(onedown);
}

void InterruptThree() {
  state[1]=digitalRead(oneup);
}
  
void InterruptFour()  {
  state[2]=digitalRead(twodown);
}




