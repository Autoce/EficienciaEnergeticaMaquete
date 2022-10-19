#ifndef LED_PROJ_INCLUDED
#define LED_PROJ_INCLUDED
#include <cstdint>
class LED
{
    public:

      LED(uint8_t outputPin, uint8_t channel);
      void analogWrite(uint16_t duty_cycle);
      uint16_t getDutyCycle();

    private:

      uint8_t channel;
      uint8_t outputPin;
      uint16_t duty_cycle;

};

#endif
