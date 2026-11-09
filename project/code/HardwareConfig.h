#ifndef __HARDWARECONFIG_H__
#define __HARDWARECONFIG_H__

void pit_handler(void);

extern uint16 FPS;
extern uint8 FPS_Flag;

void Hardware_Init(void);

#endif