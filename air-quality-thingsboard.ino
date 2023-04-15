/* Librerie per connettività Wi-Fi */
#include <WiFiNINA.h>
#include <ThingsBoard.h> // Necessita di ArduinoHttpClient & PubSubClient & ArduinoJson

/* Librerie per utilizzo dei sensori */ 
#include <Wire.h>
#include <GMXXX.h>    // Sensore MultiGas
#include <HM330X.h>   // Sensore Laser PM
#include <DPS310.h>   // Barometro + Temperatura
#include <SCD30.h>    // CO2 + Temperatura + Umidità

#define THINGSBOARD_ACCESS_TOKEN "..."

DPS310 dps310 = DPS310();
SCD30 scd30;
GAS_GMXXX<TwoWire> gas;
HM330X hm330x;

// Parametri rete Wi-Fi e server MQTT
WiFiClient client;
ThingsBoard tb(client);

char ssid[] = "...";    // Nome rete Wi-Fi
char pass[] = "...";          // Password rete Wi-Fi
int status = WL_IDLE_STATUS;

void setup() {

  /* Uso il led sulla scheda per mostrare lo stato della connessione */
  /* Led accesso fisso: Connessione OK, Led spento: Errore di connesione */
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* CONNESSIONE ALLA RETE WI-FI */
  WiFi.setTimeout(120 * 1000); // Timeout tentativo di connessione (2 minuti)
  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, pass);
    /* Se non riesco a connettermi, attendo 10 secondi e riprovo */
    if (status != WL_CONNECTED) delay(10000);
  }
  
  /* INIZIALIZZAZIONE DEI SENSORI */
  /* Avvio barometro */
  dps310.begin();
  /* Avvio sensore Temperatura, Umidità, CO2 */
  scd30.begin();
  /* Avvio sensore MultiGas */
  gas.begin(); 
  /* Avvio sensore Particolato */
  hm330x.begin();

}

void loop() {

  /****************************************
   * CONNESSIONE ALLA RETE WI-FI
   ****************************************/
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(LED_BUILTIN, LOW);   // Connessione fallita: spengo il led
    status = WiFi.begin(ssid, pass);
    /* Se non riesco a connettermi, attendo 10 secondi e riprovo */
    if (status != WL_CONNECTED) delay(10000);
  }
  
  /****************************************
   * CONNESSIONE AL BROKER MQTT
   ****************************************/
  if(!tb.connected()){
    digitalWrite(LED_BUILTIN, LOW);   // Connessione fallita: spengo il led
    while(!tb.connected()){
      //tb.connect("thingsboard.cloud", THINGSBOARD_ACCESS_TOKEN);
      //tb.connect("65.108.244.100", THINGSBOARD_ACCESS_TOKEN, 8080);
      if(!tb.connect("nova-aps.it", THINGSBOARD_ACCESS_TOKEN)) delay(5000);
    }
    digitalWrite(LED_BUILTIN, HIGH);  // Connessione ok: accendo il led

  }
  
  /****************************************
   * Temperature, Pressure
   ****************************************/

  float temperature = 0.0;
  float pressure = 0.0;

  temperature = dps310.measure_Temperature();
  pressure = dps310.measure_Pressure();

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("pressure", pressure);

  /****************************************
   * Humidity, CO2, (Temperature)
   ****************************************/

  float co2 = 0;
  float humidity = 0;
 
  scd30.measure_CO2_and_Humidity(&co2, &humidity);

  tb.sendTelemetryFloat("co2", co2);
  tb.sendTelemetryFloat("humidity", humidity);

  /****************************************
   * Multigas Sensor
   ****************************************/
   
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

  tb.sendTelemetryInt("no2", no2_ppm);
  tb.sendTelemetryInt("c2h5oh", c2h5oh_ppm);
  tb.sendTelemetryInt("voc", voc_ppm);
  tb.sendTelemetryInt("co", co_ppm);

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

  tb.sendTelemetryInt("pm1", pm1);
  tb.sendTelemetryInt("pm2_5", pm2_5);
  tb.sendTelemetryInt("pm10", pm10);
  

  /***************************************
   * Attesa prima del prossimo ciclo
   ***************************************/

  tb.loop();

  delay(5000);

}
