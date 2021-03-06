#include <Arduino.h>
#include <Ultrasonic.h>
#include <ESP32Encoder.h>
#include <PID_v1.h>
#include <World.h>

using namespace world;

byte serial[7];

union AtoF{
  float f;
  byte a4[4];
}floatbyte;

union AtoI{
  uint32_t u;
  byte a4[4];
}intbyte;

uint32_t array_to_Int(byte byte0, byte byte1, byte byte2, byte byte3){

  intbyte.a4[3] = byte0;
  intbyte.a4[2] = byte1;
  intbyte.a4[1] = byte2;
  intbyte.a4[0] = byte3;

  return intbyte.u;
}

float array_to_Float(byte byte0, byte byte1, byte byte2, byte byte3){

  floatbyte.a4[3] = byte0;
  floatbyte.a4[2] = byte1;
  floatbyte.a4[1] = byte2;
  floatbyte.a4[0] = byte3;

  return floatbyte.f;
}

void serialReader(){
  while(Serial.available() <= 6);

  while(Serial.available() >= 7){

    while(Serial.peek() != 30) Serial.read();
    Serial.readBytes(serial, 7);
    if(serial[6] != 50) break;

    switch (serial[1]){

      case 100:
      Serial.println("OK");
      break;

      case 101:
      ESP.restart();
      break;

      case 102:
      Serial.print("WALK PID = ");
      Serial.print(walk_Kp, 4);
      Serial.print(", ");
      Serial.print(walk_Ki, 4);
      Serial.print(", ");
      Serial.println(walk_Kd, 4);
      break;

      case 103:
      Serial.print("TURN PID = ");
      Serial.print(turn_Kp, 4);
      Serial.print(", ");
      Serial.print(turn_Ki, 4);
      Serial.print(", ");
      Serial.println(turn_Kd, 4);
      break;

      case 104:
      Serial.print(actionCollision?"NEXT":"STOP");
      Serial.print(", ");
      Serial.println(collision);
      break;

      case 105:
      Serial.print("POSITION = ");
      Serial.print(leftEncoderPosition);
      Serial.print(", ");
      Serial.println(rightEncoderPosition);
      break;

      case 106:
      updateDistance(2);
      Serial.print("DISTANCE = ");
      Serial.print(frontDistance);
      Serial.print(", ");
      Serial.println(backDistance);
      break;

      case 110:
      walk_Kd = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(walk_Kp, walk_Ki, walk_Kd);
      Serial.print("WALK_KD = ");
      Serial.println(walk_Kd, 4);
      break;

      case 111:
      walk_Ki = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(walk_Kp, walk_Ki, walk_Kd);
      Serial.print("WALK_KI = ");
      Serial.println(walk_Ki, 4);
      break;

      case 112:
      walk_Kp = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(walk_Kp, walk_Ki, walk_Kd);
      Serial.print("WALK_KP = ");
      Serial.println(walk_Kp, 4);
      break;

      case 113:
      turn_Kd = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(turn_Kp, turn_Ki, turn_Kd);
      Serial.print("TURN_KD = ");
      Serial.println(turn_Kd, 4);
      break;

      case 114:
      turn_Ki = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(turn_Kp, turn_Ki, turn_Kd);
      Serial.print("TURN_KI = ");
      Serial.println(turn_Ki, 4);
      break;

      case 115:
      turn_Kp = array_to_Float(serial[2], serial[3], serial[4], serial[5]);
      MotorPID.SetTunings(turn_Kp, turn_Ki, turn_Kd);
      Serial.print("TURN_KP = ");
      Serial.println(turn_Kp, 4);
      break;

      case 116:
      actionCollision = (serial[4] == 1);
      if(serial[5]>30) serial[5] = 30;
      else if(serial[5]<0) serial[5] = 0;
      collision = serial[5];
      Serial.print("COLLISION = ");
      Serial.print(serial[4]==1?"NEXT":"STOP");
      Serial.print(", ");
      Serial.println(serial[5]);

      break;

      case 117:
      uint32_t steps = array_to_Int(0, serial[3], serial[4], serial[5]);
      int action = ((serial[2]>>1)&1);
      bool direction = ((serial[2]&1) == 1);
      if(((serial[2]>>2)&1) == 0) Serial.print(execute(action, steps, direction));
      else Serial.print(execute(0, 10^12, direction));
      Serial.println();
      Output = 0;
      leftSpeed = Output;
      rightSpeed = Output;
      moveMotor();

      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Tudo Pronto");
  MotorPID.SetMode(AUTOMATIC);
  MotorPID.SetSampleTime(10);
  rightEncoderMotor.attachHalfQuad(PIN_RIGHTENCODER1, PIN_RIGHTENCODER2);
	leftEncoderMotor.attachHalfQuad(PIN_LEFTENCODER1, PIN_LEFTENCODER2);
  ledcSetup(RIGHTCHANNEL1, 5000, 8);
  ledcAttachPin(PIN_RIGHTMOTOR1, RIGHTCHANNEL1);
  ledcSetup(RIGHTCHANNEL2, 5000, 8);
  ledcAttachPin(PIN_RIGHTMOTOR2, RIGHTCHANNEL2);
  ledcSetup(LEFTCHANNEL1, 5000, 8);
  ledcAttachPin(PIN_LEFTMOTOR1, LEFTCHANNEL1);
  ledcSetup(LEFTCHANNEL2, 5000, 8);
  ledcAttachPin(PIN_LEFTMOTOR2, LEFTCHANNEL2);
}

void loop() {
  serialReader();
}
