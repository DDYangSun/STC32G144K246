#ifndef __HARDWARECONFIG_H__
#define __HARDWARECONFIG_H__

void pit_TIM0(void);

extern uint16 FPS;
extern uint8 FPS_Flag;
extern uint8 TIM0_ShowTFT;

void Hardware_Init(void);
void TFT_ShowFPS(void);
void TFT_ShowData(uint8 TIM0_ShowTFT);

#endif