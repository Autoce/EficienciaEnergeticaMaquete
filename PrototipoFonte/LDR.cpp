#include "LDR.hpp"
#include <Arduino.h>
#define AMOSTRAS_MED 100

LDR::LDR(uint8_t inputPin, const double* polynomial) : inputPin(inputPin), polynomial(polynomial)
{
  double sum = 0;
  for(uint8_t l = 0; l < AMOSTRAS_MED; l++)
  {
    double luminance = getLuminanceFromHardware();
    sum += luminance;
    luxReadings.push_back(luminance);
  }
  currentLuxAvg = sum/AMOSTRAS_MED;
}

void LDR::update()
{
  double sum = 0, luminance;
  luminance = getLuminanceFromHardware();
  luxReadings.pop_front();
  luxReadings.push_back(luminance);
  for(uint8_t l = 0; l < AMOSTRAS_MED; l++)
    sum += luxReadings[l];
  currentLuxAvg = sum/AMOSTRAS_MED;
}

double LDR::getLuminance()
{
  return currentLuxAvg;
}

double LDR::getLuminanceFromHardware()
{
  uint16_t ADC = analogRead(inputPin);
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
