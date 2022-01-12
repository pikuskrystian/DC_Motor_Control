#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S69_cm33_core0.h"
#include "fsl_debug_console.h"
#include "lcd.h"
#include "usb_device_interface_0_cic_vcom.h"

//--------------------------------------- macro ----------------------------------------------------------
//color macro
#define rgb(r, g, b) ((uint16_t)((r >> 3) << 11 ) | ((g >> 2) << 5) | (b >> 3))

//pwm macro
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3

#define offset 100

//interwa dla ctimer2 pwm mode
volatile uint8_t divInterval = 0, debugAddres = ' ';
volatile float pos = 0, left = 0, right = 0, speed = 0, lastPos=0, divPos=0, debugVal = 0;
volatile bool sr = false, sl = false, status = false;

//--------------------------------------- function ----------------------------------------------------------

//ustawienie pozycji (mostek h)
void Motor_SetPower(int8_t power)
{
	if(power > 100)
		power = 100;
	if(power < -100)
		power = -100;

	if(power>0) {
		GPIO_PinWrite(BOARD_INITPINS_AIN1_GPIO, BOARD_INITPINS_AIN1_PORT, BOARD_INITPINS_AIN1_PIN, 0);
		GPIO_PinWrite(BOARD_INITPINS_AIN2_GPIO, BOARD_INITPINS_AIN2_PORT, BOARD_INITPINS_AIN2_PIN, 1);
	}else if(power<0) {
		GPIO_PinWrite(BOARD_INITPINS_AIN1_GPIO, BOARD_INITPINS_AIN1_PORT, BOARD_INITPINS_AIN1_PIN, 1);
		GPIO_PinWrite(BOARD_INITPINS_AIN2_GPIO, BOARD_INITPINS_AIN2_PORT, BOARD_INITPINS_AIN2_PIN, 0);
	}else {
		//soft stop
		GPIO_PinWrite(BOARD_INITPINS_AIN1_GPIO, BOARD_INITPINS_AIN1_PORT, BOARD_INITPINS_AIN1_PIN, 0);
		GPIO_PinWrite(BOARD_INITPINS_AIN2_GPIO, BOARD_INITPINS_AIN2_PORT, BOARD_INITPINS_AIN2_PIN, 0);
	}

	CTIMER_UpdatePwmDutycycle(CTIMER2_PERIPHERAL, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER2_PWM_2_CHANNEL, abs(power));
}

void hardStop()
{
	CTIMER_UpdatePwmDutycycle(CTIMER2_PERIPHERAL, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER2_PWM_2_CHANNEL, 0);
}

void enc_callback(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	if(GPIO_PinRead(BOARD_INITPINS_ENCB_GPIO, BOARD_INITPINS_ENCB_PORT, BOARD_INITPINS_ENCB_PIN)){
		pos--;
	}
	else{
		pos++;
	}

	//PRINTF("Position call \r\n");
}

void ls1_callback(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	hardStop();

	if(!sl){
		left = pos;
		left = left + offset;
		sl = true;
		USB_write('L', left);
	}

	PRINTF("pr1 \r\n");

}

void ls2_callback(pint_pin_int_t pintr ,uint32_t pmatch_status)
{
	hardStop();

	if(!sr){
		right = pos;
		right = right - offset;
		sr = true;
		USB_write('R', right);
	}

	PRINTF("pr2 \r\n");

}

void ct2_callback(uint32_t flags)
{
	//interwal czasowy
	divInterval++;
	if(divInterval>=100){
		divInterval=0;

		divPos=(pos-lastPos);
		lastPos=pos;
		speed=divPos*1.25;
	}

	// 1/(1kh) interupt timer
}

void setUp(uint8_t speed)
{
	//stop motor
	Motor_SetPower(0);

	//wait
	for(volatile int i=0; i<10000000; i++);

	//set right
	sr = false;

	while(!sr){
		Motor_SetPower(-speed);
	}

	PRINTF("right \r\n");

	//set left
	for(volatile int i=0; i<10000000; i++);

	sl = false;

	while(!sl){
		Motor_SetPower(+speed);
	}

	PRINTF("left \r\n");


	//back to zero (right)
	for(volatile int i=0; i<10000000; i++);

	while(pos != right)
	{
		if(pos > right)
			Motor_SetPower(-speed);
		else
			Motor_SetPower(+speed);

		if(pos > left || pos < right){
			PRINTF("SetUp loop error \r\n");
			hardStop();
			break;
		}
	}

	Motor_SetPower(0);
	USB_write('P', pos);

	if(sr && sl){
		USB_write('O', 79);
		status = true;
	}

	PRINTF("Left: %d, Pos: %d, Right: %d \r\n", left, pos, right);
}

void setPosition(int32_t set, uint8_t speed)
{
	Motor_SetPower(0);

	if(set> left){
		set = left;
	}

	if(pos < right){
		set = right;
	}

	while(set != pos){

		if(set > pos)
			Motor_SetPower(+speed);
		else
			Motor_SetPower(-speed);

		if(pos > left || pos < right){
			PRINTF("setPosition loop error \r\n");
			hardStop();
			break;
		}
	}

	Motor_SetPower(0);
}

void interprete(uint8_t addres, float val){

	switch(addres)
	{

	case 'Z':
	{
		//value does't metter
		setUp(25);
		break;
	}

	case 'P':
	{
		setPosition(val, 25);
		break;
	}

	default:
	{
		PRINTF("Unsuporteted data %d # %f \r\n", addres, val);
		break;
	}
	}
}

int main(void) {
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	//initial
	LCD_Init(FLEXCOMM8_PERIPHERAL);
	EnableIRQ(CTIMER2_TIMER_IRQN);
	Motor_SetPower(0);

	char sbuff[30] = {};
	while(1) {


		uint8_t buffer[5] = {};

		float val = 0;

		USB_read(buffer, 5);

		if(buffer[0] != 0)
		{
			val = 0;
			memcpy(&val, buffer+1, 4);

			debugAddres = buffer[0];
			debugVal = val;


			interprete(buffer[0], val);

			LCD_Clear(rgb(255,255,255));
		}

		LCD_Clear(rgb(0,0,0));

		sprintf(sbuff, "ct2 speed: %f", speed);
		LCD_Puts(10, 10, sbuff, rgb(255,255,255));

		sprintf(sbuff, "Debug: %c, %f", debugAddres, debugVal);
		LCD_Puts(10, 20, sbuff, rgb(255,255,255));

		sprintf(sbuff, "Position: %f", pos);
		LCD_Puts(10, 30, sbuff, rgb(255,255,255));

		sprintf(sbuff, "Left: %f", left);
		LCD_Puts(10, 40, sbuff, rgb(255,255,255));

		sprintf(sbuff, "Right: %f", right);
		LCD_Puts(10, 50, sbuff, rgb(255,255,255));;

		LCD_GramRefresh();
	}

	return 0 ;
}
