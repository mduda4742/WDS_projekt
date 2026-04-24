#ifndef ODOMSTATE_HPP
#define ODOMSTATE_HPP

typedef struct {
    double x, y, yaw;
    double linear_vel, angular_vel;
} odomState;

#endif // ODOMSTATE_HPP
