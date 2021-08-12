/*
Empieza a grabar cuando:
  Botonstart==1 
  Llega a la hora Inicial indicada (HoraInicio_i, con i=1,2,3)
Se detiene la grabación cuando:
  BotonBreak==1
  Llega a la hora Final indicada (HoraFinal_i, con i=1,2,3)
  
Hay un salto de fichero si pasa un determinado tiempo, dado por "SaltoFichero". Para modificar el tamaño (en segundos) del
fichero, se puede cambiar la variable "duracion".
La variable "siguiente" se utiliza para que cuando se llega al tiempo de SaltoFichero el programa para de
grabar y automaticamente comienza un nuevo fichero.
La variable "ControladorFinal" no se puede tocar ya que sino no funciona el salto de fichero.

El nombre del archivo está compuesto por el día más la hora, así por ejemplo, si se graba un archivo el 24 a las 16:07:35, 
entonces el archivo se llamará 24160735.wav

Cuando se toca el botón para grabar y no tiene la tarjeta SD salta error (titila 6 veces el led ROJO). 
WARNING: EVITAR SACAR LA TARJETA MICRO SD PARA QUE NO SE ROMPA Y PREVENIR MAL FUNCIONAMIENTO DE LA MISMA.
*/

#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#include "RTClib.h"     // para el RTC

#define SD_ChipSelectPin 10  //example uses hardware SS pin 53 on Mega2560

TMRpcm audio;   // create an object for use in this sketch 
#define MIC A0

//--------------------------------------------------------------------------------------------
// Variables que se pueden modificar

unsigned long freq_muestreo = 22000; //Frecuencia de muestreo en Hertz
uint8_t Duracion = 5; //Duracion de los ficheros, esta en minutos

uint8_t HoraInicio_1 = 8;
uint8_t MinInicio_1 = 0;
uint8_t SecInicio_1 = 0;

uint8_t HoraFinal_1 = 10;
uint8_t MinFinal_1 = 0;
uint8_t SecFinal_1 = 0;

uint8_t HoraInicio_2 = 14;
uint8_t MinInicio_2 = 0;
uint8_t SecInicio_2 = 0;

uint8_t HoraFinal_2 = 18;
uint8_t MinFinal_2 = 0;
uint8_t SecFinal_2 = 0;

//uint8_t HoraInicio_3 = 6;
//uint8_t MinInicio_3 = 0;
//uint8_t SecInicio_3 = 0;

//uint8_t HoraFinal_3 = 7;
//uint8_t MinFinal_3 = 0;
//uint8_t SecFinal_3 = 0;


//--------------------------------------------------------------------------------------------
//Defino variables

uint8_t LED_Work  = 2; //cuando está grabando está encendido
uint8_t BotonStart = 3; //Boton de Comienzo: Inicio de Grabación
  
uint8_t LED_Error = 4; //si parpadea hubo error, si está encendido es que se mandó a Detener
uint8_t BotonBreak = 5; //Boton de Interrupción: Cortar Fichero

uint8_t RST_PIN = 6;

unsigned long t; //Variable del millis()

volatile boolean Siguiente = false; //para ir al próximo fichero
volatile boolean ControladorFinal=false;

char filename[] = "_00000000_000000.wav"; 
unsigned long SaltoFichero = Duracion * 60000; //60 seg * 1000 (mili) = 60000 milisegundos

uint8_t HoraInicio;
uint8_t MinInicio;
uint8_t SecInicio;

uint8_t HoraFinal;
uint8_t MinFinal;
uint8_t SecFinal;

//==============================================================================
  RTC_DS3231 rtc;
  
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  *date = FAT_DATE(now.year(), now.month(),  now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
//==============================================================================
//------------------------------------------------------------------------------
void getFileName(){
    DateTime now = rtc.now();
    sprintf(filename, "%02d%02d%02d%02d.wav", now.day(), now.hour(), now.minute(), now.second());
}
//--------------------------------------------------------------------------------
//==============================================================================
void LedError() {
  digitalWrite(LED_Work, LOW);  //apago LED de trabajo
  for (int j = 1; j < 6; j++) { // parpadea el de error
    digitalWrite(LED_Error, HIGH);
    delay(1000);
    digitalWrite(LED_Error, LOW);
    delay(1000);      
  }
  pinMode(RST_PIN,  OUTPUT);
  digitalWrite(RST_PIN, LOW);
}
//==============================================================================
void LedFinal(){
  digitalWrite(LED_Work,   LOW);
  delay(500);
  digitalWrite(LED_Work,   HIGH);
  delay(500);
  digitalWrite(LED_Work,   LOW);
}
//==============================================================================
void setup() {  
    rtc.begin();
    Serial.begin(9600);
  if (rtc.lostPower()) {
    // following line sets the RTC to the date & time this sketch was compiled (setea el dia y hora de la compu)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2021, 7, 25, 17, 58, 0));// Esta linea es para ajustar de forma manual el día y hora
  } 
  audio.CSPin = SD_ChipSelectPin; // The audio library needs to know which CS pin to use for recording
  pinMode(MIC, INPUT);  // Microphone
  pinMode(LED_Error, OUTPUT);
  pinMode(LED_Work, OUTPUT); 
  pinMode(BotonStart, INPUT_PULLUP);
  pinMode(BotonBreak, INPUT_PULLUP);
}
void loop() {
  DateTime now = rtc.now();
  //digitalWrite(LED_Work, HIGH);
  //digitalWrite(LED_Error, HIGH);

  if (((MinInicio_1-now.minute())==1)||((MinInicio_1-now.minute())==(-59))&&((HoraInicio_1-now.hour())==1)){
     HoraInicio=HoraInicio_1;
     MinInicio=MinInicio_1;
     SecInicio=SecInicio_1;
      
     HoraFinal=HoraFinal_1;
     MinFinal=MinFinal_1;
     SecFinal=SecFinal_1;
   }
   if(((now.hour()>=(HoraInicio_1)))&&((now.hour()<=(HoraFinal_1)))) {
    HoraInicio=HoraInicio_1;
     MinInicio=MinInicio_1;
     SecInicio=SecInicio_1;
      
     HoraFinal=HoraFinal_1;
     MinFinal=MinFinal_1;
     SecFinal=SecFinal_1;
   }
   if(((MinInicio_2-now.minute())==1)||((MinInicio_2-now.minute())==(-59))&&((HoraInicio_2-now.hour())==1)){
     HoraInicio=HoraInicio_2;
     MinInicio=MinInicio_2;
     SecInicio=SecInicio_2;
      
     HoraFinal=HoraFinal_2;
     MinFinal=MinFinal_2;
     SecFinal=SecFinal_2;
   }
  if(((now.hour()>=(HoraInicio_2)))&&((now.hour()<=(HoraFinal_2)))) {
    HoraInicio=HoraInicio_2;
     MinInicio=MinInicio_2;
     SecInicio=SecInicio_2;
      
     HoraFinal=HoraFinal_2;
     MinFinal=MinFinal_2;
     SecFinal=SecFinal_2;
   }
//   else if(((MinInicio_3-now.minute())==1)||((MinInicio_3-now.minute())==(-59))&&((HoraInicio_3-now.hour())==1)){
//     HoraInicio=HoraInicio_3;
//     MinInicio=MinInicio_3;
//     SecInicio=SecInicio_3;
//     
//     HoraFinal=HoraFinal_3;
//     MinFinal=MinFinal_3;
//     SecFinal=SecFinal_3;
//   }
    Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second());      
  if (!digitalRead(BotonStart)||(Siguiente==1)||(now.hour()==HoraInicio && now.minute()==MinInicio && now.second()==SecInicio)) { 
    if (!SD.begin(SD_ChipSelectPin)) LedError();
    Serial.print("Antes de getFile: ");Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second());    
    getFileName();    
    SdFile::dateTimeCallback(dateTime); //Para poner los atributos en el archivo guardado
    audio.startRecording(filename,freq_muestreo,MIC);
    Serial.print("Despues de startRecord: ");Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second());    
    t=millis();
    Siguiente=false;
    ControladorFinal=true;
    digitalWrite(LED_Work, HIGH);
  }
  if ((millis() - t > SaltoFichero)&&(ControladorFinal==1)){        
      digitalWrite(LED_Work, LOW);
      Siguiente=true;
      audio.stopRecording(filename);
  }   
  else if (!digitalRead(BotonBreak)||(now.hour()==HoraFinal && now.minute()==MinFinal && now.second()==SecFinal)){           
       audio.stopRecording(filename);
       Serial.print("Despues de stopRecord: ");Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second());    
       LedFinal();
       Siguiente=false;
       ControladorFinal=false;
       return; 
  }
} //cierra void loop
