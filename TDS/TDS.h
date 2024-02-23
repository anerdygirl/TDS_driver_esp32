#define VREF 3.3   // analog reference voltage(Volt) of the ADC
#define SCOUNT 30  // sum of sample point

// initialize variables needed for reading and conversion
int analogBuffer[SCOUNT];      // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];  // analogBuffer after applying median filtering
int analogBufferIndex = 0;
int copyIndex = 0;

float voltage = 0;
float tdsValue = 0;
float temperature = 16;  // current temperature for compensation. you can use a temperature sensor and adjust as needed

void setupTDS(int pin) {
  pinMode(pin, INPUT);
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
  if ((iFilterLen & 1) > 0) {  // median value if array length is odd
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {  // median value if array length is even
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
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