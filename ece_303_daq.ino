// DAQ
#include <Wire.h>
#include "dht.h"

#define dht_apin   A0  // Analog Pin for Temperature and Humidity Sensor
#define water_apin A1  // Analog Pin for Water Sensor
#define interrupt   2  // Interrupt Pin

dht DHT;

int temp, temp_alarm, water, water_alarm;
int count = 0, timer_count = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(18);
  Wire.onRequest(requestEvent);
  pinMode(interrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interrupt), pulse_count, RISING);
  TCCR2B = TCCR2B & 0b11111000 | 0x04; // Clock 2 for PWM
  TIMSK2 = (1 << TOIE1); // Enable overflow interrupt
}

ISR(TIMER2_OVF_vect) {
  timer_count++;
  if (timer_count == (980 / 2)) {
    if (Serial.available() <= 0) {
      Serial.println(count * 2);
    }
    timer_count = 0;
    count = 0;
  }
}

void loop() {
  DHT.read11(dht_apin);
  temp = DHT.temperature;
  water = analogRead(water_apin);
  delay(1500);
}

void requestEvent() {
  temp_alarm = temp > 30 ? 1 : 0
  water_alarm = water > 150 ? 1 : 0

  byte response[] = {
    temp,
    water,
    temp_alarm,
    water_alarm,
    count * 2
  };
  Wire.write(response, 5);
}

void pulse_count() {
  count++;
}
