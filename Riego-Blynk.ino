#define BLYNK_PRINT Serial   
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#define BLYNK_PRINT Serial
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = "twwgkR7jNi09H6EHM87AGO35dVm4Uu5m";   //Llave Blynk
char ssid[] = "RHUTEC";                        //Nombre WiFi 
char pass[] = "Cloclo**1998";                        //Contraseña WiFi


#define pirPin D1            
int pirValue;                   
int pinValue;                   

#define sensorPin D6 
#define rainPin D5
int sensorState = 0;
int rainState = 0;
int lastState = 0;
int lastRainState = 0;
#define DHTPIN 2    
#define DHTTYPE DHT11     
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

BLYNK_WRITE(V0)
{
 pinValue = param.asInt();    
} 

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("No se pudo leer los datos del Sensor DH11");
    return;
  }
 
  Blynk.virtualWrite(V5, h);  //V5 Humedad
  Blynk.virtualWrite(V6, t);  //V6 Temperatura
}
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
   pinMode(sensorPin, INPUT);
   pinMode(rainPin, INPUT);
   pinMode(pirPin, INPUT);
  dht.begin();

  timer.setInterval(1000L, sendSensor);
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
   sensors.begin();
}
int sensor=0;
void sendTemps()
{
sensor=analogRead(A0);
sensors.requestTemperatures();
float temp = sensors.getTempCByIndex(0); 
Serial.println(temp);
Serial.println(sensor);
Blynk.virtualWrite(V1, temp);
Blynk.virtualWrite(V2,sensor);
delay(1000);
}

void getPirValue(void)        //Obtener Datos del Sensor PIR
  {
   pirValue = digitalRead(pirPin);
    if (pirValue) 
     { 
       Serial.println("Movimiento Detectado");
       Blynk.notify("Movimiento Detectado");  
     }
  }
  
void loop()
{
  Blynk.run(); 
  timer.run(); 
  sendTemps();
sensorState = digitalRead(sensorPin);
Serial.println(sensorState);

if (sensorState == 1 && lastState == 0) {
  Serial.println("Enviar notificacion, necesita agua");
  Blynk.notify("Riega tus plantas");
  lastState = 1;
  delay(1000);
//Enviar notificación 
    
  } 
  else if (sensorState == 1 && lastState == 1) {
    //No hacer nada, no ha sido regado todavia
  Serial.println("No ha sido regado todavia");
  delay(1000);
  }
  else {
    Serial.println("No necesita agua");
    lastState = 0;
    delay(1000);
  }



rainState = digitalRead(rainPin);
Serial.println(rainState);

  if (rainState == 0 && lastRainState == 0) {
  Serial.println("Esta lloviendo!");
  Blynk.notify("Esta lloviendo!");
  lastRainState = 1;
  delay(1000);
//Enviar notificacion
    
  } 
  else if (rainState == 0 && lastRainState == 1) {
  delay(1000);
  }
  else {
    Serial.println("No esta lloviendo");
    lastRainState = 0;
    delay(1000);
  }


  if (pinValue == HIGH)    
      {
        getPirValue();
      }
      Blynk.run();
  delay(100);
}
