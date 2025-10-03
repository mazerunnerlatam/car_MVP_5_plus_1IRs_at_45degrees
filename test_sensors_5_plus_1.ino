#include <Arduino.h>

// Arduino Mega
#define IR1_right_far A0 // This is A0
#define IR2_right A1 // This is A1
#define IR3_center A2
#define IR4_left A3
#define IR5_left_far A4
#define IR6_front A5

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);

  pinMode(IR1_right_far, INPUT);
  pinMode(IR2_right, INPUT);
  pinMode(IR3_center, INPUT);
  pinMode(IR4_left, INPUT);
  pinMode(IR5_left_far, INPUT);

  Serial.println("About to start...");
  delay (1000);
}

void loop() {


  int srm  = digitalRead(IR1_right_far); // Symbol !  means NOT
  int sr = digitalRead(IR2_right); // Please Notice this is a DigitalREAD
  int scenter = digitalRead(IR3_center);
  int sl = digitalRead(IR4_left);
  int slm = digitalRead(IR5_left_far); // Sensor Right More More in config 6 IRs
  int front = !digitalRead(IR6_front);


  Serial.print("\t");
  Serial.print(slm);
  Serial.print("\t");
  Serial.print(sl);
  Serial.print("\t");
  Serial.print(scenter);
  Serial.print("\t");
  Serial.print(sr);
  Serial.print("\t");
  Serial.print(srm);
  Serial.print("\t");
  Serial.print(front);
  Serial.println("\t => Notice this DigitalREAD: 1 or 0");
  

  delay(500); 
}
