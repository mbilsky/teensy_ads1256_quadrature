#include <SPI.h>

#include <QuadDecode_def.h>
//more details available here: https://forum.pjrc.com/threads/26803-Hardware-Quadrature-Code-for-Teensy-3-x

//(other stuff for getting the ADS1526 to work is in the next tab
#define ADS_RST_PIN    8 //ADS1256 reset pin
#define ADS_RDY_PIN    22 //ADS1256 data ready
#define ADS_CS_PIN    21 //ADS1256 chip select

/*
    CLK  - pin 13
    DIN  - pin 11 (MOSI)
    DOUT - pin 12 (MISO)
*/


//put the ADC constants here

double resolution = 8388608.; //2^23-1

//this needs to match the setting in the ADC init function in the library tab
double Gain = 64.; //be sure to have a period

double vRef = 5.0; //reference voltage

//we'll calculate this in setup
double bitToVolt = 0.;

uint32_t val1 = 0;
uint32_t val2 = 0;
uint32_t val3 = 0;


//Quadrature encoder stuff
QuadDecode<1> outputQuad; //template using ftm 1
QuadDecode<2> motorQuad; //template using ftm 1
//pins 32 and 25 (A and B respectively)
uint32_t motorPos = 0;
uint32_t outputPos = 0;

//measurement variables
double value = 0;
double numAvg = 10.; // number of readings to average
long timeNow = 0;

//first run flag
int firstRun = 1;

double voltageOffset = 0;

void setup() {
  //stuff to initilize the ADC
  delay(1000);
  Serial.begin(115200);
  Serial.println("booting");
  //initialize the ADS
  pinMode(ADS_CS_PIN, OUTPUT);

  pinMode(ADS_RDY_PIN, INPUT);
  pinMode(ADS_RST_PIN, OUTPUT);

  SPI.begin();

  initADS();
  Serial.println("done init");

  //determine the conversion factor
  //do some calculations for the constants
  bitToVolt = resolution * Gain / vRef;


  //Now setup the quadrature
  motorQuad.setup();
  motorQuad.start();


  outputQuad.setup();
  outputQuad.start();
}



void loop() {
  // put your main code here, to run repeatedly:

  if (firstRun) {
    for (int i = 0; i < 500; i++) {
      value += read_Value();
    }

    value /= 500.;
    value /= bitToVolt;
    voltageOffset = value;
    firstRun = 0;
  }

  value = 0;

  for (int i = 0; i < numAvg; i++) {
    value += read_Value();
  }

  value /= numAvg;
  value /= bitToVolt;
  value -= voltageOffset;

  //get the position
  motorPos = motorQuad.calcPosn();
  outputPos = outputQuad.calcPosn();
  timeNow = micros();

  Serial.print(timeNow);
  Serial.print("\t");
  Serial.print(motorPos);
  Serial.print("\t");
  Serial.print(outputPos);
  Serial.print("\t");
  Serial.println(value, 11);

}
