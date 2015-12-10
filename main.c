
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "xspi.h"
#include "xgpio.h"
#include "lcd.h"
#include "xtmrctr.h"		// Timer Drivers
#include "xtmrctr_l.h" 		// Low-level timer drivers
#include "xintc.h"
#include "xil_exception.h"

#define	LED_CHANNEL	1
#define PWM_CHANNEL 1
#define SCALE_CHANNEL 1
#define AMP_CHANNEL	1
#define ULTRA0_CHANNEL 1
#define ULTRA1_CHANNEL 1
#define LED_DEVICE_ID		XPAR_LEDS_DEVICE_ID
#define PWM_DEVICE_ID		XPAR_TUNINGWORD_DEVICE_ID
#define SCALE_DEVICE_ID		XPAR_SCALE_DEVICE_ID
#define ULTRA0_DEVICE_ID	XPAR_ULTRA0_DEVICE_ID
#define ULTRA1_DEVICE_ID	XPAR_ULTRA1_DEVICE_ID
#define CLK_FREQ	100000000

static XGpio led;
static XGpio pwm;
static XGpio scale;
static XGpio dc;
static XGpio ultra0;
static XGpio ultra1;
static XSpi spi;
static XTmrCtr axiTimer;
static XIntc intc;

double sin_freq;
unsigned int pwm_freq;
unsigned char scaleVal;

void onLED(int num) {

	XGpio_DiscreteWrite(&led, LED_CHANNEL, num);
}

void offLED() {
    // Fill in this Function
    XGpio_DiscreteWrite(&led, LED_CHANNEL, 0x00);
}

static char tones[17][3]={"C6","B5","A5","G5","F5","E5","D5","C5","B4","A4","G4","F4","E4","D4","C4","B4","A4"};
static char freqs[17][7]={"1046.5", "987.77", "880.00", "783.99", "698.46", "659.26", "587.33", "523.25",
	"493.88", "440.00", "391.99", "349.23", "329.63", "293.66", "261.63", "246.94", "220.00"
};
unsigned int tuningWord[17] = {92051113, 86885168,
                        77405618, 68960489, 61437191, 57989122, 51662093,
                        46025556, 43442144, 38702809, 34480684, 30718595,
                        28994561, 25830606, 23013218, 21721072, 19351404};

unsigned char pitchnum = 0;
unsigned char volume = 0;
unsigned int distance0 = 0;
unsigned int distance1 = 0;
unsigned int accu0 = 0;
unsigned int accu1 = 0;
unsigned char count = 0;
unsigned int val = 0;

unsigned char R = 0;
unsigned char B = 0;


int tmp;
unsigned int tmp_distance0;
unsigned int tmp_distance1;

char *itoa(int value)
 {
     static char buffer[12];        // 12 bytes is big enough for an INT32
     int original = value;        // save original value

     int c = sizeof(buffer)-1;

     buffer[c] = 0;                // write trailing null in last byte of buffer

     if (value < 0)                 // if it's negative, note that and take the absolute value
         value = -value;

     do                             // write least significant digit of value that's left
     {
         buffer[--c] = (value % 10) + '0';
         value /= 10;
     } while (value);

     if (original < 0)
         buffer[--c] = '-';

     return &buffer[c];
 }
void print(char *str);


void TimerCounterHandler(void *CallbackRef) {
	XIntc_Disable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR);
    distance0 = XGpio_DiscreteRead(&ultra0, ULTRA0_CHANNEL);

    if(distance0 < 300)
    	distance0 = 300;
    tmp = (distance0 - 300) >> 5;

    if (tmp > 16)
        pitchnum = 16;
    else
        pitchnum = tmp;
    XGpio_DiscreteWrite(&pwm, PWM_CHANNEL, tuningWord[pitchnum]);
    //t = (unsigned int)(pow(2,32)*(1046 * (distance0-299)) / (double)pwm_freq);

    distance1 = XGpio_DiscreteRead(&ultra1, ULTRA1_CHANNEL);
    if (distance1 < 300)
    	distance1 = 300;
    if (distance1 - 300 > 511)
        volume = 0;
    else
        volume = (511 - (distance1 - 300))>>1;
    XGpio_DiscreteWrite(&scale, SCALE_CHANNEL, volume);
	val++;
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR);
}


int main()
{
	XStatus status;
    init_platform();


    XSpi_Config *spiConfig;

    u32 controlReg;

    status = XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);
    if (status != XST_SUCCESS)  {
    	xil_printf("Initialize GPIO dc fail!\n");
    	return 0;
    }

    // Set the direction for all signals to be outputs
    XGpio_SetDataDirection(&dc, 1, 0x0);

    //Initialize the SPI driver so that it is  ready to use.

    spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
    if (spiConfig == NULL) {
    	xil_printf("Can't find spi device!\n");
    	return 0;
    }

    status = XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress);
    if (status != XST_SUCCESS) {
    	xil_printf("Initialize spi failed!\n");
    	return 0;
    }

    // Reset the SPI device to leave it in a known good state.

    XSpi_Reset(&spi);

    // Setup the control register to enable master mode

    controlReg = XSpi_GetControlReg(&spi);
    XSpi_SetControlReg(&spi,
    		(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
    		(~XSP_CR_TRANS_INHIBIT_MASK));

    // Select 1st slave device
    XSpi_SetSlaveSelectReg(&spi, ~0x01);


    status = XGpio_Initialize(&pwm, PWM_DEVICE_ID);
    if (status != XST_SUCCESS){
    	xil_printf("PWM initialization failed\n\r");
    	return 0;
    }

    status = XGpio_Initialize(&scale, SCALE_DEVICE_ID);
    if (status != XST_SUCCESS){
    	xil_printf("Volume control initialization failed\n\r");
    	return 0;
    }

    status = XGpio_Initialize(&ultra0, ULTRA0_DEVICE_ID);
    if (status != XST_SUCCESS){
       	xil_printf("Ultrasonic sensor 0 initialization failed\n\r");
       	return 0;
    }

    status = XGpio_Initialize(&ultra1, ULTRA1_DEVICE_ID);
    if (status != XST_SUCCESS){
       	xil_printf("Ultrasonic sensor 1 initialization failed\n\r");
       	return 0;
    }

    status = XGpio_Initialize(&led, LED_DEVICE_ID);
    if (status != XST_SUCCESS){
       	xil_printf("LED initialization failed\n\r");
       	return 0;
    }

//onLED((1<<13)|distance0);



    initLCD();
    clrScr();
    setColor(180, 180, 180);
	fillRect(0, 0, 239, 319);
	setColor(255, 255, 255);
	fillRect(10, 10, 230, 310);


	setColor(56,56,240);
	setColorBg(255, 255, 255);


	xil_printf("lcd on\n\r");


    status = XGpio_Initialize(&pwm, PWM_DEVICE_ID);
    if (status != XST_SUCCESS){
    	xil_printf("pwm\n\r");

    }

    status = XGpio_Initialize(&scale, SCALE_DEVICE_ID);
    if (status != XST_SUCCESS){
    	xil_printf("scale_sd\n\r");

    }

    XGpio_Initialize(&ultra0, ULTRA0_DEVICE_ID);
    XGpio_Initialize(&ultra1, ULTRA1_DEVICE_ID);
    XGpio_Initialize(&led, LED_DEVICE_ID);
    //onLED((1<<13)|distance0);
    onLED(0x3FFF);

    status = XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_1_DEVICE_ID);
    if (status != XST_SUCCESS) {
    	xil_printf("Initialize timer fail!\n");
    	return XST_FAILURE;
    }

    status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
    if (status != XST_SUCCESS) {
    	xil_printf("Initialize interrupt controller fail!\n");
    	return XST_FAILURE;
    }

    status = XIntc_Connect(&intc,
    		XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR,
    		(XInterruptHandler)XTmrCtr_InterruptHandler,
    		(void *)&axiTimer);
    if (status != XST_SUCCESS) {
    	xil_printf("Connect IHR fail!\n");
    	return XST_FAILURE;
    }

    status = XIntc_Start(&intc, XIN_REAL_MODE);
    if (status != XST_SUCCESS) {
    	xil_printf("Start interrupt controller fail!\n");
    	return XST_FAILURE;
    }

    // Enable interrupt
   	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_1_INTERRUPT_INTR);


   	// Setup the handler for the timer counter that will be called from the
   	// interrupt context when the timer expires, specify a pointer to the
   	// timer counter driver instance as the callback reference so the handler
   	// is able to access the instance data

    XTmrCtr_SetHandler(&axiTimer, TimerCounterHandler, &axiTimer);

    // Enable the interrupt of the timer counter so interrupts will occur
    // and use auto reload mode such that the timer counter will reload
    // itself automatically and continue repeatedly, without this option
    // it would expire once only

    XTmrCtr_SetOptions(&axiTimer, 0,
    	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);


    // Set a reset value for the timer counter such that it will expire
    // eariler than letting it roll over from 0, the reset value is loaded
    // into the timer counter when it is started

    XTmrCtr_SetResetValue(&axiTimer, 0, 0xFFFF0000);

    XTmrCtr_Start(&axiTimer, 0);
    xil_printf("Timer start!!\n");

    microblaze_enable_interrupts();

    while(1){

        R = (distance0 - 300);
        if (R>255)
        	R=255;
        R = 255 - R;

        B = (distance1 - 300);
        if (B>255)
        	B=255;
        B = 255 - B;

        if(count == 128){

        	setColor(R,127,B);
        	setFont(BigFont);
        	lcdPrint(" Theremin", 40, 15);
        	setFont(SmallFont);
        	lcdPrint("ECE 253", 95, 35);
        	lcdPrint("Left : pitch  control", 40, 55);
        	lcdPrint("Right: volume control", 40, 70);
        	lcdPrint("Left Distance:", 40, 100);
        	lcdPrint("Right Distance:", 40, 140);
        	lcdPrint("Tone:", 40, 180);
        	lcdPrint("Frequency:", 40, 220);
        	lcdPrint("Volume:", 40, 260);


        	int disptmp = accu0 >> 7;

        	if (disptmp < 1000){
        		setColor(255, 255, 255);
        		fillRect(74, 120, 82, 130 );
        	}

        	setColor(R,127,B);
        	lcdPrint(itoa(disptmp), 50, 120);
        	setColor(R,127,B);
        	lcdPrint("mm", 85, 120);

        	disptmp = accu1 >> 7;

        	if (disptmp < 1000){
        		setColor(255, 255, 255);
        		fillRect(74, 160, 82, 170 );
        	}

        	setColor(R,127,B);
        	lcdPrint(itoa(disptmp), 50, 160);

        	setColor(R,127,B);
        	lcdPrint("mm", 85, 160);

        	disptmp = volume * 200 >> 9;
        	if (disptmp < 10){
        		setColor(255, 255, 255);
        		fillRect(57, 280, 63, 290 );
        		setColor(R,127,B);
        	}

        	lcdPrint(itoa(disptmp), 50, 280);

        	if (volume == 0){
        		lcdPrint("  ", 50, 200);
        		lcdPrint("                   ", 50, 240);
        	}
        	else{
        		lcdPrint("Hz", 100, 240);
        		lcdPrint(tones[pitchnum], 50, 200);
        		lcdPrint(freqs[pitchnum], 50, 240);
        	}

        	accu0 = 0;
        	accu1 = 0;
        	count = 0;
        	//onLED((1<<13)|val);
        	onLED((1<<13)|distance0);
        	//for (i=0;i <100000;i++);
        }
        else {
        	accu0 += distance0;
        	accu1 += distance1;
        	count++;
        }
    }

    return 0;
}
