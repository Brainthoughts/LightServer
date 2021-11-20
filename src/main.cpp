#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
//#include <helpers.h>
#include <string>
#include <creds.h>

#ifdef __AVR__
#include <avr/power.h>
#endif
#ifndef STASSID
#define STASSID "VUSD-Guest"
#define STAPSK NULL
#endif
#define PIN 14

const char *ssid = STASSID;
const char *password = STAPSK;
float brightness = .25;
String currentColorHex;
uint32_t currentColor;
String message;

const int startingIndex = 3;
const int displayWidth = 16;
const int displayBuffer[7] = {5, 5, 5, 5, 5, 5, 5};

const int characterSpacing = 2;
const String endSpacing = "  ";
boolean scroll = true;
int scrollPosition = 0;
long long lastIncrease = 500;
long long increaseDelay = 100;

const int letterWidth = 5;
const int letterHeight = 7;
const int LETTERS[37][letterHeight][letterWidth] =
        {
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}}, // A
                {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}}, // B
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // C
                {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}}, // D
                {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}, // E
                {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}}, // F
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // G
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}}, // H
                {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1}}, // I
                {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // J
                {{1, 0, 0, 0, 1}, {1, 0, 0, 1, 0}, {1, 0, 1, 0, 0}, {1, 1, 0, 0, 0}, {1, 0, 1, 0, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}}, // K
                {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}, // L
                {{1, 1, 0, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}}, // M
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}}, // N
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // O
                {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}}, // P
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {0, 1, 1, 1, 1}}, // Q
                {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}}, // R
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // S
                {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}}, // T
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // U
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}}, // V
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {0, 1, 0, 1, 0}}, // W
                {{1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {1, 0, 0, 0, 1}}, // X
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}}, // Y
                {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}, // Z
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 1
                {{0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}}, // 2
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 0, 0}, {1, 1, 1, 1, 1}}, // 3
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 1, 1, 0}, {0, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 4
                {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}}, // 5
                {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 6
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 7
                {{1, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}}, // 8
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 9
                {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 1}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 1, 1, 1, 0}}, // 0
                {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}}, // EMPTY
        };


ESP8266WebServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

int strToHex(const char str[]) {
//    Serial.println("strToHex");
    return (int) strtol(str, nullptr, 16);
}

void colorWipe(uint32_t c, uint8_t wait) {
//    Serial.println("colorWipe");
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

int letterIndex(char letter) {
//    Serial.println("letterIndex");
    if (letter > 47 && letter < 58)
        return letter - 22;
    if (letter > 64 && letter < 91)
        return letter - 65;
    if (letter > 96 && letter < 123)
        return letter - 97;
    return 36;
}

void updateDisplay() {
//    Serial.println("updateDisplay");
    strip.clear();
//    Serial.println(scrollPosition);
    if (message.length() == 0) {
//        Serial.println("emptyMessage");
        strip.show();
        return;
    }

    if (scroll && lastIncrease + increaseDelay < millis()) {
//        Serial.print("Before: ");
//        Serial.println(lastIncrease);
        lastIncrease = millis();
//        Serial.print("After: ");
//        Serial.println(lastIncrease);
        scrollPosition++;
    }

    int index = startingIndex;

    int totalWidth = letterWidth + characterSpacing;

    for (int row = 6; row >= 0; row--) {

        if (row % 2 == 0) {
            for (int i = 0; i < displayWidth; i++) {
                int iPosition = scrollPosition + i;

                int letter = iPosition / totalWidth % message.length();

                int column = iPosition % totalWidth;

                if (column < letterWidth && LETTERS[letterIndex(message[letter])][row][column]) {
//                    Serial.println("column<#1");
                    strip.setPixelColor(index, currentColor);
                }

                index++;
            }
        } else {
            for (int i = displayWidth - 1; i >= 0; i--) {
                int iPosition = scrollPosition + i;

                int letter = iPosition / totalWidth % message.length();

                int column = iPosition % totalWidth;

                if (column < letterWidth && LETTERS[letterIndex(message[letter])][row][column]) {
//                    Serial.println("column<#2");
                    strip.setPixelColor(index, currentColor);
                }

                index++;
            }
        }
//        Serial.println("index+=");
        index += displayBuffer[row];

    }

    strip.show();
}


void handleRoot() {
//    Serial.println("handleRoot");
    server.send(200, "text/html", String("<h1>Light Changer</h1>") +
                                  String("<form action='/' method='POST'><label for='color'>Color: </label><input name='color' type='color' id='color' value='") +
                                  String(currentColorHex) +
                                  String("'/><br> <label for='scroll'>Message:</label> <input name='message' type='text' id='message' value='") +
                                  String(message).substring(0, message.length() - endSpacing.length()) +
                                  String("' /> <label for='scroll'>Scroll:</label> <input name='scroll' id='scroll' type='checkbox' ") +
                                  String(scroll ? "checked" : "") +
                                  String("/> <br><label for='speed'>Speed:</label> <input name='speed' type='number' id='speed' min=1 value='") +
                                  String(increaseDelay) +
                                  String("'/> <input name='submit' type='submit' /></form>")
    );
    scrollPosition = 0;
}

void postRoot() {
//    Serial.println("postRoot");
//    Serial.print("Color: ");
//    Serial.println(server.arg("color"));
    currentColorHex = server.arg("color");
    int r = (int) strToHex(server.arg("color").substring(1, 3).c_str());
    int g = (int) strToHex(server.arg("color").substring(3, 5).c_str());
    int b = (int) strToHex(server.arg("color").substring(5).c_str());
    currentColor = strip.Color(r, g, b);
    message = server.arg("message") + endSpacing;
    Serial.println(server.arg("scroll"));
    scroll = server.arg("scroll") == "on";
    increaseDelay = server.arg("speed").toInt();
//    Serial.print("Message: ");
//    Serial.println(message);
    handleRoot();
    Serial.println(currentColor);
}

void handleNotFound() {
//    Serial.println("handleNotFound");
    String text = "File Not Found\n\n";
    text += "URI: ";
    text += server.uri();
    text += "\nMethod: ";
    text += (server.method() == HTTP_GET) ? "GET" : "POST";
    text += "\nArguments: ";
    text += server.args();
    text += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        text += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", text);
}

void setup(void) {
//    Serial.println("setup");
    currentColorHex = "#ffffff";
    currentColor = strip.Color(255, 255, 255);
    message = "test" + endSpacing;;
    strip.begin();
    strip.setBrightness(brightness * 255);
    strip.fill(strip.Color(0, 0, 0));
    strip.show(); // Initialize all pixels to 'off'


    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, postRoot);

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
//    Serial.println("loop");
    server.handleClient();
    MDNS.update();
    updateDisplay();
}