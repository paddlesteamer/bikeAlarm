
#include <Wire.h>
#include <L3G.h>
#include <SoftwareSerial.h>
#include <LowPower.h>

#define DEBUG

#define RD_INTERVAL SLEEP_250MS

#define RELAY        6
#define DORJI_ENABLE 5
#define DORJI_TX     4
#define DORJI_RX     3

#define DORJI_PRE  "@"
#define DORJI_POST "!"

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
  
  dorji.begin(9600);
  Wire.begin();
  
#ifdef DEBUG
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Starting...");
#endif

  delay(100);

  if (!gyro.init())
  {
#ifdef DEBUG
    Serial.println("Failed to autodetect gyro type!");
#endif
    while (1);
  }

  gyro.enableDefault();
}

void broadcastAlarm() {
  digitalWrite(DORJI_ENABLE, HIGH);
  delay(50);
  dorji.write(DORJI_PRE);
  dorji.write("ALARM");
  dorji.write(DORJI_POST);
  delay(50);
  digitalWrite(DORJI_ENABLE, LOW);
}

void alarmOn() {
  alarm = true;
  
#ifdef DEBUG
  Serial.println("Alarm!");
#endif

  digitalWrite(RELAY, LOW);

  broadcastAlarm();  
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
      return;
    }
  }

  if ((curX + lastX) < -2000) {   
    lastEn = curMil;
    alarmOn(); 
  } 
  
  lastX = curX;

  LowPower.idle(RD_INTERVAL, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);
}
