#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "ST7565.h"
#include <PS2Keyboard.h>

#define BACKLIGHT_LED 10

#define DEBUG 1
// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);
const int keyboardDataPin = 3;
const int keyboardIRQPin = 2;
const int sonarPin = 4;

char inputHolder[168];
enum States {START, SHOWOLDMESSAGE, RECIEVENEW, END};
States currentState = START;

bool shown = false;

PS2Keyboard keyboard;

char oldMessage[168];
void setup()   {                
  Serial.begin(9600);
  EEPROM_readAnything(0, oldMessage);
  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  pinMode(sonarPin,INPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  //initialize keyboard
  keyboard.begin(keyboardDataPin, keyboardIRQPin);
  // initialize and set the contrast to 0x18
  glcd.begin(0x18);
  }
void waitTime(long inter, void (*f)(bool)){
    static long starttime = 0;
    static bool started = false;
    if(started){
        
        long curtime = millis();
        if(curtime > (starttime + inter)){
            started = false;
            Serial.println("started again");
            reactSonar();
            (*f)(false);
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
            }else{
                showStartText();
            }
            break;
        case SHOWOLDMESSAGE:
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
            currentState = RECIEVENEW;
            break;
        case RECIEVENEW:
            waitTime(3000, gatherKeyboardText);
            break;
        case END:
            glcd.clear();
            glcd.drawstring(0, 1, "Thank You");
            glcd.display();
            delay(1000);
            glcd.clear();
            glcd.display();
            delay(1000);
            currentState = START;
            break;
    }   
}

void gatherKeyboardText(bool reset){
    static bool startfunc = false;
    static long starttime = millis();
    static int inputCounter = 0;
    static int distance = 0; 
    if(startfunc){
        starttime = millis();
        startfunc = false;
    }
    if(reset == true){
        Serial.print("reseting");
        inputCounter = 0;
        memset(inputHolder, 0, sizeof(inputHolder));
        startfunc = true;

         
    }
    long curtime = millis();
    if(keyboard.available()){
        char c = keyboard.read();
        switch (c >= 97 && c <= 122 && inputCounter <= 168 || c == 32 ){
            case true:
                inputHolder[inputCounter] = c;
                inputCounter++;
                glcd.clear();
                glcd.drawstring(0, 0, inputHolder);
                glcd.display();
                break;
            case false:
                if(c == PS2_DELETE && inputCounter > 0){
                    inputCounter--;
                    inputHolder[inputCounter] = 32;
                    glcd.clear();
                    glcd.drawstring(0, 0, inputHolder);
                    glcd.display();
                    break;
                }else if(c == 13){
                    inputCounter = 0;
                    EEPROM_writeAnything(0, inputHolder);
                    sendMessageAndData(inputHolder, curtime-starttime, checkSonar());
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
void sendMessageAndData(char message[168], long timetyping, int distance){
    Serial.print("{\"message\": \"");
    Serial.print(message);
    Serial.print("\", \"staytime\": \"");
    Serial.print(timetyping);
    Serial.print("\", \"distance\": \"");
    Serial.print(distance);
    Serial.print("\"}");
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
}
void showStartText(){
    glcd.clear();
    glcd.drawstring(0, 0, "Would you like to");
    glcd.drawstring(0, 1, "send a message to the ");
    glcd.drawstring(0, 2, "next person who sees this screen?");
    glcd.drawstring(0, 5, "Press Enter");
    glcd.display();
}
void reactSonar(){
    int sonVal = checkSonar();
    Serial.println(sonVal);
    if(currentState == RECIEVENEW){
        if(sonVal > 50){
            Serial.println("left");
            delay(1000);
            gatherKeyboardText(true);
            currentState = START;

        }
    }
    
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

