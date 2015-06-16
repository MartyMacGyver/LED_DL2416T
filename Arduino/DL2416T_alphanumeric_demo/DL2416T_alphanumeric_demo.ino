
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//    Arduino demo for DL2416T displays
/*
    Copyright (c) 2015 Martin F. Falatic
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const int PIN_DS = 2;
const int PIN_CP = 3; 
const int PIN_A0 = 4;
const int PIN_A1 = 5;
const int PIN_WR = 6;
const int PIN_SE = 7;

const int DELAY_SHIFT = 0;
const int DELAY_LATCH = 0;
const int DELAY_CURSOR = 10;
const int DELAY_CHAR = 30;
const int MAX_CHARS = 8;  // Adjust accordingly - I used a 74HC595 to multiplex them

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_SE, OUTPUT); digitalWrite(PIN_SE, HIGH);
    pinMode(PIN_WR, OUTPUT); digitalWrite(PIN_WR, HIGH);
    pinMode(PIN_A0, OUTPUT); digitalWrite(PIN_A0, LOW );
    pinMode(PIN_A1, OUTPUT); digitalWrite(PIN_A1, LOW );
    pinMode(PIN_DS, OUTPUT); digitalWrite(PIN_DS, LOW );
    pinMode(PIN_CP, OUTPUT); digitalWrite(PIN_CP, LOW );
}

void ssr_shift(int PIN_STATE) {
    digitalWrite(PIN_DS, PIN_STATE);
    digitalWrite(PIN_CP, HIGH);
    delay(DELAY_SHIFT);
    digitalWrite(PIN_CP, LOW);
    delay(DELAY_SHIFT);
}

void led_set_cursor(int pos, boolean showCursor) {
    ssr_shift(LOW);  // cursor control (active - D0 will set its visibility)
    ssr_shift(LOW);  // D6
    ssr_shift(LOW);  // D5
    ssr_shift(LOW);  // D4
    ssr_shift(LOW);  // D3
    ssr_shift(LOW);  // D2
    ssr_shift(LOW);  // D1
    if (showCursor) ssr_shift(HIGH);  // D0
    else            ssr_shift(LOW);   // D0
    ssr_shift(LOW); // Final shift (value = don't care)
    led_write_address(pos);
}

void led_set_char(int pos, int character) {
    character = character & 0b01111111;
    ssr_shift(HIGH);  // cursor control (inactive - we're writing a character)
    ssr_shift((character & 64) ? HIGH:LOW); // D6
    ssr_shift((character & 32) ? HIGH:LOW); // D5
    ssr_shift((character & 16) ? HIGH:LOW); // D4
    ssr_shift((character &  8) ? HIGH:LOW); // D3
    ssr_shift((character &  4) ? HIGH:LOW); // D2
    ssr_shift((character &  2) ? HIGH:LOW); // D1
    ssr_shift((character &  1) ? HIGH:LOW); // D0
    ssr_shift(LOW); // Final shift (value = don't care)
    led_write_address(pos);
}

void led_write_address(int pos) {
    digitalWrite(PIN_A0, (pos & 1) ? LOW:HIGH);
    digitalWrite(PIN_A1, (pos & 2) ? LOW:HIGH);
    if (pos & 4) 
        digitalWrite(PIN_SE, HIGH);
    else 
        digitalWrite(PIN_SE, LOW);
    digitalWrite(PIN_WR, LOW);
    delay(DELAY_LATCH);
    digitalWrite(PIN_WR, HIGH);
    delay(DELAY_LATCH);
}

int a = 0;
char buffer[] = "        THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG!        ";
char * bptr = buffer;

int mode = 2;

void loop()
{
    if (*bptr == 0) bptr = buffer;
    char thischar = *bptr;
    Serial.print(thischar);

    if (mode == 0) {
        led_set_cursor(a, true);
        delay(DELAY_CURSOR);
        led_set_cursor(a, false);
        led_set_char(a, thischar);
    }
    else if (mode == 1) {
        led_set_char(a, random(32, 96)); // 32 <= ord(char) <= 95
    }
    else if (mode == 2) {
        led_set_char(random(0, MAX_CHARS), random(32, 96)); // 32 <= ord(char) <= 95
    }

    a++; if (a >= MAX_CHARS) { a = 0; }
    bptr++;

    delay(DELAY_CHAR);
}


