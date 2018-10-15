
void setup() {
    Serial.begin(9600);
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
void Forward(int Pwm){
  Serial.println(Pwm);
}
void Left(int Pwm){
  Serial.println(Pwm);
}
void Right(int Pwm){
  Serial.println(Pwm);
}
void Stop(){
  Serial.println("Stopped");
}

void loop() {
    if (Serial.available()) {
        String myString = Serial.readString();
        

        String xval = getValue(myString, ',', 0);
        String yval = getValue(myString, ',', 1);
        if(yval == "F"){
        Forward(xval.toInt());
        }
        else if(yval == "L") {
        Left(xval.toInt());
        }
        else if(yval == "R") {
        Right(xval.toInt());
        }
        else if(yval == "S") {
        Stop();
        }
        

        
    }
}
