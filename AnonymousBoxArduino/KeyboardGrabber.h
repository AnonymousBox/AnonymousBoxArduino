class KeyboardGrabber{
    public:
        KeyboardGrabber(int dataPin, int irqPin);
        void send();
        void reset(void);
        void grab(char);
        char* getInput();
        PS2Keyboard keyb;
    private:
        int inputCounter;
        char inputHolder[168];
        bool started;


};
