/*
    Arduino EncoderStepCounter library
    Copyright (C) 2019 Manuel Reimer <manuel.reimer@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Arduino.h"
#include "EncoderStepCounter.h"

// Constructor
// Sets default values for attributes
EncoderStepCounter::EncoderStepCounter(int aPin1, int aPin2, EncoderType aEncType):
  encoder_pin1(aPin1),
  encoder_pin2(aPin2),
  encoder_type(aEncType),
  last_zero_dir(UNKNOWN_DIR),
  encoderpos(0)
{
}

// This one is meant to be used for derived libraries.
// You set the defalt encoder type in your initialization list.
// The user is able to override with the begin() call in the setup() routine.
void EncoderStepCounter::begin(EncoderType aEncType) {
  encoder_type = aEncType;
  begin();
}

// Does all the initialization
// Has to be called in setup()
void EncoderStepCounter::begin() {
  pinMode(encoder_pin1, INPUT_PULLUP);
  pinMode(encoder_pin2, INPUT_PULLUP);
  delay(5); // Wait for the filter cap to charge
  lastpin1 = digitalRead(encoder_pin1);
  lastpin2 = digitalRead(encoder_pin2);
  lastfull = lastpin1;
}

// Returns true if we entered a new half step (both pins are same value)
bool EncoderStepCounter::CheckEncoderPos(bool& aPosValue, EncDir& aDirection) {
  // Read current values
  bool pin1 = digitalRead(encoder_pin1);
  bool pin2 = digitalRead(encoder_pin2);

  // If nothing has changed, then exit
  if (pin1 == lastpin1 && pin2 == lastpin2)
    return false;

  // Reached a position which is not the last position
  bool retval = false;
  if (pin1 == pin2 && lastfull != pin1) {
    lastfull = pin1;
    aPosValue = pin1;

    if (lastpin1 != pin1)
      aDirection = CCW;
    if (lastpin2 != pin2)
      aDirection = CW;

    retval = true;
  }

  // Remember last values
  lastpin1 = pin1;
  lastpin2 = pin2;

  return retval;
}

// Reads the pins and detects rotation
void EncoderStepCounter::tick() {
  // Get actual position change
  bool posvalue;
  EncDir direction;
  if (!CheckEncoderPos(posvalue, direction)) return;

  // For half step encoders, each half step change counts
  if (encoder_type == HALF_STEP) {
    encoderpos += direction;
  }

  // For full step encoders, only the "one position" counts but we have to
  // check the "zero position" also for reliability.
  // Same inverted for FULL_STEP_INVERTED encoders.
  else {
    if (posvalue == 0)
      last_zero_dir = direction;
    else if (last_zero_dir == direction) {
      last_zero_dir = UNKNOWN_DIR;
      encoderpos += direction;
    }
  }
}

// Getter and Setter
signed char EncoderStepCounter::getPosition() const {
  return encoderpos;
}
void EncoderStepCounter::setPosition(signed char aPosition) {
  encoderpos = aPosition;
}
void EncoderStepCounter::reset() {
  encoderpos = 0;
}
