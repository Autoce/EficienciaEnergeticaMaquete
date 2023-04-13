#ifndef AREA_PROJ_INCLUDED
#define AREA_PROJ_INCLUDED
#include "LDR.hpp"
#include "LED.hpp"
#include "PID.hpp"

typedef struct
{
  double Lx = 0;
  double dutyCycle = 0;
} AreaInfo_t;

class Area
{
  public:

    Area(uint8_t inputPin, uint8_t outputPin, uint8_t channel, uint8_t samples, const double* polynomial, double Kp, double Ki, double Kd, double N, double Ts, bool mode);
    ~Area();
    void update(double reference) const;
    AreaInfo_t getInformation() const;

  private:
    bool operationMode;
    LED* areaLED;
    LDR* areaLDR;
    PID* areaPID;
  
};
#endif
