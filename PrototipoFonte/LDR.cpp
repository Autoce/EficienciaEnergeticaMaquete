#include "LDR.hpp"
#include <Arduino.h>

LDR::LDR(uint8_t inputPin, uint8_t samples, const double* polynomial) : inputPin(inputPin), samples(samples), polynomial(polynomial)
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
  const uint16_t ADC = analogRead(inputPin);
  auto ADCtoLx = [](uint16_t ADC, const double* polynomial)
  {
    double Lx = 0;
    double scaledADC = ADC / 1000.0;
    for (uint8_t i = 0; i < 8; i++)
      Lx += polynomial[i] * pow(scaledADC, 7 - i);
    if (Lx < 0) return (double)0;
    return Lx;
  };
  return ADCtoLx(ADC, polynomial);
}
