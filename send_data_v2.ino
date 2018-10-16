#include <Wire.h>
#include <Servo.h>

int RightMotor = 9; //5    // pin maping according to schematics 
int LeftMotor = 10;  //7
int R_dir = 8; 
int L_dir = 12;
String c ;


int ch1,ch2,ch3,ch4,ch5,ch6;

void setup() {
  Serial.begin(9600);
  pinMode(7,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(2,INPUT);
  TCCR1B = TCCR1B & 0B11111000 | 0B00000100;  
  pinMode(RightMotor,OUTPUT); 
  pinMode(LeftMotor,OUTPUT);
  pinMode(R_dir,OUTPUT); 
  pinMode(L_dir,OUTPUT); 
}

String getValue(String data, char separator, int index)
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
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
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
void Left(unsigned int Pwm) 
{ 
   //digitalWrite(R_dir,LOW); 
   //digitalWrite(L_dir,HIGH); 
   //analogWrite(RightMotor,Pwm);  
   //analogWrite(LeftMotor,Pwm); 

   digitalWrite(R_dir,HIGH); 
   digitalWrite(L_dir,LOW); 
   analogWrite(RightMotor,Pwm);  
   analogWrite(LeftMotor,Pwm); 
} 
void Right(unsigned int Pwm) 
{ 
   //digitalWrite(R_dir,HIGH); 
   //digitalWrite(L_dir,LOW); 
   //analogWrite(RightMotor,Pwm);  
   //analogWrite(LeftMotor,Pwm); 

   digitalWrite(R_dir,LOW); 
   digitalWrite(L_dir,HIGH); 
   analogWrite(RightMotor,Pwm);  
   analogWrite(LeftMotor,Pwm); 
} 
void Stop() 
{ 
   digitalWrite(RightMotor,LOW);  
   digitalWrite(LeftMotor,LOW); 
} 

void loop() {
    if (Serial.available()) {
        String myString = Serial.readString();
        

        String xval = getValue(myString, ',', 0);
        String yval = getValue(myString, ',', 1);
        if(yval == "F"){
        Forward(xval.toInt());
        delay(1000);
        Stop();
        }
        else if(yval == "L") {
        Left(xval.toInt());
        delay(1000);
        Stop();
        }
        else if(yval == "R") {
        Right(xval.toInt());
        delay(1000);
        Stop();
        }
        else if(yval == "S") {
        Stop();
        }
        Serial.println(yval);
        

        
    }
}
