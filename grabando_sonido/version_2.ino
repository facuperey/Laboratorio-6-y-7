/*                                              CÓDIGO DE GRABACIÓN -- VERSIÓN 2
*Empieza a grabar cuando:
  Botonstart==1 
  Llega a la hora Inicial indicada (HoraInicio_i, con i=1,2,3)
*Se detiene la grabación cuando:
  BotonBreak==1
  Llega a la hora Final indicada (HoraFinal_i, con i=1,2,3)

*Hay un salto de fichero si pasa un determinado tiempo, dado por "SaltoFichero". Para modificar el tamaño (en segundos) del
 fichero, se puede cambiar la variable "duracion".
 
*La variable "siguiente" se utiliza para que cuando se llega al tiempo de SaltoFichero el programa para de
 grabar y automaticamente comienza un nuevo fichero.
 
*La variable "ControladorFinal" no se puede tocar ya que no para la grabación ni tampoco funciona el salto de fichero.

*Si se empieza a grabar antes de la hora de inicio, la grabación para 2 segundos antes del inicio automático
 ya que sino se produce error.
 
*Hay 3 intervalos de horas de grabación. Para añadir más intervalos de grabación se tienen que agregar las horas 
 de inicio y finalizacíon pertinentes, además del "void confHora_i" con "i" siendo el número de intervalo de grabación
 (por ejemplo, para HoraInicio_5 corresponde void confHora_5).
 
*El nombre del archivo está compuesto por la hora, y al final dos dígitos 0 (ejemplo: 15:52:05 --> 15520500.wav) en el caso
 de que el fichero no exista en la memoria SD. Si dicho archivo existe, se le coloca al final del nombre el dígito 1, 2 y 
 así segun corresponda (ejemplo 15520500 -->15520501).

*Entre que para de grabar y comienza un nuevo fichero hay unos segundos que se pierden (entre 2 a 5), esto se debe a que 
 por como está escrito el código, la grabación se demora al iniciar (esa línea de código es necesaria para que 
 se puedan crear, en la memoria SD, 99 ficheros en vez de 30).

*Cuando se toca el botón para grabar y no tiene la tarjeta SD salta error (titila 6 veces el led ROJO) y luego se reinicia 
 el Arduino.
 
        ***********
        **WARNING**: EVITAR SACAR LA TARJETA MICRO SD PARA QUE NO SE ROMPA Y PREVENIR MAL FUNCIONAMIENTO DE LA MISMA.
        ***********

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
uint8_t Duracion = 10; //Duracion de los ficheros, esta en minutos

uint8_t HoraInicio_1 = 7;
uint8_t MinInicio_1 = 0;
uint8_t SecInicio_1 = 0;

uint8_t HoraFinal_1 = 9;
uint8_t MinFinal_1 = 0;
uint8_t SecFinal_1 = 0;

uint8_t HoraInicio_2 = 11;
uint8_t MinInicio_2 = 0;
uint8_t SecInicio_2 = 0;

uint8_t HoraFinal_2 = 13;
uint8_t MinFinal_2 = 0;
uint8_t SecFinal_2 = 0;

uint8_t HoraInicio_3 = 16;
uint8_t MinInicio_3 = 0;
uint8_t SecInicio_3 = 0;

uint8_t HoraFinal_3 = 18;
uint8_t MinFinal_3 = 0;
uint8_t SecFinal_3 = 0;

//--------------------------------------------------------------------------------------------
//Defino variables o constantes

uint8_t LED_Work  = 2; //cuando está grabando está encendido
uint8_t BotonStart = 3; //Boton de Comienzo: Inicio de Grabación
  
uint8_t LED_Error = 4; //si parpadea hubo error, si está encendido es que se mandó a Detener
uint8_t BotonBreak = 5; //Boton de Interrupción: Cortar Fichero

uint8_t RST_PIN = 6;

unsigned long t; //Variable del millis()
unsigned long SaltoFichero = Duracion * 60000; //60 seg * 1000 (mili) = 60000 milisegundos

volatile boolean Siguiente = false; //para ir al próximo fichero
volatile boolean ControladorFinal=false;

char filename[] = "_00000000_000000.wav"; 
int i=0;

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
//-------------------------------------------------------------------------------
void confHora_1(){
  DateTime now = rtc.now();
  if (((MinInicio_1-now.minute())==1)||((MinInicio_1-now.minute())==(-59))&&(((HoraInicio_1-now.hour())==1)||((HoraInicio_1-now.hour())==0))){
    
     HoraInicio=HoraInicio_1;
     MinInicio=MinInicio_1;
     SecInicio=SecInicio_1;
      
     HoraFinal=HoraFinal_1;
     MinFinal=MinFinal_1;
     SecFinal=SecFinal_1;
   }
   if((now.hour()>=(HoraInicio_1))&&((now.hour()<=(HoraFinal_1)))) {
     
   HoraInicio=HoraInicio_1;
     MinInicio=MinInicio_1;
     SecInicio=SecInicio_1;
      
     HoraFinal=HoraFinal_1;
     MinFinal=MinFinal_1;
     SecFinal=SecFinal_1;
   }
}
//-------------------------------------------------------------------------------
void confHora_2(){
DateTime now = rtc.now();
if(((MinInicio_2-now.minute())==1)||((MinInicio_2-now.minute())==(-59))&&(((HoraInicio_2-now.hour())==1)||((HoraInicio_2-now.hour())==0))){
     
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
}
//==============================================================================
//-------------------------------------------------------------------------------
void confHora_3(){
DateTime now = rtc.now();
if(((MinInicio_3-now.minute())==1)||((MinInicio_3-now.minute())==(-59))&&(((HoraInicio_3-now.hour())==1)||((HoraInicio_3-now.hour())==0))){
     
   HoraInicio=HoraInicio_3;
   MinInicio=MinInicio_3;
   SecInicio=SecInicio_3;
      
   HoraFinal=HoraFinal_3;
   MinFinal=MinFinal_3;
   SecFinal=SecFinal_3;
   }
  if(((now.hour()>=(HoraInicio_3)))&&((now.hour()<=(HoraFinal_3)))) {
    
   HoraInicio=HoraInicio_3;
   MinInicio=MinInicio_3;
   SecInicio=SecInicio_3;
      
   HoraFinal=HoraFinal_3;
   MinFinal=MinFinal_3;
   SecFinal=SecFinal_3;
   }
}
//==============================================================================
//Luego de haber empezado manualmete una grabacion y se acerque el inicio automatico, 
//este para de grabar 2 segundos antes para evitar producir un error. 
void manual_auto(){
  DateTime now = rtc.now();
  if (ControladorFinal==1 && ((HoraInicio-now.hour())==1||(HoraInicio-now.hour())==0) && ((MinInicio-now.minute())==1||(MinInicio-now.minute())==(-59)) &&((SecInicio-now.second())==2||(SecInicio-now.second())==(-58))){
    LedFinal();
    audio.stopRecording(filename);
    ControladorFinal=false;     
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void getFileName(){
    DateTime now = rtc.now();
    sprintf(filename, "%02d%02d%02d%02d.wav", now.hour(), now.minute(), now.second(),i);

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
  if (rtc.lostPower()) {
    // following line sets the RTC to the date & time this sketch was compiled (setea el dia y hora de la compu)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2021, 7, 25, 2, 58, 0));// Esta linea es para ajustar de forma manual el día y hora
  } 
  
  audio.CSPin = SD_ChipSelectPin;   // The audio library needs to know which CS pin to use for recording
  pinMode(MIC, INPUT);  // Microphone
  pinMode(LED_Error, OUTPUT);
  pinMode(LED_Work, OUTPUT); 
  pinMode(BotonStart, INPUT_PULLUP);
  pinMode(BotonBreak, INPUT_PULLUP);
}
void loop() {
  DateTime now = rtc.now(); 
  
  confHora_1();
  confHora_2();
  confHora_3();   

    if (!digitalRead(BotonStart)||(Siguiente==1)||(now.hour()==HoraInicio && now.minute()==MinInicio && now.second()==SecInicio)) { 
     if (!SD.begin(SD_ChipSelectPin)) LedError();
     getFileName();
     for (int i = 0; i <= 99; i++){
      if (SD.exists(filename)){
      sprintf(filename, "%02d%02d%02d%02d.wav", now.hour(), now.minute(), now.second(), (i+1));
      }
    }
    SdFile::dateTimeCallback(dateTime); //Para poner los atributos en el archivo guardado     
    audio.startRecording(filename,freq_muestreo,MIC);
    t=millis();
    digitalWrite(LED_Work, HIGH);
    Siguiente=false;
    ControladorFinal=true;
    
  }
    manual_auto();

  if ((millis() - t > SaltoFichero)&&(ControladorFinal==1)){        
      digitalWrite(LED_Work, LOW);
      Siguiente=true;
      audio.stopRecording(filename);      
  }   
  else if (!digitalRead(BotonBreak)||(now.hour()==HoraFinal && now.minute()==MinFinal && now.second()==SecFinal)&&(ControladorFinal==1)){      
       LedFinal();
       audio.stopRecording(filename);
       Siguiente=false;
       ControladorFinal=false;
       return; 
  }
} //cierra void loop
