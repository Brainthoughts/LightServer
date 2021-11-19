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
#define STASSID "ssid"
#define STASPSK "password"
#endif
#define PIN 14

const char *ssid = STASSID;
const char *password = STAPSK;
float brightness = .5;
String currentColorHex;
uint32_t currentColor;
String message;

ESP8266WebServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

int strToHex(const char str[]) {
    return (int) strtol(str, 0, 16);
}

void colorWipe(uint32_t c, uint8_t wait) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

void handleRoot() {
    server.send(200, "text/html", String("<h1>Light Changer</h1>") +
                                  String("<form action='/' method='POST'><input name='color' type='color' value='") +
                                  String(currentColorHex) +
                                  String("'/> <input name='text' type='text' placeholder='What to display' /> <input name='submit' type='submit' /></form>"));
}

void postRoot() {
    Serial.print("Color: ");
    Serial.println(server.arg("color"));
    currentColorHex = server.arg("color");
    int r = (int) strToHex(server.arg("color").substring(1, 3).c_str());
    int g = (int) strToHex(server.arg("color").substring(3, 5).c_str());
    int b = (int) strToHex(server.arg("color").substring(5).c_str());
    currentColor = strip.Color(r, g, b);
    message = server.arg("text");
    Serial.print("Message: ");
    Serial.println(message);
    handleRoot();
    Serial.println(currentColor);
    colorWipe(currentColor, 10);
}

void handleNotFound() {
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
    currentColorHex = "#000000";
    currentColor = 0;
    message = "";
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
    server.handleClient();
    MDNS.update();
}


