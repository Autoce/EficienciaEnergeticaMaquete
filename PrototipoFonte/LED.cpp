#include "LED.hpp"
#include <Arduino.h>

LED::LED(uint8_t outputPin, uint8_t channel) : channel(channel), outputPin(outputPin)
{
  ledcSetup(channel, 10000, 10);
  ledcAttachPin(outputPin, channel);
  ledcWrite(channel, 0);
}
void LED::analogWrite(uint16_t duty_cycle)
{
  ledcWrite(channel, duty_cycle);
  this->duty_cycle = duty_cycle;
}
uint16_t LED::getDutyCycle()
{
  return duty_cycle/10.23;
}
