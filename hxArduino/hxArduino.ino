/**
   This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org>
*/

// Data PIN of 433Mhz sender
#define TRANSMIT_PIN 7

// Protocoll seems to use 2 types of pulses
#define SHORT_PULSE 300
#define LONG_PULSE 680

// Every ringtone starts with this sequence (0 = short, 1 = long)
const uint16_t common_sequence = (B00110011 *  256) + B00101011;

// this selects the ring tone
const uint8_t ring_tones[] = {
  B01010100,
  B01010011,
  B01001011,
  B00110100,
  B00110011, // Phone
  B00101101, // Ding Dong, Ding Dong
  B00101011, // School Bell
  B00101100 // Dog
};

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting");

  pinMode(TRANSMIT_PIN, OUTPUT);
  digitalWrite(TRANSMIT_PIN, LOW);
}

void loop()
{
  if ( Serial.available() ) {
    uint8_t currentRingTone = Serial.parseInt();
    if (currentRingTone > 7) {
      Serial.println("Invalid ringtone!");
    } else {
      // Send it "a few" times
      for (int x = 0; x < 100; x++) {
        ring(currentRingTone);
      }
    }
  }
}

void sendPulse(int state, int duration)
{
  if (state == 1) {
    digitalWrite(TRANSMIT_PIN, HIGH);
  } else {
    digitalWrite(TRANSMIT_PIN, LOW);
  }

  delayMicroseconds(duration);
}

void ring(int ringTone)
{
  Serial.println("Ringing bell!");
  sendPulse(1, LONG_PULSE); // Sync
  sendPulse(0, 5000);

  uint8_t ringToneSequence = ring_tones[ringTone];

  int state = 1;

  // Send signal
  for (byte x = 15; x < 16; x--) {
    if (common_sequence & (1 << x)) {
      sendPulse(state, LONG_PULSE);
    } else {
      sendPulse(state, SHORT_PULSE);
    }
    state = !state;
  }

  for (byte x = 7; x < 8; x--) {
    if (ringToneSequence & (1 << x)) {
      sendPulse(state, LONG_PULSE);
    } else {
      sendPulse(state, SHORT_PULSE);
    }
    state = !state;
  }
}
