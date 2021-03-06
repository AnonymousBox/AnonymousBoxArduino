#include <EEPROM.h>
#include "thankyouBMP.h"
#include "EEPROMAnything.h"
#include "QrCode.h"
#include "ST7565.h"
#include <PS2Keyboard.h>

#define BACKLIGHT_LED 10

#define DEBUG 1
ST7565 glcd(9, 8, 7, 6, 5);
const int keyboardDataPin = 3;
const int keyboardIRQPin = 2;
const int sonarPin = 4;

char inputHolder[168];
char oldMessage[168];
long staytime;
//States of the code 
enum States {START, SHOWOLDMESSAGE, RECIEVENEW, END, LIMBO};
States currentState = START;
PS2Keyboard keyboard;
void(* resetFunc) (void) = 0; 

void setup()   {                
  Serial.begin(9600);
  //reads last message written to EEPROM
  EEPROM_readAnything(0, oldMessage);
  pinMode(BACKLIGHT_LED, OUTPUT);
  pinMode(sonarPin,INPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);
  keyboard.begin(keyboardDataPin, keyboardIRQPin);
  glcd.begin(0x1f);
}

void waitTime(long inter, void (*f)(bool)){
    static long starttime = 0;
    static bool started = false;
    long curtime = millis();
    if(started){
        if(curtime > (starttime + inter)){
            started = false;
            if(reactSonar()){
                (*f)(true);
                currentState = START;
            }
        }else{
            (*f)(false);
        }
    }else{
        starttime = millis();
        started = true;
    }

}

void loop()                     
{
    switch (currentState){
        case START:
           if(isEnter() ==1){
                currentState = SHOWOLDMESSAGE;

            } else {
                showStartText();
            }
            break;
        case SHOWOLDMESSAGE:
            glcd.clear();
            glcd.drawstring(20, 3, "By Using this Box");
            glcd.drawstring(20, 4, "You agree to the terms");
            glcd.drawstring(20, 5, "on this site");
            glcd.drawstring(20, 6, "bit.ly/1f1yT2n");
            glcd.display();
            delay(500);

            glcd.clear();
            glcd.drawstring(0, 0, "The last message: ");
            glcd.display();
            delay(2000);
            showOldMessage();
            delay(3000);
            glcd.clear();
            glcd.drawstring(0, 1, "Type your message,");
            glcd.drawstring(0, 2, "No Caps, No Numbers");
            glcd.drawstring(40,4 , "Then");
            glcd.drawstring(0, 6, "Press Enter to send");
            glcd.display();
            delay(2000);
            currentState = RECIEVENEW;
            break;
        case RECIEVENEW:
            gatherKeyboardText(false);
            if(reactSonar()){
                gatherKeyboardText(true);
                currentState = START;
            }
            //gatherKeyboardText(false);
            break;
        case END:
            //showEndText();
            //delay(4000);
            showEndGraphic();
            delay(1000);
            showQrGraphic();
            currentState = LIMBO;
            break;
        case LIMBO:
          if(reactSonar()){
                resetFunc();
            }else if(isEnter()){
                delay(5000);
            }
            break;

    }   
}

void gatherKeyboardText(bool reset){
    static bool startfunc = false;
    static long starttime = millis();
    static int inputCounter = 0;
    if(startfunc){
        starttime = millis();
        startfunc = false;
    }
    if(reset == true){
//        Serial.print("reseting");
        inputCounter = 0;
        memset(inputHolder, 0, sizeof(inputHolder));
        startfunc = true;
        return;
    }
    long curtime = millis();
    if(keyboard.available()){
        char c = keyboard.read();
        delay(10);
        switch (c >= 97 && c <= 122 && inputCounter <= 168 || c == 32 ){
            case true:
                inputHolder[inputCounter] = c;
                ++inputCounter;
                glcd.clear();
                glcd.drawstring(0, 0, inputHolder);
                glcd.display();
                break;
            case false:
                if(c == PS2_DELETE && inputCounter > 0){
                    inputHolder[--inputCounter] = 32;
                    glcd.clear();
                    glcd.drawstring(0, 0, inputHolder);
                    glcd.display();
                    break;
                }else if(c == 13 && !isEmpty(inputHolder)){
                    sendMessageAndData(inputHolder, curtime-starttime, checkSonar());
                    inputCounter = 0;
                    EEPROM_writeAnything(0, inputHolder);
                    strcpy(oldMessage, inputHolder);
                    memset(inputHolder, 0, sizeof(inputHolder));
                    startfunc = true;
                    currentState = END;
                    break;
                }
                break;
        }
    }
}
void sendMessageAndData(char message[168], long timetyping, int gotdist){
    Serial.print(F("{\"message\": \""));
    Serial.print(message);
    Serial.print(F("\", \"staytime\": \""));
    Serial.print(timetyping);
    Serial.print(F("\", \"distance\": \""));
    Serial.print(gotdist);
    Serial.print(F("\"}"));
}
void showOldMessage(){
    glcd.clear();
    glcd.drawstring(0, 0, oldMessage);
    glcd.display();
}
void showStartText(){
    glcd.clear();
    glcd.drawstring(0, 0, "Would you like to");
    glcd.drawstring(0, 1, "send a message to the ");
    glcd.drawstring(0, 2, "next person who sees this screen?");
    glcd.drawstring(0, 5, "Press Enter");
    glcd.display();
}
/*void showEndText(){
    glcd.clear();
    glcd.drawstring(0,0, "To see all the messages");
    glcd.drawstring(0,1, "go to to this site:");
    glcd.drawstring(0,2, "bit.ly/1tHLFqN");
    glcd.display();
}*/
void showEndGraphic(){
    glcd.clear();
    glcd.drawbitmap(0,0,thankyou, 128,64, BLACK);
    glcd.display();
}
void showQrGraphic(){
    glcd.clear();
    glcd.drawbitmap(0,0,qrcode, 128,64, BLACK);
    glcd.display();
}
bool reactSonar(){
    static bool startfunc = false;
    static long starttime = millis();
    long curtime = millis();
    if(startfunc){
        starttime = millis();
        startfunc = false;
    }
    //Serial.println(sonVal);
    if((curtime - starttime)  > 3000){
        startfunc = true;
        if(checkSonar() > 50){
            //Serial.println("left");
            return true;
        }
    }
    return false;
}
int checkSonar(){
    static long pulse = 0;
    static int arraysize = 9;
    static int modE;
    static int rangevalue[] = {0,0,0,0,0,0,0,0,0};
    for(int i = 0; i < arraysize; i++)
    {
        pulse = pulseIn(sonarPin, HIGH);
        rangevalue[i] = pulse/147;
        delay(10);
    }
    modE = mode(rangevalue,arraysize);
    return modE;
}
int mode(int *x,int n){

    int i = 0;
    int count = 0;
    int maxCount = 0;
    int mode = 0;
    int bimodal;
    int prevCount = 0;
    while(i<(n-1)){
        prevCount=count;
        count=0;
        while(x[i]==x[i+1]){
            count++;
            i++;
        }
        if(count>prevCount&count>maxCount){
            mode=x[i];
            maxCount=count;
            bimodal=0;
        }
        if(count==0){
            i++;
        }
        if(count==maxCount){//If the dataset has 2 or more modes.
            bimodal=1;
        }
        if(mode==0||bimodal==1){//Return the median if there is no mode.
            mode=x[(n/2)];
        }
        return mode;
    }
}
bool isEmpty(char vals[168]){
    for(int i = 0; i<sizeof(vals); i++){
        char checked = vals[i];
        if(checked != 0 && checked != 32){
            return false;
        }
    }
    return true;
}
bool isEnter(){
    if(keyboard.available()){
        char c = keyboard.read();
        if(c == 13){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

