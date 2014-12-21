/*
Disclaimer: All insulting comments are directed towards myself (@Ben)
 */
// DEFINE SENSOR PINS and interpretation specs
int delaytime = 100; //specify delay time (ms)
const int steps[] = { 
  1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4 , 5 , 5, 6, 6, 7 ,8 , 9, 10, 12, 14}; // why I no use function=? (faster ? )
// light Sensor 1
#define lightSensorPin1 2 // must be! due to interrupts
int ls1StepSize = 1;
boolean ls1UseExp = true; //using exponential traversion
// light Sensor 2
#define lightSensorPin2 3 // must be! due to interrupts
int ls2StepSize = 1;
boolean ls2UseExp = true; //using exponential traversion
// ultrasonic distance 1 // CHECK PULSEIN FUNCTION CALL!! (might be deactivated due to timeout!)
#define echoPinSonic1 10 
#define trigPinSonic1 11
boolean us1UseExp = true; //using exponential traversion
int us1StepSize = 1;
//ultrasonic distance 2 //CHECK PULSEIN FUNCTION CALL!! (might be deactivated due to timeout!)
#define echoPinSonic2 12 
#define trigPinSonic2 13
boolean us2UseExp = true; //using exponential traversion
int us2StepSize = 1;
//Potty 
#define potPin 0 //analog
boolean potUseExp = false;
int potStepSize = 1;
//presure
#define pressurePin 4 //analog
boolean prsUseExp = true;
int prsStepSize = 1;
//Joystick
#define joystickXPin 1 // analog
#define joystickYPin 2 // analog
#define joystickButtonPin 3 // analog
boolean joXUseExp = false;
boolean joYUseExp = false;
boolean joBUseExp = false; 
boolean useJoyButton = false;
int joXStepSize = 1;
int joYStepSize = 1;
int joBStepSize = 1;


//MIDI specifications
byte noteON = 144;
byte noteOFF = 128;
byte controlChange = 176;
/*MIDI Channel Specification
 *
 * Controller 0 : Sonic Distance 1
 * Controller 1 : Sonic Distance 2
 * Controller 2 : Light Sensor 1
 * Controller 3 : Light Sensor 2
 * Controller 4 : Potty
 * Controller 5 : Pressure
 * Controller 6 : Joystick X
 * Controller 7 : Joystick Y
 * Controller 8 : Joystick Button
 */
int controllers = 9;
int oldChannelValues[20];
int channelValues[20];
boolean changedValueInd[20];

//exponential Function Indicator (also mapped to the channels)
boolean expFunInd[20];

//variables for the lightsensor 1 & 2
unsigned long cnt1 = 0;
unsigned long oldcnt1 = 0;
unsigned long cnt2 = 0;
unsigned long oldcnt2 = 0;

void setup() {
  for (int i = 0; i < 20; ++i) {
    oldChannelValues[i] = 0;
    channelValues[i] = 0;
    changedValueInd[i] = false;
    expFunInd[i] = false;
  }

  //Init Serial connection
  Serial.begin(9600);

  //Init expon bools
  expFunInd[0] = ls1UseExp;
  expFunInd[1] = ls2UseExp;
  expFunInd[2] = us1UseExp;
  expFunInd[3] = us2UseExp;
  expFunInd[4] = potUseExp;
  expFunInd[5] = prsUseExp;
  expFunInd[6] = joXUseExp;
  expFunInd[7] = joYUseExp;
  expFunInd[8] = joBUseExp;
  // Initialize  Sonic 1
  pinMode(trigPinSonic1, OUTPUT);
  pinMode(echoPinSonic1, INPUT);
  // Initialize Sonic 2
  pinMode(trigPinSonic2, OUTPUT);
  pinMode(echoPinSonic2, INPUT);
  //Initialize LightSensor 1
  pinMode(lightSensorPin1, INPUT);
  digitalWrite(lightSensorPin1, HIGH);
  attachInterrupt(0, incLightSensorCounter1, RISING);
  //Initialize LightSensor 2
  pinMode(lightSensorPin2, INPUT);
  digitalWrite(lightSensorPin2, HIGH);
  attachInterrupt(1, incLightSensorCounter2, RISING);
  //Initialize Potty
}

/*
 * Mesure Distance from sonic sensors
 */

void checkAndChange(int controller, int stepSize, int newVal){
  if((channelValues[controller]+us1StepSize) <= newVal){
    oldChannelValues[controller] = (channelValues[controller]/stepSize)*stepSize;
    changedValueInd[controller] = true;
    channelValues[controller] = (newVal/stepSize)*stepSize;
  }
  else if((channelValues[controller]-us1StepSize) > newVal){
    oldChannelValues[controller] = (channelValues[controller]/stepSize)*stepSize;
    changedValueInd[controller] = true;
    channelValues[controller] = (newVal/stepSize)*stepSize;
  }

}

/*
 *This needs configuration
 */

void sonicDistance1(){
  /*
  * Figure out why it stops and remove that
   */
  long duration; 
  digitalWrite(trigPinSonic1, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPinSonic1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSonic1, LOW);
  duration = pulseIn(echoPinSonic1, HIGH,10000); //IF not connected add third value specifying timeout! (Else remove it!! (Thing will not work otherwise..)
  int distance = ((int) ((duration/2) / 29.1));
  distance  = constrain(distance,0,50);
  distance = map(distance,0,50,0,127);
  checkAndChange(0, us1StepSize, distance);




}

int sonicDistance2(){
  long duration;
  digitalWrite(trigPinSonic2, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPinSonic2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSonic2, LOW);
  duration = pulseIn(echoPinSonic2, HIGH,10000);//IF not connected add third value specifying timeout ! (Else remove it!! (Thing will not work otherwise..)
  int distance = ((int) ((duration/2) / 29.1));
  distance  = constrain(distance,0,50);
  distance = map(distance,0,50,0,127);
  checkAndChange(1, us2StepSize, distance);

  
}


/*
 *This needs configuration
 */
void lightSensor1(){
  unsigned long temp = cnt1;
  unsigned long hz = temp - oldcnt1;
  oldcnt1 = temp;
  hz = (hz+50)/100;
  hz = constrain(hz, 0 , 20);
  hz = map(hz,0,20,0,127);
  checkAndChange(2, ls1StepSize, hz);

}

void lightSensor2(){
  unsigned long temp = cnt2;
  unsigned long hz = temp - oldcnt2;
  oldcnt2 = temp;
  hz = (hz+50)/100;
  //Serial.println(hz);
  hz = constrain(hz, 0 , 20);
  hz = map(hz,0,20,0,127);
  checkAndChange(3, ls2StepSize, hz);
}

void potty(){ 
  int temp = analogRead(potPin);
  int value = map(temp,0,1023,0,127); //nicely done <3
  checkAndChange(4, potStepSize, value);
}

/*
 *Pressure doesn't work for now
 */
void pressure(){
  int value = analogRead(pressurePin);
  value = value; // mapping.. told you..
  checkAndChange(5, prsStepSize, value);
}

void joystick(){
  int sensorValueX = analogRead(joystickXPin);
  int sensorValueY = analogRead(joystickYPin);
  sensorValueX = map(sensorValueX,0,1023,0,127);
  sensorValueY = map(sensorValueY,0,1023,0,127);
  checkAndChange(6, joXStepSize, sensorValueX);
  checkAndChange(7, joYStepSize, sensorValueY);
  int buttonVal = analogRead(joystickButtonPin);
  //TODO ADD MAP FUNCTION TO DISTANCE you lazy idiot

  if((buttonVal && !channelValues[8] )|| (!buttonVal && channelValues[8])){ 
    channelValues[8] = buttonVal;
    boolean off = !!buttonVal;
    sendButtonSignal(buttonVal, 9);
  }

}

void sendButtonSignal(boolean off , int c){
  int val;  
  if (off){
    val = 0;
  }
  else{
    val = 127;
  }  

  MIDImessage(controlChange, c , val);
}

void loop() {
  // gather sensor data 
  sonicDistance1();
  sonicDistance2();
  lightSensor1();
  lightSensor2();
  potty();
  joystick();
  //  pressure(); // not working!!
  sendMessages();
  delay(delaytime);

}

void sendMessages(){
  for(int i = 0 ; i < controllers ; ++i){
    if(changedValueInd[i]){
      changedValueInd[i] = false;
      int val = constrain(channelValues[i],0,127);
      MIDImessage(controlChange, (i+1) , val);
    }
  } 
}


//send MIDI message
void MIDImessage(byte command, byte data1, byte data2) {
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}




void incLightSensorCounter1()
{
  cnt1++;
}

void incLightSensorCounter2()
{
  cnt2++;
}




/*

void sendExpMidiFunction(int controller){
  int temp = oldChannelValues[controller];
  int newv = channelValues[controller];
  int i = 1; 
  if(temp <= newv){
    while(temp < newv || temp < 127){
      temp = temp + steps[i];
      if(temp > newv){
        temp = newv;
      }
      if(temp > 127 || temp < 0 ){
        temp= 127;
      }
      MIDImessage(controlChange, controller , temp);
      ++i;
      delay(10);
    }
  }
  else{
    while(temp > newv || temp >= 0){
      temp = temp - steps[i];

      if(temp < newv){
        temp = newv;
      }

      if(temp < 0 || temp > 127){
        temp = 0; 
      }
      MIDImessage(controlChange, controller , temp);
      ++i;
      delay(10);
    }

  }
}*/



