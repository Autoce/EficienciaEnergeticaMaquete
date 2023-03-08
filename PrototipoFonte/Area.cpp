#include "Area.hpp"

Area::Area(uint8_t inputPin, uint8_t outputPin, uint8_t channel, uint8_t samples, const double* polynomial, double Kp, double Ki, double Kd, double N, double Ts)
{
  areaLED = new LED(outputPin, channel);
  areaLDR = new LDR(inputPin, samples, polynomial);
  areaPID = new PID(Kp, Ki, Kd, N, Ts);
}

Area::~Area()
{
  delete areaLED;
  delete areaLDR;
  delete areaPID;
}

void Area::update(double reference) const
{
  areaLDR->update();
  const double Lx = areaLDR->getLuminance();
  const uint16_t PWM_OUT = areaPID->Compute(reference, Lx);
  areaLED->analogWrite(PWM_OUT);
}

AreaInfo_t Area::getInformation() const
{
  AreaInfo_t tmp;
  tmp.Lx = areaLDR->getLuminance();
  tmp.dutyCycle = areaLED->getDutyCycle();
  return tmp;
}
