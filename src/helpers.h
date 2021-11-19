//
// Created by Alexander Niedner on 11/18/21.
//

#ifndef LIGHTSERVER_HELPERS_H
#define LIGHTSERVER_HELPERS_H

#endif //LIGHTSERVER_HELPERS_H

int strToHex(const char str[]) {
    return (int) strtol(str, nullptr, 16);
}

void colorWipe(uint32_t c, uint8_t wait, Adafruit_NeoPixel strip) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}