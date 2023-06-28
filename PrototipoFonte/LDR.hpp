#ifndef LDR_PROJ_INCLUDED
#define LDR_PROJ_INCLUDED
#include <cstdint>
#include <queue>

class LDR
{
  public:

    LDR(uint8_t inputPin, uint8_t samples, const double* polynomial, const double* reference);
    void update();
    double getLuminance() const;

  private:

    const double *polynomial, *reference;


    double currentLuxAvg;
    std::deque<double> luxReadings;
    uint8_t inputPin, samples;
    double getLuminanceFromHardware() const;
};

#endif 
