#include <Arduino.h>
#include "TBMotor.h"

OseppTBMotor::OseppTBMotor(uint8_t dir_pin, uint8_t pwm_pin)
{
  _dir_pin = dir_pin;
  _pwm_pin = pwm_pin;
  pinMode(_dir_pin, OUTPUT);
  digitalWrite(_pwm_pin, LOW);
}
void OseppTBMotor::forward(uint8_t speed)
{
  analogWrite( _pwm_pin, 0);
  digitalWrite(_dir_pin, HIGH);
  analogWrite( _pwm_pin, speed);
}
void OseppTBMotor::backward(uint8_t speed)
{
  analogWrite( _pwm_pin, 0);
  digitalWrite(_dir_pin, LOW);
  analogWrite( _pwm_pin, speed);
}
void OseppTBMotor::SetSpeed(int speed)
{
  analogWrite( _pwm_pin, 0);
  if (speed < 0)
  {
    speed = -speed;
    digitalWrite(_dir_pin, LOW);
  } else {
    digitalWrite(_dir_pin, HIGH);
  }
  analogWrite( _pwm_pin, speed);
}



PID::PID(float p, float i, float d, float i_limit)
{
  _p = p;
  _i = i;
  _d = d;
  _ilimit = abs(i_limit);
  last_err = 0.0;
  sum_err = 0.0;
}
float PID::pid(float error)
{
  sum_err += error;
  if(_ilimit!=0)
  {
    if (sum_err > _ilimit)sum_err = _ilimit; else if (sum_err < -_ilimit)sum_err = -_ilimit;
  }
  float dot_err = error - last_err;
  last_err = error;
  return error * _p + sum_err * _i + dot_err * _d;
}

void PID::reset()
{
  last_err = 0.0;
  sum_err = 0.0;
}




