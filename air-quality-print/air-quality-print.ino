/* Librerie per utilizzo dei sensori */ 

//#include <GMXXX.h>    // Sensore MultiGas
#include <HM330X.h>   // Sensore Laser PM
//#include <DPS310.h>   // Barometro + Temperatura
#include <SCD30.h>    // CO2 + Temperatura + Umidità
#include <HP20x_dev.h> // High precision barometer

#include "Arduino.h"
#include "Wire.h"

//DPS310 dps310 = DPS310();
SCD30 scd30;
//GAS_GMXXX<TwoWire> gas;
HM330X hm330x;

void setup() {

  Wire.begin();
  Serial.begin(9600);        // start serial for output

  Serial.println("**** Starting sensors ****\n");

  delay(500);
  
  /* INIZIALIZZAZIONE DEI SENSORI */
  /* Avvio barometro */
  // dps310.begin();
  /* Avvio sensore Temperatura, Umidità, CO2 */
  scd30.begin();
  /* Avvio sensore MultiGas */
  // gas.begin(); 
  /* Avvio sensore Particolato */
  hm330x.begin();
  /* Avvio Barometro Alta Precisione*/
  HP20x.begin();

  delay(500);

}

void loop() {

  Serial.println("------------------\n");
 
  /****************************************
   * Temperature, Pressure
   ****************************************/
/*
  float temperature = 0.0;
  float pressure = 0.0;

  temperature = dps310.measure_Temperature();
  pressure = dps310.measure_Pressure();

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("pressure", pressure);
*/
  /****************************************
   * Humidity, CO2, (Temperature)
   ****************************************/

  float co2 = 0;
  float humidity = 0;
  float temp = 0;
 
  scd30.measure_CO2_and_Humidity(&co2, &humidity);

  //co2 = scd30.measure_CO2();
  //humidity = scd30.measure_Humidity();
  //temp = scd30.measure_Temperature();

    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.println("ppm.\n"); 

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%.\n"); 

   // Serial.print("Temperature: ");
   // Serial.print(temp);
   // Serial.println("C.\n"); 

  /****************************************
   * Multigas Sensor
   ****************************************/
/*   
  uint32_t no2 = 0;
  uint32_t c2h5oh = 0;
  uint32_t voc = 0;
  uint32_t co = 0;
 
  float no2i_ppm = 0;
  float c2h5oh_ppm = 0;
  float voc_ppm = 0;
  float co_ppm = 0;

  no2 = gas.measure_NO2(); 
  c2h5oh = gas.measure_C2H5OH(); 
  voc = gas.measure_VOC(); 
  co = gas.measure_CO();

  if (no2 > 999) no2 = 999;
  no2_ppm = gas.calcVol(no2);
 
  if (c2h5oh > 999) c2h5oh = 999;
  c2h5oh_ppm = gas.calcVol(c2h5oh);
  
  if (voc > 999) voc = 999;
  voc_ppm = gas.calcVol(voc);

  if (co > 999) co = 999;
  co_ppm = gas.calcVol(co);

*/
  /***************************************
   * Particulate Matter
   ***************************************/

  uint32_t pm1 = 0;
  uint32_t pm2_5 = 0;
  uint32_t pm10 = 0;
  
  pm1 = hm330x.measure_PM1();
  pm2_5 = hm330x.measure_PM2_5();
  pm10 = hm330x.measure_PM10();

  // pm1_atm = hm330x.measure_PM1_Atmospheric();
  // pm2_5_atm = hm330x.measure_PM2_5_Atmospheric();
  // pm10_atm = hm330x.measure_PM10_Atmospheric();

    Serial.print("PM1: ");
    Serial.print(pm1);
    Serial.println("ppm.\n"); 
    
    Serial.print("PM2.5: ");
    Serial.print(pm2_5);
    Serial.println("ppm.\n"); 

    Serial.print("PM10: ");
    Serial.print(pm10);
    Serial.println("ppm.\n"); 


  /***************************************
   * Barometro
   ***************************************/


    long Pressure = HP20x.ReadPressure();
    Serial.print("Pressure: ");
    Pressure = Pressure / 100.0;
    Serial.print(Pressure);
    Serial.println("hPa.\n"); 


  /***************************************
   * Attesa prima del prossimo ciclo
   ***************************************/

  delay(5000);

}
