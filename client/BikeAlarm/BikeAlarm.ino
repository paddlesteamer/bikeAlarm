
#include <Wire.h>
#include <L3G.h>
#include <SoftwareSerial.h>

#define RD_INTERVAL 500

#define RELAY        6
#define DORJI_ENABLE 5
#define DORJI_TX     4
#define DORJI_RX     3

SoftwareSerial  dorji(DORJI_RX, DORJI_TX);
L3G gyro;

bool alarm = false;

int lastX = 0;
int lastEn = 0;

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(DORJI_ENABLE, OUTPUT);


  digitalWrite(RELAY, HIGH);
  digitalWrite(DORJI_ENABLE, LOW);
  
  Serial.begin(9600);
  dorji.begin(9600);
  Wire.begin();

  while(!Serial);

  Serial.println("Starting...");

  delay(100);

  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }

  gyro.enableDefault();
}


void alarmOn() {
  alarm = true;
  Serial.println("Alarm!");
  digitalWrite(RELAY, LOW);
  digitalWrite(DORJI_ENABLE, HIGH);
  delay(50);
  dorji.write("ALARM!\n");
  delay(50);
  digitalWrite(DORJI_ENABLE, LOW);
}

void alarmOff() {
  digitalWrite(RELAY, HIGH);

  alarm = false;
}



void loop() {
  gyro.read();

  int curX = (int) gyro.g.x;
  int curMil = millis();

  if (alarm) {
    
    
    if ((curMil - lastEn) > 5000)
      alarmOff();
    else {
      lastX = curX;
      delay(RD_INTERVAL);
      return;
    }
  }

  if ((curX + lastX) < -2000) {   
    lastEn = curMil;
    alarmOn(); 
  } 
  
  lastX = curX;
  

  delay(RD_INTERVAL);
}
