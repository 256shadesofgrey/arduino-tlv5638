#ifndef __ARDUINO_TLV5638
#define __ARDUINO_TLV5638

#include <Arduino.h>
#include <SPI.h>

#define _BV(bit) (1 << (bit))
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define cwi(sfr, bit) (_SFR_WORD(sfr) &= ~_BV(bit))
#define swi(sfr, bit) (_SFR_WORD(sfr) |= _BV(bit))

#define R1 15
#define Fast_Mode 14
#define Power_Down 13
#define R0 12

#define REF1 1
#define REF0 0

//REGISTERED SELECT BITS
#define DACB_Buf(reg)  (cwi(reg, R1)|cwi(reg, R0))  //R1=0 R0=0 Write data to DAC B and BUFFER
#define W_Buf(reg)     (swi(reg, R0)|cwi(reg, R1))  //R1=0 R0=1 Write data to BUFFER
#define DACA(reg)      (swi(reg, R1)|cwi(reg, R0))  //R1=1 R0=0 Write data to DAC A and update DAC B with BUFFER content
#define W_Control(reg) (swi(reg, R0)|swi(reg, R1))  //R1=1 R0=1 Write data to control register

//DATA BITS: CONTROL
#define V_RefX1(reg) (cwi(reg, REF0)|cwi(reg, REF1)) //REF1=0 REF0=0 External
#define V_Ref1V(reg) (swi(reg, REF0)|cwi(reg, REF1)) //REF1=0 REF0=1 1.024V
#define V_Ref2V(reg) (swi(reg, REF1)|cwi(reg, REF0)) //REF1=1 REF0=0 2.048V
#define V_RefX2(reg) (swi(reg, REF0)|swi(reg, REF1)) //REF1=1 REF0=1 External

//POWER AND SPEED BITS
#define Power_On(reg) (cwi(reg, Power_Down))         //Power on.
#define Power_Off(reg) (swi(reg, Power_Down))        //Power off.
#define S_Fast(reg) (swi(reg, Fast_Mode))            //Fast mode.
#define S_Slow(reg) (cwi(reg, Fast_Mode))            //Slow mode.

class TLV5638 {
	public:
		TLV5638(byte CS);
		void setRefVoltage(uint16_t RefV);
		void powerControl(uint16_t powerState);
		void speedControl(uint16_t speed);
		void setLevel(uint16_t level, uint16_t channel);
		void simpleSetLevel(uint16_t level, uint16_t channel);
		void setReferenceVoltage(float RefV);
		uint16_t getVoltage(float Voltage);
		void setVoltage(float Voltage, uint16_t channel);
		void setReference_Voltage(float RefV);
	
	protected:
		void writeReg(uint16_t value);
		byte CS_pin;                                 //CS pin.
		uint16_t reg;                                //Last value sent to the chip.
		float Reference_Voltage;                     //Reference voltage (in mV).
};

#endif