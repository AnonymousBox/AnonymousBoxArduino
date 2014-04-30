#include <PS2Keyboard.h>
#include "KeyboardGrabber.h"

KeyboardGrabber :: KeyboardGrabber(int dataPin, int irqPin){
    keyb.begin(dataPin, irqPin);
    inputCounter = 0;
}
void KeyboardGrabber :: reset(){
    inputCounter = 0;
    memset(inputHolder, 0, sizeof(inputHolder));
}
char* KeyboardGrabber :: getInput(){
    return inputHolder;
}
void KeyboardGrabber :: grab(char c){
    switch (c >= 97 && c <= 122 && inputCounter <= 168 || c == 32 ){
        case true:
            inputHolder[inputCounter] = c;
            inputCounter++;
        case false:
            if(c == PS2_DELETE && inputCounter > 0){
                inputCounter--;
                inputHolder[inputCounter] = 32;
            }

    }

}

