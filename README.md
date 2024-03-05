This is a custom driver/library for sen0277 TDS sensor on ESP32. It's best to use it with ADC module and a waterproof temperature sensor for accurate readings. Source code from: https://wiki.dfrobot.com/Gravity__Analog_TDS_Sensor___Meter_For_Arduino_SKU__SEN0244

TDS.ino is a "main" program to test the functions in TDS.h

TDS.h contains the functions

variables:

int analogBuffer[SCOUNT]: array of SCOUNT size to store analog values read by the ADC

int analogBufferTemp[SCOUNT]: a copy of analogBuffer to apply the median filtering on

int analogBufferIndex = 0: to index analogBuffer

int copyIndex = 0: to index analogBufferTemp

float voltage = 0

float tdsValue = 0

float temperature = 16: current temperature for compensation. you can use a temperature sensor and adjust value as needed

functions:

void setupTDS(int pin): initializes sensor pin

float getMedian(int bArray[], int iFilterLen): median filtering algorithm. takes int array bArray off size iFilterLen and outputs median bTemp

float getTDSValue(float voltage): gets final TDS value from voltage 

float getFinalVoltage(float avg, float coeff): compensates for temperature variations. takes average voltage and coefficient as input and computes final voltage, which will be used to calculate TDS 

float TDSCoeff(float temperature): calculates TDS coefficient. formula computed at 25°C 

float avgVoltage(int buffer[], int count, float vref): calculates average voltage read from the sensor after applying median filtering and voltage conversion. takes the array of readings which will go through median filtering, median filter size and ADC voltage reference as inputs.
