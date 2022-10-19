#ifndef LDR_PROJ_INCLUDED
#define LDR_PROJ_INCLUDED
#include <cstdint>
#include <queue>
class LDR
{
  public:

    LDR(uint8_t inputPin, const double* polynomial);
    void update();
    double getLuminance();

  private:

    const double* polynomial;
    double currentLuxAvg;
    std::deque<double> luxReadings;
    uint8_t inputPin;
    double getLuminanceFromHardware();
};

#endif 
