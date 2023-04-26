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

typedef struct
{
  AreaInfo_t a0;
  AreaInfo_t a1;
  AreaInfo_t a2;
  AreaInfo_t a3;
} AreaInfoBundle_t;


class Area
{
  public:

    Area(uint8_t inputPin, uint8_t outputPin, uint8_t channel, uint8_t samples, const double* polynomial, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite);

    Area(uint8_t inputPin0, uint8_t inputPin1, uint8_t outputPin0, uint8_t outputPin1, uint8_t channel0, uint8_t channel1, uint8_t samples, const double* polynomial0, const double* polynomial1, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite);

    Area(uint8_t inputPin0, uint8_t inputPin1, uint8_t inputPin2, uint8_t inputPin3, uint8_t outputPin0, uint8_t outputPin1, uint8_t outputPin2, uint8_t outputPin3, uint8_t channel0, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t samples, const double* polynomial0, const double* polynomial1, const double* polynomial2, const double* polynomial3, double Kp, double Ki, double Kd, double N, double Ts, bool OverWrite);

    ~Area();
    void update(double reference) const;
    AreaInfoBundle_t getInformation() const;

  private:
    bool pwrOverWrite;
    size_t mode;
    LED *areaLED, *areaLED0, *areaLED1, *areaLED2, *areaLED3;
    LDR *areaLDR, *areaLDR0, *areaLDR1, *areaLDR2, *areaLDR3;
    PID* areaPID;
  
};
#endif
