#include "TDS.h"

void setup() {
  Serial.begin(9600);
  setupTDS(27);
}

void loop() {
  // read a bunch of analog values and store them in the buffer every 40ms
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(27);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }
  // get the latest reading every 800ms and the avg voltage by using the median filter algorithm
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      voltage = avgVoltage(analogBufferTemp, SCOUNT, VREF);
    }
  }
  // calculate temperture coeff
  float coeff = TDSCoeff(temperature);
  // get final voltage value
  voltage = getFinalVoltage(voltage, coeff);
  // convert voltage to TDS
  tdsValue = getTDSValue(voltage);
  // print results
  Serial.print("TDS Value:");
  Serial.print(tdsValue, 0);
  Serial.println("ppm");
  delay(1000);
}