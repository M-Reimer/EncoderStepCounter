#include "EncoderStepCounter.h"

// These are for my hardware which has bad interrupt settings
// https://github.com/NicoHood/HoodLoader2/issues/67
// For official Arduino boards, use the commented out lines
// and only adapt the ENCODER_PINx values

#define ENCODER_PIN1 14
#define ENCODER_INT1 3
//#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 19
#define ENCODER_INT2 6
//#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)

// Create instance for one half step encoder
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);

void setup() {
  Serial.begin(9600);
  // Initialize encoder
  encoder.begin();
  // Initialize interrupts
  attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
  attachInterrupt(ENCODER_INT2, interrupt, CHANGE);
}

// Call tick on every change interrupt
void interrupt() {
  encoder.tick();
}

// This is an example on how to change a "long" variable
// with the library. With every loop the value is added
// and then cleared in the encoder library
signed long position = 0;
void loop() {
  signed char pos = encoder.getPosition();
  if (pos != 0) {
    position += pos;
    encoder.reset();
    Serial.println(position);
  }
}
