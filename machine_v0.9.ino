//акселерометр
//#include "Wire.h"
//#define MPU 0x68

// задній моторчик
#define enA 10
#define in1 9
#define in2 8

// передній моторчик
#define enB 5
#define in3 7
#define in4 6

// лівий ультразвуковий датчик
#define trigL 12
#define echoL 11

// правий ультразвуковий датчик
#define trigR 4
#define echoR 3

// середній ультразвуковий датчик
#define trigM 2
#define echoM 1

void setup(){
  //Wire.begin();    
  //Піни моторчика на вихід
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Піни 'тріг' на вихід, 'ехо' на вхід
  pinMode(trigL, OUTPUT);
  pinMode(trigR, OUTPUT);
  pinMode(trigM, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(echoR, INPUT);
  pinMode(echoM, INPUT);
  //pinMode(A0, INPUT);
  //акселерометр
  Serial.begin(9600);
}

void loop(){
  driver();
}

void driver(){
  //int16_t ac_x = accelerometr();
  byte impL_dis = leftUS();
  byte impM_dis = middleUS();
  byte impR_dis = rightUS();
  
  //Serial.print("ac_x: "); Serial.print(ac_x); Serial.print("\t");
  Serial.print("a: "); Serial.print(analogRead(A0)); Serial.print("\t");
  Serial.print("disL: "); Serial.print(impL_dis); Serial.print("\t");
  Serial.print("disM: "); Serial.print(impM_dis); Serial.print("\t");
  Serial.print("disR: "); Serial.print(impR_dis); Serial.print("\n");
  
  if((impL_dis > 30 && impM_dis > 30 && impR_dis > 30) || (impL_dis <= 30 && impM_dis > 30 && impR_dis <= 30)) {
    forward(190);
    stabilization();
  }

  else if(impL_dis <= 30 && impM_dis <= 30 && impR_dis > 30){
    forward(100);
    right(255);
  }

  else if(impL_dis > 30 && impM_dis <= 30 && impR_dis <= 30){
    forward(100);
    left(255);
  }

  else if(impL_dis <= 30 && impM_dis <= 30 && impR_dis <= 30){
    back(200);
  }

  else if(impL_dis > 30 && impM_dis <= 10 && impR_dis > 30 && impR_dis > impL_dis){
    back(200);
    left(255);
  }

  else if(impL_dis > 30 && impM_dis <= 10 && impR_dis > 30 && impL_dis > impR_dis){
    back(200);
    right(255);
  }

  else if(impL_dis > 30 && impM_dis <= 30 && impM_dis > 10 && impR_dis > 30 && impR_dis > impL_dis){
    forward(100);
    right(255);
  }

  else if(impL_dis > 30 && impM_dis <= 30 && impM_dis > 10 && impR_dis > 30 && impL_dis > impR_dis){
    forward(100);
    left(255);
  }
}

//поворот вліво, шим задати вручну
void left(byte pwr){
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
  analogWrite(enB, pwr);
}

//поворот вправо, шим задати вручну
void right(byte pwr){
  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
  analogWrite(enB, pwr);
}

//вперед, шим задати вручну
void forward(byte pwr){
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
  analogWrite(enA, pwr);  
}

//назад, шим задати вручну
void back(byte pwr){
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  analogWrite(enA, pwr);
}

//стоп задній мотор
void stopA(){
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
}

//стоп передній мотор
void stopB(){
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}

void stabilization(){
  int pot_value = analogRead(A0);
  
  if(pot_value >= 390 && pot_value <= 410){
    stopB();
  }
  
  if(pot_value < 390){
    left(255);
  }
  
  if(pot_value > 410){
    right(255);
  }
}

byte leftUS(){
  digitalWrite(trigL, 0);
  delayMicroseconds(5);
  digitalWrite(trigL, 1);
  delayMicroseconds(15);
  digitalWrite(trigL, 0);
  int impL = pulseIn(echoL, 1);  
  return impL*0.034/2;
}

byte middleUS(){
  digitalWrite(trigM, 0);
  delayMicroseconds(5);
  digitalWrite(trigM, 1);
  delayMicroseconds(15);
  digitalWrite(trigM, 0);
  int impM = pulseIn(echoM, 1);
  return impM*0.034/2;
}

byte rightUS(){
  digitalWrite(trigR, 0);
  delayMicroseconds(5);
  digitalWrite(trigR, 1);
  delayMicroseconds(15);
  digitalWrite(trigR, 0);
  int impR = pulseIn(echoR, 1);
  return impR*0.034/2;
}
/*
int16_t accelerometr(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40] // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  int16_t accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  int16_t accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  int16_t accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  int16_t temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  int16_t gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  int16_t gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  int16_t gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  return accelerometer_x;
}*/
