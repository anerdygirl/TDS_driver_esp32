#define VREF 3.3   // analog reference voltage(Volt) of the ADC. using the ESP32's built-in ADC
#define SCOUNT 30  // sum of sample point

#ifndef TDS_H
#define TDS_H
void readvalues(int (&analogBuffer)[SCOUNT]);  // Function declaration with reference parameter
float avgVoltage(int buffer[], int count, float vref);
float getMedian(int bArray[], int iFilterLen);
#endif

// initialize variables needed for reading and conversion
int analogBuffer[SCOUNT];      // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];  // analogBuffer after applying median filtering
int analogBufferIndex = 0;
float voltage = 0;
float tdsValue = 0;
float temperature = 16;  // current temperature for compensation (arbitrary value). you can use a temperature sensor and adjust as needed


void setupTDS(int pin) {
  pinMode(pin, INPUT);
}

void calibrate() {
  /* read reference body
      if ref isn't available -> print "reference NA. enter an existing reference"
    periodically:
      read tds
      compare reading with ref_tds
        if reading unstable -> keep "calibrating"
        else (if reading stable and matching): print "calibration complete" */
  String str, ref[3] = { "MILK", "WATER", "COOLANT" };
  bool isinref = false;
  // request input until it matches the reference object is available
  while (!isinref) {
    Serial.print("enter an existing reference: ");
    if (Serial.available() > 0) {
      str = Serial.readString();
      str.toUpperCase();
      // check if input exists in the array
      for (int i = 0; i < 3; i++) {
        if (str == ref[i])
          isinref = true;
      }

      if (isinref) {
        // carry on with the rest of the calibration:
        // read TDS
        int TDS;
        // compare value read with ref
        if (str == ref[0]) {
          //calibrate for ref == milk
        } else if (str == ref[1]) {
          //calibrate for ref == water
        } else if (str == ref[2]) {
          //calibrate for ref == engine coolant
        }
        return;
      } else
        Serial.println("ref NA. enter an existing reference...");
    }
  }
}

static unsigned long analogSampleTimepoint = millis(); // start time for reading analog values
void readanalogvalues(int (&analogBuffer)[SCOUNT]) {
  // read a bunch of analog values and store them in the buffer every 40ms
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(27);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }
  // get the latest readings and average voltage every 800ms
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (int copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      voltage = avgVoltage(analogBufferTemp, SCOUNT, VREF);
    }
  }
}

// converts voltage to TDS value
float getTDSValue(float voltage) {
  //gotta figure out the correct formula.. or simply paste it as is for now
  float tdsValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * 0.5;
  return tdsValue;
}

// takes temperature into account to compensate for the temperature variations
float getFinalVoltage(float avg, float coeff) {
  float voltage = avg / coeff;
  return voltage;
}

// calculates temperature coefficient
float TDSCoeff(float temperature) {
  float coeff = 1.0 + 0.02 * (temperature - 25.0);
  return coeff;
}

// calculates average voltage read from the sensor after applying median filtering and voltage conversion
float avgVoltage(int buffer[], int count, float vref) {
  float avg = getMedian(buffer, count) * vref / 1024.0;
  return avg;
}

// filters an array of readings to get a stable analog value
float getMedian(int bArray[], int iFilterLen) {
  // copy bArray into bTab
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  // sort bTab in ascending order
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  // apply median filtering according to the parity of array length
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}