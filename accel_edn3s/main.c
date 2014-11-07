#include <msp430.h>
#include "CORDIC.h"
#include "USEFUL.h"
#include "DEBOUNCE.h"

#define B1  B8_3(P1OUT)
#define greenLED B8_6(P1OUT)            // Green LED macro
#define redLED B8_0(P1OUT)                // Red LED macro

volatile unsigned int g1mSTimeout;
volatile unsigned int readings[3];
volatile coordinates tare;
volatile coordinates input;

typedef enum {
	yes, no
} flagState;

typedef struct {
	flagState state;
} flag;

volatile flag confirm;

/*
 unsigned int totangle = 0;
 Quadrant q;
 calculations calc;
 measurements mes;*/

SwitchDefine gPushButton;    //Structure to contain all info for the P1.3 button
Timer gTimer;
SwitchDefine* switches[SWITCHES];
Timer* timers[SWITCHES];

void init_ADC() {
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	ADC10CTL1 = INCH_2 + CONSEQ_3;            // A2/A1/A0, repeat multi channel
	ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;
	ADC10AE0 = (BIT0 + BIT1 + BIT2);          // P1.0,1, 2 Analog enable
	ADC10DTC1 = 0x20;
}

void fetch_ADC() {
	ADC10CTL0 &= ~ENC;
	while (ADC10CTL1 & BUSY)
		;               // Wait if ADC10 core is active
	ADC10SA = (int) &(readings);        // Data buffer start
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
	__bis_SR_register(GIE);      // LPM0, ADC10_ISR will force exit
	__delay_cycles(2);
}

int fetch_Switch() {
	ManageSoftwareTimers(g1mSTimeout);
	g1mSTimeout = 0;
	Debouncer(switches[0], timers[0]);
	if(switches[0]->CurrentValidState == On){
		return 1;
	}else{
		return 0;
	}
}
void init_Timer(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	TACCR0 = 16000; // Upper limit of count for TAR
	TACCTL0 = CCIE; //capture and compare
	TACTL = TASSEL_2 | ID_0 | TACLR | MC_1; //use CMCLK divided by 8, only conting up
	DCOCTL = CALDCO_16MHZ;	//set frequency to 1mhz
	BCSCTL1 = CALBC1_16MHZ;

}
void InitializeVariables(void) {

//	P1DIR |= redLED + greenLED;
	P1DIR |= BIT6;
	gButtonPresses = 0;
	gPushButton.ControlState = DbExpectHigh;
	gPushButton.PortNumber = 1;
	gPushButton.bit = BIT3;
	switches[0] = &gPushButton;
	timers[0] = &gTimer;
}

int main(void) {

	init_ADC();
	init_Timer();
	InitializeVariables();
	InitPorts(switches[0]);
	_BIS_SR(GIE);
	/*
	 coordinates c;
	 c.x = 20;
	 c.y = 50;
	 c.z = 0;

	 q = getQuad(c.x, c.y);  //returns 1 2 3 or 4

	 calc = findQuadrantAngle(c.x, c.y);

	 totangle = findTotalAngle(calc.angle,q);

	 mes = getDisplaySetting(&c);
	 */

	for (;;) {
		confirm.state = no;
		fetch_ADC(); //current values are in readings
		if (fetch_Switch()) {
			P1OUT ^= BIT6;
			confirm.state = yes; //approve state changes
		}



	}
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
	//__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA0_routine(void) {
	g1mSTimeout++;
}
