#include <WiFi.h>
//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Artnet.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

char auth[] = "";

#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define INTERRUPT_THRESHOLD 1
#include "FastLED.h"
FASTLED_USING_NAMESPACE
#define FASTLED_SHOW_CORE 0


#define LED_WIDTH 80
#define LED_HEIGHT 32
#define NUM_LEDS LED_WIDTH*LED_HEIGHT
#define UNIVERSE_SIZE 170
#define PIXEL_PER_PIN NUM_LEDS/5 
/*we will  use 5 pins each pin will drive two 16x16 panel hence 512 pix  it will take 15ms to refresh your entire panel instead of 76ms
hence having a refresh rate of 65fps instead of 13fps*/
#define PIN1 15   
#define PIN2 4    
#define PIN3 27
#define PIN4 25   
#define PIN5 13
#define NB_FRAMES_TO_RECORD 2000
#define BUFFER_SIZE 10
//CRGB leds[NUM_LEDS];
static uint8_t frame[NUM_LEDS*3]; //transfer files

char filename[256];
char SAVE_NAME[]="savedata"; //the name of your save
Artnet artnet;

File root;
static File  myFile ;
static TaskHandle_t FastLEDshowTaskHandle2 = 0;
static TaskHandle_t userTaskHandle = 0;

static uint32_t deb=0;
static uint32_t fin=0;

static bool new_record=true; 
int recordingnumber=0;

int record=0;

IPAddress local_ip(192,168,137,10);             // if you want a static network-setup
IPAddress gateway_ip(192,168,137,1);
IPAddress subnet_mask(255, 255, 255, 0);    
IPAddress dns_ip(8,8,8,8);

const char *ssid  = "";                           //your wifi setup 
const char *pass = "";

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(V1)
{ record=param.asInt();
}

void saveframes()
{
    if (userTaskHandle == 0) {
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
        // -- Store the handle of the current task, so that the show task can
        //    notify it when it's done
       // noInterrupts();
        userTaskHandle = xTaskGetCurrentTaskHandle();
        
        // -- Trigger the show task
        xTaskNotifyGive(FastLEDshowTaskHandle2);
        //to thge contrary to the other one we do not wait for the display task to come back
    }
}



void FastLEDshowTask2(void *pvParameters)
{
  //Serial.printf("djdj\n");

  
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 500 );
    // -- Run forever...
    for(;;) {
        // -- Wait for the trigger
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        
           while(!new_record)
           {
                 
                     // memcpy(leds,Tpic,LED_WIDTH*LED_HEIGHT*sizeof(CRGB));
                     delay(1 );
              if(artnet.nbframeread+BUFFER_SIZE-1<artnet.frameslues)
              {
              fin=millis();
               memcpy(frame,artnet.getframe(),NUM_LEDS*sizeof(CRGB));
          
                  myFile.write(frame,NUM_LEDS*sizeof(CRGB));
                 
                  deb=deb+millis()-fin;
                     //userTaskHandle=0; //so we can't have two display tasks at the same time
              }
                 
           }
           userTaskHandle=0;
          } 
}

uint16_t compteur=0;


//----------------------------------------------------SETUP---------------------------------------------------------

void setup() {

  
Serial.begin(115200);

 if(!SD.begin(5,SPI,50000000)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

               
 WiFi.mode(WIFI_STA);
                WiFi.config(local_ip, gateway_ip, subnet_mask, dns_ip);

                WiFi.begin(ssid, pass);
                while (WiFi.status() != WL_CONNECTED)
                {
                    Serial.println(WiFi.status());
                    delay(500);
                    //Serial.print(".");
                    
                }
             Blynk.config(auth);
             Blynk.connect();
             delay(500);
             Serial.println(WiFi.localIP());

//Blynk.begin(auth, ssid, pass);  // If you want to login with Blynk over Wifi
  /*
   WiFi.mode(WIFI_STA);
    
    Serial.printf("Connecting ");
   WiFi.begin("", "");                // Put in your Wifi login data

    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(WiFi.status());
   
      
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
*/
    
  // myFile = SD.open(filename, FILE_WRITE);
   
    xTaskCreatePinnedToCore(FastLEDshowTask2, "FastLEDshowTask2", 3000, NULL,1, &FastLEDshowTaskHandle2, FASTLED_SHOW_CORE);
    
    
 /* 
  FastLED.addLeds<WS2812B, PIN1, GRB>(leds, 0*PIXEL_PER_PIN, PIXEL_PER_PIN); 
  FastLED.addLeds<WS2812B, PIN2, GRB>(leds, 1*PIXEL_PER_PIN, PIXEL_PER_PIN); 
  FastLED.addLeds<WS2812B, PIN3, GRB>(leds, 2*PIXEL_PER_PIN, PIXEL_PER_PIN); 
  FastLED.addLeds<WS2812B, PIN4, GRB>(leds, 3*PIXEL_PER_PIN, PIXEL_PER_PIN); 
  FastLED.addLeds<WS2812B, PIN5, GRB>(leds, 4*PIXEL_PER_PIN, PIXEL_PER_PIN); 
    */

artnet.begin(NUM_LEDS+160,UNIVERSE_SIZE,BUFFER_SIZE); //the number of pixels and the maximum size of your iniverses
 Serial.println("Ready to record...");

    saveframes();   

}

bool stopped=false;


void loop() {
  Blynk.run();
  while(record==1){
    Blynk.run();
  if(new_record)
  {
        memset(filename, 0, 255);
    sprintf(filename,"/%s_%03d.dat",SAVE_NAME,recordingnumber);
    myFile = SD.open(filename, FILE_WRITE);
    new_record=false;
  saveframes();
Serial.println("start new recording");
  } 
    
 
      
    if(!new_record)
  {   
      artnet.read(); 
      if(artnet.nbframeread==0)
    {
          Serial.println("Recording has started");
          Serial.printf("Filename:%s\n",filename);
    }
          
  } 
  }
  if(!new_record)
  {
  myFile.close();
     Serial.println("Recording stopped ...");
     Serial.printf("frames saved:%d frames lues:%d lost frames:%d\n",artnet.nbframeread,artnet.frameslues,artnet.lostframes)   ; 
     Serial.printf("time per frame %f\n",(float)deb/(artnet.nbframeread));
     artnet.nbframeread=0;
   artnet.frameslues=0;
   artnet.lostframes=0;
   deb=0;
  stopped=true;
  new_record=true;
  recordingnumber++;
  }
        
}
