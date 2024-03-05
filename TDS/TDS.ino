#include "TDS.h"

#ifndef ARRAY_MANIPULATION_H
#define ARRAY_MANIPULATION_H
void readvalues(int (&analogBuffer)[SCOUNT]);  // Function declaration with reference parameter
#endif

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);
  setupTDS(27);
}

void loop() {
  
  // get the latest reading every 800ms and the avg voltage by using the median filter algorithm
  readvalues(analogBuffer);
  
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      voltage = avgVoltage(analogBufferTemp, SCOUNT, VREF);
    }
  }

  // calculate temperture coeff. for reading variation due to temperature changes
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