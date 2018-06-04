#include <Arduino.h>
class OseppTBMotor
{
  public:
    OseppTBMotor(uint8_t dir_pin, uint8_t pwm_pin);
    void forward(uint8_t speed);
    void backward(uint8_t speed);
    void SetSpeed(int speed);
  private:
    uint8_t _dir_pin, _pwm_pin;
};

class PID
{
  private:
    float _p;
    float _i;
    float _d;
    float _ilimit;
    float last_err;
  public:
    float sum_err;
    PID(float p, float i, float d, float i_limit);
    float pid(float error);
    void reset();
};


