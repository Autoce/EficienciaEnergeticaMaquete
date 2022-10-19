#ifndef PID_PROJ_INCLUDED
#define PID_PROJ_INCLUDED

class PID
{
  public:

    PID(double Kp, double Ki, double Kd, double N, double Ts);
    double Compute(double reference, double sensor);

  private:

    double e2, e1, e0, u2, u1, u0;
    double Kp, Kd, Ki;
    double a0, a1, a2, b0, b1, b2, ku1, ku2, ke0, ke1, ke2;
};

#endif
