#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "ST7565.h"
#include <PS2Keyboard.h>

#define BACKLIGHT_LED 10

// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);
const int keyboardDataPin = 3;
const int keyboardIRQPin = 2;

char inputHolder[200];
enum States {START, SHOWOLDMESSAGE, RECIEVENEW, END};
States currentState = START;

bool shown = false;

PS2Keyboard keyboard;

char oldMessage[200];
void setup()   {                
  Serial.begin(9600);
  EEPROM_readAnything(0, oldMessage);
  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  //initialize keyboard
  keyboard.begin(keyboardDataPin, keyboardIRQPin);
  // initialize and set the contrast to 0x18
  glcd.begin(0x18);
  }

void loop()                     
{
    switch (currentState){
        case START:
            if(!shown){
                showStartText();
            }else{
                if(isEnter() ==1){

                    shown = false;
                    currentState = SHOWOLDMESSAGE;
                }
            }
            break;
        case SHOWOLDMESSAGE:
            if(!shown){ 
                glcd.clear();
                glcd.drawstring(0, 0, "The last message: ");
                glcd.display();
                delay(2000);
                showOldMessage();
                delay(3000);
            }else{
                glcd.clear();
                glcd.drawstring(0, 1, "Type your message,");
                glcd.drawstring(0, 2, "No Caps, No Numbers");
                glcd.drawstring(40,4 , "Then");
                glcd.drawstring(0, 6, "Press Enter to send");
                glcd.display();
                currentState = RECIEVENEW;
                break;
            }
            break;
        case RECIEVENEW:
            waitTime(20000);
            gatherKeyboardText();
            break;
        case END:
            if(!shown){
                glcd.clear();
                glcd.drawstring(0, 1, "Thank You");
                glcd.display();
                shown = true;
            }else{
                delay(1000);
                glcd.clear();
                glcd.display();
                delay(1000);
                shown = false;
                currentState = START;
                break;
            }
            
    }   
}
void waitTime(long interval){
    static long starttime = millis();
    long curtime = millis();
    static bool didit = false;
    if(didit){
        starttime = millis();
        didit = false;
    }
    if(curtime > (starttime + interval)){
        didit = true; 
        shown = false;
        currentState = START;
    }

}
bool gatherKeyboardText(){
    static long starttime = millis();
    long curtime = millis();
    static int inputCounter = 0;
    if(keyboard.available()){
        char c = keyboard.read();
        switch (c >= 97 && c <= 122 && inputCounter < 168 || c == 32 ){
            case true:
                
                inputHolder[inputCounter] = c;
                inputCounter++;
                glcd.clear();
                glcd.drawstring(0, 0, inputHolder);
                glcd.display();
                return true;
                break;
            case false:
                if(c == PS2_DELETE && inputCounter > 0){
                    inputCounter--;
                    inputHolder[inputCounter] = 32;
                    glcd.clear();
                    glcd.drawstring(0, 0, inputHolder);
                    glcd.display();
 
                    return true;
                    break;
                }else if(c == 13){
                    inputCounter = 0;
                    EEPROM_writeAnything(0, inputHolder);
                    sendMessageAndData(inputHolder, curtime-starttime);
                    strcpy(oldMessage, inputHolder);
                    memset(inputHolder, ' ', sizeof(inputHolder));
                    shown = false;
                    currentState = END;
                    break;
                }
                return false;
                break;
        }
    }else{
        return false;
    }

}
void sendMessageAndData(char message[200], long timetyping){
    Serial.print("{\"message\": \"");
    Serial.print(message);
    Serial.print("\", \"timetyping\": ");
    Serial.print(timetyping);
    Serial.print("}");
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

void showOldMessage(){
    glcd.clear();
    glcd.drawstring(0, 0, oldMessage);
    glcd.display();
    shown = true;
}
void showStartText(){
    glcd.clear();
    glcd.drawstring(0, 0, "Would you like to");
    glcd.drawstring(0, 1, "send a message to the ");
    glcd.drawstring(0, 2, "next person who sees this screen?");
    glcd.drawstring(0, 5, "Press Enter");
    glcd.display();
    shown = true;
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

