#include "Area.hpp"

Area::Area(uint8_t inputPin, uint8_t outputPin, uint8_t channel, uint8_t samples, const double* polynomial, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite)
{
  pwrOverWrite = OverWrite;
  mode = 0;
  areaLED0 = new LED(outputPin, channel);
  areaLDR0 = new LDR(inputPin, samples, polynomial);
  areaPID = new PID(Kp, Ki, Kd, N, Ts);
}

Area::Area(uint8_t inputPin0, uint8_t inputPin1, uint8_t outputPin0, uint8_t outputPin1, uint8_t channel0, uint8_t channel1, uint8_t samples, const double* polynomial0, const double* polynomial1, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite){
  pwrOverWrite = OverWrite;
  mode = 1;
  areaLED0 = new LED(outputPin0, channel0);
  areaLDR0 = new LDR(inputPin0, samples, polynomial0);
  areaLED1 = new LED(outputPin1, channel1);
  areaLDR1 = new LDR(inputPin1, samples, polynomial1);
  areaPID = new PID(Kp, Ki, Kd, N, Ts);
}

Area::Area(uint8_t inputPin0, uint8_t inputPin1, uint8_t inputPin2, uint8_t inputPin3, uint8_t outputPin0, uint8_t outputPin1, uint8_t outputPin2, uint8_t outputPin3, uint8_t channel0, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t samples, const double* polynomial0, const double* polynomial1, const double* polynomial2, const double* polynomial3, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite){
  pwrOverWrite = OverWrite;
  mode = 2;
  areaLED0 = new LED(outputPin0, channel0);
  areaLDR0 = new LDR(inputPin0, samples, polynomial0);

  areaLED1 = new LED(outputPin1, channel1);
  areaLDR1 = new LDR(inputPin1, samples, polynomial1);

  areaLED2 = new LED(outputPin2, channel2);
  areaLDR2 = new LDR(inputPin2, samples, polynomial2);

  areaLED3 = new LED(outputPin3, channel3);
  areaLDR3 = new LDR(inputPin3, samples, polynomial3);
  areaPID = new PID(Kp, Ki, Kd, N, Ts);
}

Area::~Area()
{
  switch (mode)
  {
  case 0:
    delete areaLED0;
    areaLED0 = nullptr;
    delete areaLDR0;
    areaLDR0 = nullptr;
    delete areaPID;
    areaPID = nullptr;
    break;
  case 1:
    delete areaLED0;
    areaLED0 = nullptr;
    delete areaLDR0;
    areaLDR0 = nullptr;
    delete areaLED1;
    areaLED1 = nullptr;
    delete areaLDR1;
    areaLDR1 = nullptr;
    delete areaPID;
    areaPID = nullptr;
    break;
  case 2:
    delete areaLED0;
    areaLED0 = nullptr;
    delete areaLDR0;
    areaLDR0 = nullptr;
    delete areaLED1;
    areaLED1 = nullptr;
    delete areaLDR1;
    areaLDR1 = nullptr;
    delete areaLED2;
    areaLED2 = nullptr;
    delete areaLDR2;
    areaLDR2 = nullptr;
    delete areaLED3;
    areaLED3 = nullptr;
    delete areaLDR3;
    areaLDR3 = nullptr;
    delete areaPID;
    areaPID = nullptr;
    break;
  default:
    break;
  }
}

void Area::update(double reference) const
{   
  if(pwrOverWrite){
    switch (mode)
    {
    case 0:
      areaLDR0->update();
      areaLED0->analogWrite(reference*10.23);
      break;

    case 1:
      areaLDR0->update();
      areaLDR1->update();
      areaLED0->analogWrite(reference*10.23);
      areaLED1->analogWrite(reference*10.23);
      break;

    case 2:
      areaLDR0->update();
      areaLDR1->update();
      areaLDR2->update();
      areaLDR3->update();
      areaLED0->analogWrite(reference*10.23);
      areaLED1->analogWrite(reference*10.23);
      areaLED2->analogWrite(reference*10.23);
      areaLED3->analogWrite(reference*10.23);
      break;

    default:
      break;
    }    
  }
  else{
    double Lx;
    uint16_t PWM_OUT;

    switch (mode)
    {
    case 0:
      areaLDR0->update();
      Lx = areaLDR0->getLuminance();
      PWM_OUT = areaPID->Compute(reference, Lx);
      areaLED0->analogWrite(PWM_OUT);
      break;

    case 1:
      areaLDR0->update();
      areaLDR1->update();
      Lx = (areaLDR0->getLuminance() + areaLDR1->getLuminance())/2;
      PWM_OUT = areaPID->Compute(reference, Lx);
      areaLED0->analogWrite(PWM_OUT);
      areaLED1->analogWrite(PWM_OUT);
      break;

    case 2:
      areaLDR0->update();
      areaLDR1->update();
      areaLDR2->update();
      areaLDR3->update();
      Lx = (areaLDR0->getLuminance() + areaLDR1->getLuminance() + areaLDR2->getLuminance() + areaLDR3->getLuminance())/4;
      PWM_OUT = areaPID->Compute(reference, Lx);
      areaLED0->analogWrite(PWM_OUT);
      areaLED1->analogWrite(PWM_OUT);
      areaLED2->analogWrite(PWM_OUT);
      areaLED3->analogWrite(PWM_OUT);
      break;

    default:
      break;
    }
  }
}

AreaInfoBundle_t Area::getInformation() const
{
  AreaInfo_t tmp;
  AreaInfoBundle_t tmpB;
  
  switch (mode)
  {
  case 2:
    tmp.Lx = areaLDR2->getLuminance();
    tmp.dutyCycle = areaLED2->getDutyCycle();
    tmpB.a2 = tmp;

    tmp.Lx = areaLDR3->getLuminance();
    tmp.dutyCycle = areaLED3->getDutyCycle();
    tmpB.a3 = tmp;
  
  case 1:
    tmp.Lx = areaLDR1->getLuminance();
    tmp.dutyCycle = areaLED1->getDutyCycle();
    tmpB.a1 = tmp;

  case 0:
    tmp.Lx = areaLDR0->getLuminance();
    tmp.dutyCycle = areaLED0->getDutyCycle();
    tmpB.a0 = tmp;
    break;
  
  default:
    break;
  }

  return tmpB;
}
