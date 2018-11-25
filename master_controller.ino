// Master
#include <Wire.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <math.h>

#define cal_addr  0x10
#define awg_in_apin A0
#define fan_dpin     2
#define pwm_dpin     3
#define relay_dpin   4
#define piezo_dpin   5
#define r_temp_dpin  8
#define b_water_dpin 9
#define w_norm_dpin  10

HX711_ADC LoadCell(7, 6); // dout, sck

int temp, temp_alarm, water, water_alarm, rpm, incomingByte;
struct calib {
  float cal_factor;
};
float weight = 0, cal_factor = 0;
long stabilisingTime = 2000, t = 0;
char buff[64], weight_str[32], cal_str[32];
byte response[5];

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  EEPROM.get(cal_addr, cal_factor);

  LoadCell.begin();
  LoadCell.start(stabilisingTime);
  LoadCell.setCalFactor(cal_factor);

  Wire.begin();
  pinMode(r_temp_dpin, OUTPUT); // Red
  pinMode(w_norm_dpin, OUTPUT); // White
  pinMode(b_water_dpin, OUTPUT); // Blue
  pinMode(relay_dpin, OUTPUT); // Relay
  pinMode(fan_dpin, OUTPUT); // Cooling Fan
}

void loop() {
  LoadCell.update();
  if (millis() > t + 1000) {
    Wire.requestFrom(18, 5);
    int index = 0;

    // Wait for response
    while (Wire.available()) {
      byte b = Wire.read();
      response[index] = b;
      index++;
    }

    snprintf(buff, 64, "% d %d %d %s %s ", response[2], response[3], response[4], weight_str, cal_str);
    Serial.println(buff);

    weight = LoadCell.getData();
    cal_factor = LoadCell.getCalFactor();

    dtostrf(weight, 10, 2, weight_str);
    dtostrf(cal_factor, 10, 2, cal_str);
    t = millis();
  }

  float val = weight * (255 / 100.0);
  int value = (int) round(val);
  analogWrite(pwm_dpin, value);
  digitalWrite(fan_dpin, (value >= 123) ? HIGH : LOW); //615

  if (Serial.available() > 0) {
    float tmp = 0;
    char inByte = Serial.read();
    if (inByte == 'H') tmp = 10.0;
    else if (inByte == 'L') tmp = -10.0;
    else if (inByte == 'T') LoadCell.tareNoDelay();

    if (tmp != 't') {
      calib
      var;

      float new_cal = LoadCell.getCalFactor() + tmp;
      LoadCell.setCalFactor(new_cal);

      var.cal_factor = new_cal;
      EEPROM.put(cal_addr,
        var);
    }
  }

  if ((response[2] == 0) && (response[3] == 0)) {
    digitalWrite(relay_dpin, HIGH);
    digitalWrite(w_norm_dpin, HIGH);
    digitalWrite(r_temp_dpin, LOW);
    digitalWrite(b_water_dpin, LOW);
  } else {
    tone(piezo_dpin, 1000, 500);
    if (response[2] == 1) {
      digitalWrite(r_temp_dpin, HIGH);
    }
    if (response[3] == 1) {
      digitalWrite(b_water_dpin, HIGH);
    }
    digitalWrite(relay_dpin, LOW);
    digitalWrite(w_norm_dpin, LOW);
  }
  incomingByte = Serial.read();
  if (incomingByte = 999) {
    digitalWrite(relay_dpin, LOW);
  }
}
