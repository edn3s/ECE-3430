
#define MAKEDEBOUNCETIME 10		//Definitions for debounce Times for the P1.3 button (ms)
#define RELEASEBOUCETIME 20
#define SWITCHES 1

typedef enum {
	DbExpectHigh, DbValidateHigh, DbExpectLow, DbValidateLow
} DbState;
typedef enum {
	Off, On
} SwitchStatus;
typedef struct {
	DbState ControlState;
	SwitchStatus CurrentSwitchReading;
	SwitchStatus CurrentValidState;
	SwitchStatus PreviousValidState;
	int PortNumber;
	int bit;
} SwitchDefine;

typedef struct {
	int ms;
	int hs;
	int ds;
} Timer;


DbState Debouncer(SwitchDefine *Switch, Timer *t);
SwitchStatus GetSwitch(SwitchDefine *Switch);
void InitializeVariables(void);
void InitializeHardware(void);
void InitTimerSystem(); //This should set up a periodic interrupt at a 1 mS rate using SMCLK as the clock source.
void InitPorts(SwitchDefine *Switch);
void ManageSoftwareTimers(unsigned int g1ms);
void CaptureTime(Timer *t);
int CompareTime(Timer *t);


unsigned int g1mSCounter;
unsigned int g500Counter;
unsigned int g10sCounter;
unsigned int gButtonPresses;
int i;

