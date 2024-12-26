#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <BH1750.h>

#define             SDA_PIN 17  // Define your custom SDA pin
#define             SCL_PIN 18  // Define your custom SCL pin

TFT_eSPI            tft = TFT_eSPI();
char                lux_buf[20];
BH1750              lightMeter;

const int           pulseA = 44;
const int           pulseB = 43;
volatile int        lastEncoded = 0;
volatile long       encoderValue = 0;
char                rotary_buf[20];

IRAM_ATTR void handleRotary() {
    // Never put any long instruction
    int MSB = digitalRead(pulseA); //MSB = most significant bit
    int LSB = digitalRead(pulseB); //LSB = least significant bit

    int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
    int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
    lastEncoded = encoded; //store this value for next time
    if (encoderValue > 255) {
        encoderValue = 255;
    } else if (encoderValue < 0 ) {
        encoderValue = 0;
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I²C with custom pins
    lightMeter.begin();
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Lamp Control", 80, 20, 4);
    tft.drawString("Lux : ", 30, 60, 2);
    tft.drawString("Set : ", 30, 90, 2);

    pinMode(pulseA, INPUT_PULLUP);
    pinMode(pulseB, INPUT_PULLUP);
    attachInterrupt(pulseA, handleRotary, CHANGE);
    attachInterrupt(pulseB, handleRotary, CHANGE);
}

void loop() {
    float lux = lightMeter.readLightLevel();
    sprintf(lux_buf, "%.2f", lux);
    int rotary = map(encoderValue, 0, 255, 0, 5000);
    sprintf(rotary_buf, "%d", rotary);
    tft.fillRect(90, 60, 90, 50, TFT_BLACK);
    tft.drawString(lux_buf, 90, 60, 2);
    tft.drawString(rotary_buf, 90, 90, 2);
    if (lux < rotary) {
        tft.fillCircle(240, 110, 50, TFT_ORANGE);
    } else {
        tft.fillCircle(240, 110, 50, TFT_BLACK);
    }
    delay(500);
}