#include <compiler_defs.h>
#include <C8051F020_defs.h> // Include SFR declarations

#define SEG P2
#define MOTOR P1

unsigned int lookUpTable[4]={63,6,91,79};
int currFloor =0;
int i =0;
bit elevatorOverload = 0;
bit sensor1Crossed = 0;
bit sensor2Crossed = 0;
int peopleCount = 0;

unsigned int floors[4]={0x00};

void motor_delay(){
int i = 0;
int j = 0;
for (i=0;i<1000;++i){
	for(j=0;j<100;++j){
	
	}
}
}

void door_delay(){
int i = 0;
int j = 0;
for (i=0;i<1000;++i){
	for(j=0;j<600;++j){
	
	}
}
}


void ascend(){
MOTOR = 0x01;
motor_delay();
MOTOR = 0x00;
}

void descend(){
MOTOR = 0x02;
motor_delay();
MOTOR = 0x00;
}


void init_intClock(void)
{
	OSCXCN = 0x00;
	// Bits 6-4 is 000 to disable it , else > don't care
	OSCICN = 0x14; // should be put as 0x14 to make it work	 but originally is it 0x04
	while (!(OSCICN & 0x10)); // Wait till IFRDY ( bit no 4 ) pin is set	, not working ,why if ^4 is 0
	/* Missing Clock Enable Bit(7) is 0 ( disabled) 
	  bits [ 1 : 0] are 10 for 2MHz
	  bit (2) is 1 to enable internal clock
	  bit (3) is 0 to disable external clock
	  */
}


void ext_int(void) interrupt 0
{
floors[1]=0xff;
} 


void ext_int3(void) interrupt 3
{
floors[2]=0xff;
} 

void ext_int1(void) interrupt 1
{
floors[0]=0xff;
}


void ext_int2(void) interrupt 2
{
floors[3]=0xff;
}


void timer2_interrupt(void) interrupt 5
{


	if(sensor1Crossed){
		peopleCount--;
		sensor1Crossed = 0;
	}
	else {
		sensor2Crossed = 1;
		
	}

if (peopleCount <= 4 ) {
	elevatorOverload = 0;
}
if ( peopleCount < 0 ) {
peopleCount = 0;
}
TF2=0; // flag
}


void timer4_interrupt(void) interrupt 16
{



    
	if(sensor2Crossed){
		peopleCount++;
		sensor2Crossed = 0;
	}
	else {
		sensor1Crossed = 1;
		
	}

if (peopleCount > 4 ) {
	elevatorOverload = 1;
}
T4CON = 0x06; //flag
}





void main (void)
{
EA = 0; 
WDTCN = 0xde;
WDTCN = 0xad;
init_intClock();
EA = 1;
EX0=1; 
EX1 = 1;

//Timer 0/1 code
ET0=1;
ET1 = 1;
TMOD = 0x66;
TR0=1;
TR1=1;
TL0 = 0xff;
TH0 = 0xff;
TL1 = 0xff;
TH1 = 0xff;

IT0 = 1; // external interrupt type, 0: level transition, 1: edge transition
IT1 = 1;

//timer 2 code
T2CON = 0x02;
RCAP2 = 0xffff;
TL2 = 0xff;
TH2 = 0xff;
T2CON=0x06;
ET2=1;

//timer 4 code
T4CON = 0x02;
RCAP4 = 0xffff;
TL4 = 0xff;
TH4 = 0xff;
T4CON=0x06;
EIE2 = 0x04;


XBR2 = 0x48;
XBR1 = 0x3e;
XBR0 = 0x04;
P0 = 0xff;

P1MDOUT = 0xff;

P2MDOUT = 0xff;
SEG = lookUpTable[0];

while(1){
i=0;

if (elevatorOverload) continue;

for(i=0;i<4;i++){
if(floors[i] && i>currFloor){
	while(currFloor != i ){
		ascend();
		currFloor++;
		SEG = lookUpTable[currFloor];
		if(floors[currFloor]){ door_delay(); floors[currFloor]=0;}
	}
}
}

i=3;

for(i=3;i>=0;i--){
if(floors[i] && i<currFloor){
	while(currFloor != i ){
		descend();
		currFloor--;
		SEG = lookUpTable[currFloor];
		if(floors[currFloor]) { door_delay(); floors[currFloor]=0;}
	}
}
}


}
}

