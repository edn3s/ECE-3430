
#include <msp430.h>
#include "DEBOUNCE.h"


void CaptureTime(Timer *t) {

	t->ms = g1mSCounter;
	t->hs = g500Counter;
	t->ds = g10sCounter;
}

int CompareTime(Timer *t) { //returns the answer to the question has 10ms elpsed

	if (g10sCounter >= t->ds || g500Counter >= t->hs
			|| g1mSCounter >= ((t->ms) + 10)) { //start from big to small, if they are both in the same range
		return 1;
	} else {
		return 0;
	}
}

//Function that debounces

SwitchStatus GetSwitch(SwitchDefine *Switch) {
 	if (Switch->PortNumber) { //if portnumber is 1
		if ((P1IN & (Switch->bit)) == 0) { //if the bit at p1n is 1 Switch->bit
			Switch->CurrentSwitchReading = On;
		} else {
			Switch->CurrentSwitchReading = Off;
		}
	}/*
	 if (Switch->PortNumber == 2) {
	 //if portnumber is 1
	 if (P1IN & (Switch->bit)) { //if the bit at p1n is 1
	 Switch->CurrentSwitchReading = On;
	 } else {
	 Switch->CurrentSwitchReading = Off;
	 }
	 }*/
	return Switch->CurrentSwitchReading;
}


DbState Debouncer(SwitchDefine *Switch, Timer *t) {

	DbState MyState = Switch->ControlState;
	switch (MyState) {
	case DbExpectHigh:
		if (GetSwitch(Switch) == On) {
			CaptureTime(Switch);
			Switch->ControlState = DbValidateHigh;
		}
		Switch->PreviousValidState = Switch->CurrentValidState;
		Switch->CurrentValidState = Off;
		break;

	case DbValidateHigh:
		if (GetSwitch(Switch) == Off) {
			Switch->ControlState = DbExpectHigh;
		} else {
			if (CompareTime(t)) {
				//P1OUT ^= LED1;
				Switch->ControlState = DbExpectLow;
			}
		}
		Switch->PreviousValidState = Switch->CurrentValidState;
		Switch->CurrentValidState = Off;
		break;

	case DbExpectLow:
		if (GetSwitch(Switch) == Off) {
			CaptureTime(Switch);
			Switch->ControlState = DbValidateLow;
		}
		Switch->PreviousValidState = Switch->CurrentValidState;
		Switch->CurrentValidState = On;

		break;

	case DbValidateLow:
		if (GetSwitch(Switch) == On) {
			Switch->ControlState = DbExpectLow;
		} else {
			if (CompareTime(t)) {
			//	P1OUT ^= LED1;
				Switch->ControlState = DbExpectHigh;
			}
		}
		Switch->PreviousValidState = Switch->CurrentValidState;
		Switch->CurrentValidState = On;
		break;
	default:
		Switch->ControlState = DbExpectHigh;
		MyState = DbExpectHigh;
	}
	return MyState;
}



void InitPorts(SwitchDefine *Switch) {
	int b;

	g1mSCounter = 0;
	g500Counter = 0;
	g10sCounter = 0;


		if (Switch->PortNumber == 1) {
			P1REN |= Switch->bit;
			P1DIR &= ~(Switch->bit);
			P1OUT |= Switch->bit;
		}
		if (Switch->PortNumber == 2) {
			P1REN |= Switch->bit;
			P1DIR &= ~(Switch->bit);
			P1OUT |= Switch->bit;
		}

}

void ManageSoftwareTimers(unsigned int g1ms ) {

	if (g1ms > 0) {
		g1ms = 0;
		g1mSCounter++;
	}
	if (g1mSCounter >= 500) {
		g1mSCounter = 0;
		g500Counter++;
		//P1OUT ^= LED2;
	}
	if (g500Counter >= 20) {
		g500Counter = 0;
		g10sCounter++;
		//InitPorts();
	}
	if (g10sCounter >= 10)
		g10sCounter = 0;

}
