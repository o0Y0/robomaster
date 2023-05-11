#ifndef __FIRETASK_H
#define __FIRETASK_H
#include "main.h"
#include "motor.h"

typedef enum
{
    _0M,
    _15M,
    _18M,
	_22M,
    _30M,
    BULLET_SPEED_NUM,
}BULLET_SPEED;

typedef struct
{
    uint8_t working;

    /*1×Ô¶¯ 0µãÉä*/
    uint8_t mode;

    uint8_t triggerBuf;

    BULLET_SPEED bulletSpeed;

    int16_t frictionWheelSpeed[BULLET_SPEED_NUM];

    DJI_MOTOR *fricL,*fricR,*trigger;
}GUN;
extern GUN gun;

void fire_ctrl(void);
#endif

