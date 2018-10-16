#include <PID_v1.h>
#include <Wire.h>
#include <Servo.h>

int RightMotor = 9; //5    // pin maping according to schematics 
int LeftMotor = 10;  //7
int R_dir = 8; 
int L_dir = 12;
String c;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
double left_current, left_required, right_current, right_required, right_pwm, left_pwm;
PID myPID_left(&left_current, &left_pwm, &left_required, Kp, Ki, Kd, DIRECT);
PID myPID_right(&right_current, &right_pwm, &right_required, Kp, Ki, Kd, DIRECT);

void Forward(unsigned int right_pwm,unsigned int left_pwm) 
{ 
   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,right_pwm);  
   analogWrite(LeftMotor,left_pwm); 
} 

int ch1,ch2,ch3,ch4,ch5,ch6;

//String getValue(String data, char separator, int index)
//{
//    int found = 0;
//    int strIndex[] = { 0, -1 };
//    int maxIndex = data.length() - 1;
//
//    for (int i = 0; i <= maxIndex && found <= index; i++) {
//        if (data.charAt(i) == separator || i == maxIndex) {
//            found++;
//            strIndex[0] = strIndex[1] + 1;
//            strIndex[1] = (i == maxIndex) ? i+1 : i;
//        }
//    }
//    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
//}

int getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]).toInt() : 0;
}

   


void setup() {

  pinMode(7,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(2,INPUT);

  // turn on serial monitor
  Serial.begin(9600); // use the same baud-rate as the python side
  Serial.setTimeout(100);

  // reducing the pwm freq 
  TCCR1B = TCCR1B & 0B11111000 | 0B00000100;  
  //assigning Motor pin as output
  pinMode(RightMotor,OUTPUT); 
  pinMode(LeftMotor,OUTPUT);
  pinMode(R_dir,OUTPUT); 
  pinMode(L_dir,OUTPUT); 


  //initialize the variables we're linked to
  left_current = 0; //the current velocity of left wheel
  right_current = 0;//current right wheel velocity
  left_required = 0;
  right_required = 0;
  left_pwm = 0; //desired left wheel velocity from mpc
  right_pwm = 0;//desired right wheel velocity from mpc
    
  //turn the PID on
  myPID_left.SetMode(AUTOMATIC);
  myPID_right.SetMode(AUTOMATIC);

  myPID_left.SetOutputLimits(0,30);
  myPID_right.SetOutputLimits(0,30);
  
}
void loop() {

  ch1 = pulseIn(7,HIGH,30000);  //Done
  ch2 = pulseIn(6,HIGH,30000); //Done
  ch3 = pulseIn(5,HIGH,30000); //Done //Green
//  ch1 = pulseIn(5,HIGH,30000);  //Done
//  ch2 = pulseIn(6,HIGH,30000); //Done
//  ch4 = pulseIn(7,HIGH,30000); //Done //Green

  printRC();
  if(ch3>1600){
    manual();
  }
  else{
//    autonomous();
    autonomous_A();
  }

}


int pwd_up_limit   = 40;
int pwd_down_limit = 18;

int ch1_th_up   = 1550;
int ch1_th_down = 1450;
int ch1_up_limit   = 1770;
int ch1_down_limit = 1150;
double ch1_up_scale    = double(pwd_up_limit -pwd_down_limit)/(ch1_up_limit - ch1_th_up);
double ch1_down_scale  = double(pwd_up_limit -pwd_down_limit)/(ch1_down_limit - ch1_th_down);
double ch1_scale    = 0.0015;

int ch2_th_up   = 1550;
int ch2_th_down = 1450;
int ch2_up_limit   = 1770;
int ch2_down_limit = 1150;
double ch2_up_scale    = double(pwd_up_limit -pwd_down_limit)/(ch2_up_limit - ch2_th_up);
double ch2_down_scale  = double(pwd_up_limit -pwd_down_limit)/(ch2_down_limit - ch2_th_down);
double pwm, differential;
unsigned int motor_dir;
void manual(){
    pwm = 0;
    if(ch2>ch2_th_up){
      pwm = (ch2_up_scale*(ch2-ch2_th_up)) + pwd_down_limit;
      motor_dir = LOW;
    }
    else if(ch2<ch2_th_down){
      pwm = (ch2_down_scale*(ch2-ch2_th_down)) + pwd_down_limit;
      motor_dir = HIGH;
    }

    left_pwm = pwm;
    right_pwm = pwm;
    
    differential = 0;
   //if(1==0) { // when only left/right is not required without fwd/bck drive
    if(pwm==0){  //for one wheel at zero speed
      if(ch1>ch1_th_up){
        differential = (ch1_up_scale*(ch1-ch1_th_up)) + pwd_down_limit;
        right_pwm += differential;
      }
      else if(ch1<ch1_th_down){
        differential = (ch1_down_scale*(ch1-ch1_th_down)) + pwd_down_limit;
        left_pwm += differential;
      }
    }
    else{
      if(ch1>ch1_th_up){
        differential = (ch1_scale*(ch1-ch1_th_up));
        right_pwm *= (1+differential);
        left_pwm *= (1-differential);
      }
      else if(ch1<ch1_th_down){
        differential = (ch1_scale*(ch1_th_down-ch1)) ;
        left_pwm *= (1+differential);
        right_pwm *= (1-differential);
      }
    }

    Run_motor(right_pwm,left_pwm, motor_dir, motor_dir);
    printPWM();
    
}


void autonomous_A(){
  if(Serial.available()>0){
      String data = Serial.readStringUntil('x');
      //Serial.flush();
      //Serial.println(data);
      //Serial.write("recieved data");
      //Serial.flush();
      left_current = getValue(data, ',', 0);
      left_required = getValue(data, ',', 1);
      right_current = getValue(data, ',', 2);
      right_required = getValue(data, ',', 3);
      
      if(left_required==-1){
        
        left_current = 0;
        left_required = 0;
        right_current = 0;
        right_required = 0;
        Forward_A(0,0);
        
      }
      else{
  
      //Forward(left_required, right_required);
      myPID_left.Compute();
      myPID_right.Compute();
  
      String o_p =  String(left_pwm) + ","+ String(right_pwm);
      char dataStr[100] = {0};
      //sprintf(dataStr, "%02X", o_p);
      //Serial.println(dataStr);
      //Serial.println(right_pwm);
      //Serial.println(left_pwm);
      Forward_A(right_pwm,left_pwm);
      //Serial.print("Time: ");
      //time = millis();
  
      //Serial.println(time);
      Serial.println(right_pwm);
      //delay(1000);
      }
   }
}

void Run_motor(unsigned int right_pwm,unsigned int left_pwm,unsigned int R_motor_dir,unsigned int L_motor_dir ) 
{ 
   digitalWrite(R_dir,R_motor_dir); 
   digitalWrite(L_dir,L_motor_dir); 
   analogWrite(RightMotor,right_pwm);  
   analogWrite(LeftMotor,left_pwm); 
} 


void Forward_A(unsigned int right_pwm,unsigned int left_pwm) 
{ 
   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,right_pwm);  
   analogWrite(LeftMotor,left_pwm); 
} 


//void autonomous(){
//  if(Serial.available()>0){
//    while(Serial.available()){
//    c += Serial.readString();  
//   }
//
//  String sp = getValue(c, ';', 0);
//  String d = getValue(c, ';', 1);
//  
//  int s = sp.toInt();
//  char dir = d[0];
//  switch(dir){
//    case 'F':
//    Forward(s);
//    break;
//    case 'R':
//    Right(s);
//    break;
//    case 'L':
//    Left(s);
//    break;
//    case 'B':
//    Backward(s);
//    break;
//    case 'S':
//    Stop();
//    break;
//    
//    
//  }
//  Serial.println("Done");
//
// }
//
// c = "";
//}



void Forward(unsigned int Pwm) 
{ 
   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,Pwm);  
   analogWrite(LeftMotor,Pwm); 
} 
void Backward(unsigned int Pwm) 
{ 
   digitalWrite(R_dir,HIGH); 
   digitalWrite(L_dir,HIGH); 
   analogWrite(RightMotor,Pwm);  
   analogWrite(LeftMotor,Pwm); 
} 
void Left(unsigned int Pwm, unsigned int steer) 
{ 
   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,Pwm);  
   analogWrite(LeftMotor,Pwm-10); 
} 
void Right(unsigned int Pwm, unsigned int steer) 
{ 
   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,Pwm-steer);  
   analogWrite(LeftMotor,Pwm); 
} 
void Stop() 
{ 
   digitalWrite(RightMotor,LOW);  
   digitalWrite(LeftMotor,LOW); 
} 

void printRC(){
  Serial.print("C1: ");
  Serial.println(ch1);
  Serial.print("C2: ");
  Serial.println(ch2);
  Serial.print("C3: ");
  Serial.println(ch3);
  Serial.print("C4: ");
  Serial.println(ch4);
  Serial.print("C5: ");
  Serial.println(ch5);
  Serial.print("C6: ");
  Serial.println(ch6);
  //delay(1000);
  
}


void printPWM(){
  Serial.print("Right PWM: ");
  Serial.println(right_pwm);
  Serial.print("Left PWM: ");
  Serial.println(left_pwm);
}
