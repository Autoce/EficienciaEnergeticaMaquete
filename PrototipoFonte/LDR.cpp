#include "LDR.hpp"
#include <Arduino.h>

LDR::LDR(uint8_t inputPin, uint8_t samples, const double* polynomial, const double* reference) : inputPin(inputPin), samples(samples), polynomial(polynomial), reference(reference)
{
  double sum = 0;
  pinMode(inputPin, INPUT);
  for(uint8_t l = 0; l < samples; l++)
  {
    double luminance = getLuminanceFromHardware();
    sum += luminance;
    luxReadings.push_back(luminance);
  }
  currentLuxAvg = sum/samples;
}

void LDR::update()
{
  double sum = 0, luminance;
  luminance = getLuminanceFromHardware();
  luxReadings.pop_front();
  luxReadings.push_back(luminance);
  for(uint8_t l = 0; l < samples; l++)
    sum += luxReadings[l];
  currentLuxAvg = sum/samples;
}

double LDR::getLuminance() const
{
  return currentLuxAvg;
}

double LDR::getLuminanceFromHardware() const
{
  int analogValue = analogRead(inputPin);
  // Find the two nearest analogRead values in the array
  // int index = 0;
  // while (index < 101 - 1 && analogValue > polynomial[index + 1]) {
  //   index++;
  // }

  // // Perform linear interpolation
  // float analogMin = polynomial[index];
  // float analogMax = polynomial[index + 1];
  // float luxMin = reference[index];
  // float luxMax = reference[index + 1];
  // float lux = ((analogValue - analogMin) * (luxMax - luxMin) / (analogMax - analogMin)) + luxMin;

  float lux = polynomial[0] * exp(polynomial[1]*analogValue);

  return lux;
}
