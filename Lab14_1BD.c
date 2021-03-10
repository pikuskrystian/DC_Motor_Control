

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S69_cm33_core0.h"
#include "fsl_debug_console.h"
#include "lcd.h"
#include "back.h"
#include "arm_math.h"
#define MAXSET 300
volatile int32_t Set=0, motorPower=0;
volatile int32_t Position=0, oldPosition=0, divPosition=0;
volatile uint8_t divInterval=0;
volatile bool Run=false;
uint16_t modeColor=0;
volatile float32_t Speed=0;
arm_pid_instance_f32 Coef;
volatile float32_t e=0, x=0;
void Motor_SetPower(int32_t power, bool brake);
void cbEncoder_Rotate(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	if(GPIO_PinRead(BOARD_INITPINS_ENC_A_GPIO, BOARD_INITPINS_ENC_A_PORT, BOARD_INITPINS_ENC_A_PIN)) {
		if(Set < MAXSET)
			Set++;
	}else{
		if(Set > -MAXSET)
			Set--;
	}
}
void cbEncoder_Push(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	Run=!Run;
}
void cbMotor_Rotate(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	if(GPIO_PinRead(BOARD_INITPINS_MOT_ENCA_GPIO, BOARD_INITPINS_MOT_ENCA_PORT, BOARD_INITPINS_MOT_ENCA_PIN))
		Position++;
	else
		Position--;
}
void cbMotor_PWM(uint32_t flags)
{
	divInterval++;
	if(divInterval>=100){
		divInterval=0;
		divPosition=(Position-oldPosition);
		oldPosition=Position;
		Speed=divPosition*1;
		x = Run ? Set : 0;
		e=(x-Speed);
		motorPower = arm_pid_f32(&Coef, e);
		Motor_SetPower(motorPower, 1);
	}
}
void Motor_SetPower(int32_t power, bool brake)
{
	if(power > 100)
		power = 100;
	if(power < -100)
		power = -100;
	if(power>0) {
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRA_GPIO, BOARD_INITPINS_MOT_DIRA_PORT, BOARD_INITPINS_MOT_DIRA_PIN, 0);
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRB_GPIO, BOARD_INITPINS_MOT_DIRB_PORT, BOARD_INITPINS_MOT_DIRB_PIN, 1);
	}else if(power<0) {
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRA_GPIO, BOARD_INITPINS_MOT_DIRA_PORT, BOARD_INITPINS_MOT_DIRA_PIN, 1);
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRB_GPIO, BOARD_INITPINS_MOT_DIRB_PORT, BOARD_INITPINS_MOT_DIRB_PIN, 0);
	}else {
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRA_GPIO, BOARD_INITPINS_MOT_DIRA_PORT, BOARD_INITPINS_MOT_DIRA_PIN, brake);
		GPIO_PinWrite(BOARD_INITPINS_MOT_DIRB_GPIO, BOARD_INITPINS_MOT_DIRB_PORT, BOARD_INITPINS_MOT_DIRB_PIN, brake);
	}
	CTIMER_UpdatePwmDutycycle(CTIMER2_PERIPHERAL, CTIMER2_PWM_1_CHANNEL, abs(power));
}
/*
 * @brief Application entry point.
 */
int main(void) {
	Coef.Kp=0.15;
	Coef.Kd=0.05;
	Coef.Ki=0.1;
	arm_pid_init_f32(&Coef, 1);
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	LCD_Init(FLEXCOMM3_PERIPHERAL);
	EnableIRQ(CTIMER2_TIMER_IRQN);
	while(1) {
		LCD_Set_Bitmap((uint16_t*)back_160x128);
		modeColor = Run ? 0x0FFF : 0xF800;
		LCD_Puts(10, 6, "Position:", 0x0FFF);
		LCD_7seg(34, 6, Position, 4, modeColor);
		LCD_Puts(134, 30, "", 0x0FFF);
		LCD_Puts(10, 48, "oldPosition", 0x0FFF);
		LCD_7seg(34, 48, oldPosition, 4, 0x0FFF);
		LCD_Puts(134, 72, "", 0x0FFF);
		LCD_Puts(10, 90, "Tacho:", 0x0FFF);
		LCD_7seg(34, 90, (int)Speed, 4, 0x0FFF);
		LCD_Puts(134,114, "RPM", 0x0FFF);
		LCD_GramRefresh();
	}
	return 0 ;
}
