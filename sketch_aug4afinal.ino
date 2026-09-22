// =====================================================
// ESP32 Ultrasonic Fuel Level Monitoring System
// ESP32 + JSN-SR04T + MCP4725 + DSE 7320 MKII
//
//  Libraries + Configuration + Sensor Reading
// =====================================================


#include <Wire.h>
#include <Adafruit_MCP4725.h>


// =====================================================
// MCP4725 DAC
// =====================================================

Adafruit_MCP4725 dac;

#define MCP4725_ADDRESS 0x60



// =====================================================
// Ultrasonic Sensor Pins
// =====================================================

#define TRIG_PIN 18
#define ECHO_PIN 19



// =====================================================
// I2C Pins
// =====================================================

#define SDA_PIN 21
#define SCL_PIN 22



// =====================================================
// Tank Calibration
// =====================================================

const float FULL_VOLUME = 4016.0;

const float FULL_DISTANCE = 23.0;

const float EMPTY_DISTANCE = 132.58;


const float TANK_HEIGHT =
EMPTY_DISTANCE - FULL_DISTANCE;



// =====================================================
// DAC Configuration
// =====================================================

#define DAC_MAX_VALUE 4095


const float DAC_MIN_VOLTAGE = 0.0;

const float DAC_MAX_VOLTAGE = 3.30;



// =====================================================
// Filtering
// =====================================================

#define SAMPLE_COUNT 20


float filteredDistance = 0;



// =====================================================
// Sensor Fault Detection
// =====================================================

#define SENSOR_FAIL_LIMIT 5


int failCount = 0;

bool sensorFault = false;



// =====================================================
// Fuel Variables
// =====================================================

float distance = 0;

float fuelHeight = 0;

float fuelPercentage = 0;

float fuelLiters = 0;



// =====================================================
// DAC Variables
// =====================================================

int dacValue = 0;

float outputVoltage = 0;




// =====================================================
// Ultrasonic Reading Function
// =====================================================

float readUltrasonicDistance()
{

  long duration;


  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(5);


  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(20);


  digitalWrite(TRIG_PIN, LOW);



  duration = pulseIn(
              ECHO_PIN,
              HIGH,
              60000);



  if(duration == 0)
  {
    return -1;
  }



  float distanceCM =
  duration * 0.0343 / 2.0;


  return distanceCM;

}

// =====================================================
// Filtering + Fuel Calculation + Sensor Fault Detection
// =====================================================



// =====================================================
// Distance Filtering Function
// =====================================================

float getFilteredDistance()
{

  float total = 0;

  int validSamples = 0;



  for(int i = 0; i < SAMPLE_COUNT; i++)
  {

    float d = readUltrasonicDistance();



    // Accept valid ultrasonic readings

    if(d > 0 && d < 200)
    {

      total += d;

      validSamples++;

    }


    delay(30);

  }



  // =================================================
  // Sensor Working Normally
  // =================================================

  if(validSamples > 0)
  {

    failCount = 0;

    sensorFault = false;



    filteredDistance =
    total / validSamples;


    return filteredDistance;

  }



  // =================================================
  // Sensor Failure
  // =================================================

  else
  {

    failCount++;



    if(failCount >= SENSOR_FAIL_LIMIT)
    {

      sensorFault = true;

    }



    return -1;

  }

}






// =====================================================
// Fuel Level Calculation
// =====================================================

void calculateFuelLevel()
{

  // Read ultrasonic distance

  distance = getFilteredDistance();



  // =================================================
  // Sensor Fault Condition
  // =================================================

  if(sensorFault)
  {

    fuelPercentage = 0;

    fuelHeight = 0;

    fuelLiters = 0;


    return;

  }



  // =================================================
  // Sensor gives temporary no echo
  // =================================================

  if(distance < 0)
  {

    // Keep previous fuel value
    // until fault limit reached

    return;

  }



  // =================================================
  // Full Tank Condition
  // =================================================

  if(distance <= FULL_DISTANCE)
  {

    distance = FULL_DISTANCE;


    fuelPercentage = 100.0;


    fuelHeight = TANK_HEIGHT;


    fuelLiters = FULL_VOLUME;


    return;

  }



  // =================================================
  // Empty Tank Condition
  // =================================================

  if(distance >= EMPTY_DISTANCE)
  {

    distance = EMPTY_DISTANCE;


    fuelPercentage = 0.0;


    fuelHeight = 0;


    fuelLiters = 0;


    return;

  }



  // =================================================
  // Normal Measurement Range
  // =================================================


  fuelHeight =
  EMPTY_DISTANCE - distance;



  fuelPercentage =
  (fuelHeight / TANK_HEIGHT) * 100.0;



  // Limit percentage

  if(fuelPercentage > 100)
  {

    fuelPercentage = 100;

  }



  if(fuelPercentage < 0)
  {

    fuelPercentage = 0;

  }



  // Calculate fuel volume

  fuelLiters =
  (fuelPercentage / 100.0)
  * FULL_VOLUME;


}

// =====================================================
// DAC Output + Setup + Main Loop
// =====================================================



// =====================================================
// DAC Output Calculation
//
// DSE 7320 MKII Mapping
//
// Full tank 100%  = 0.0 V
// Empty tank 0%   = 3.30 V
//
// Sensor fault   = 3.30 V
// =====================================================


void updateDAC()

{

  // =================================================
  // Sensor Fault
  // =================================================

  if(sensorFault)
  {

    dacValue = DAC_MAX_VALUE;


    dac.setVoltage(
      dacValue,
      false
    );


    outputVoltage =
    DAC_MIN_VOLTAGE +
    ((dacValue / 4095.0) *
    (DAC_MAX_VOLTAGE - DAC_MIN_VOLTAGE));


    return;

  }



  // =================================================
  // Normal Fuel Level Mapping
  // =================================================

  dacValue =
  ((100.0 - fuelPercentage)
  /100.0)
  * DAC_MAX_VALUE;



  // Limit DAC value

  if(dacValue < 0)
  {

    dacValue = 0;

  }


  if(dacValue > DAC_MAX_VALUE)
  {

    dacValue = DAC_MAX_VALUE;

  }



  // Send DAC output

  dac.setVoltage(
    dacValue,
    false
  );



  // Calculate voltage for display

  outputVoltage =
  DAC_MIN_VOLTAGE +
  ((dacValue / 4095.0) *
  (DAC_MAX_VOLTAGE - DAC_MIN_VOLTAGE));


}






// =====================================================
// Setup Function
// =====================================================


void setup()

{

  Serial.begin(115200);



  Serial.println();

  Serial.println("==============================");

  Serial.println("ESP32 Fuel Monitoring System");

  Serial.println("JSN-SR04T + MCP4725");

  Serial.println("==============================");




  // Ultrasonic pins

  pinMode(TRIG_PIN, OUTPUT);

  pinMode(ECHO_PIN, INPUT);



  digitalWrite(TRIG_PIN, LOW);




  // I2C Start

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );




  // MCP4725 Start

  if(!dac.begin(MCP4725_ADDRESS))
  {

    Serial.println("MCP4725 NOT FOUND");

  }

  else
  {

    Serial.println("MCP4725 Connected");

  }




  // Initial output

  dac.setVoltage(
    0,
    false
  );



  Serial.println("System Ready");

  Serial.println();

}







// =====================================================
// Main Loop
// =====================================================


void loop()

{


  // Calculate fuel

  calculateFuelLevel();



  // Update DAC

  updateDAC();




  // =================================================
  // Serial Monitor
  // =================================================


  Serial.println("------------------------------");



  if(sensorFault)
  {

    Serial.println("!!! ULTRASONIC SENSOR FAULT !!!");

  }



  Serial.print("Distance : ");

  Serial.print(distance,2);

  Serial.println(" cm");




  Serial.print("Fuel Height : ");

  Serial.print(fuelHeight,2);

  Serial.println(" cm");




  Serial.print("Fuel Percentage : ");

  Serial.print(fuelPercentage,2);

  Serial.println(" %");




  Serial.print("Fuel Volume : ");

  Serial.print(fuelLiters,2);

  Serial.println(" L");




  Serial.print("DAC Value : ");

  Serial.println(dacValue);




  Serial.print("Output Voltage : ");

  Serial.print(outputVoltage,3);

  Serial.println(" V");



  Serial.println("------------------------------");



  delay(1000);


}

