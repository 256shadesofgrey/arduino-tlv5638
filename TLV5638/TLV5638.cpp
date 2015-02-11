#include "TLV5638.h"

/*
Input: uint16_t <value>
Sends the received value to the chip through SPI.
*/
void TLV5638::writeReg(uint16_t value) {
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE2);
	digitalWrite(CS_pin, LOW);
	delayMicroseconds(10);
	SPI.transfer(value >> 8);
	SPI.transfer(value & 0xFF);
	delayMicroseconds(10);
	digitalWrite(CS_pin, HIGH);
}

/*
Input: byte <CS pin>
Initiates SPI and sets the CS pin to HIGH.
*/
TLV5638::TLV5638(byte CS){
	SPI.begin();
	CS_pin = CS;
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
}

/*
Input: uint16_t <0-3>
Sets reference voltage of the DAC. Possible values:
0 - external
1 - 1024 mV
2 - 2048 mV
3 - external
*/
void TLV5638::setRefVoltage(uint16_t RefV){
	if(RefV == 0){
		V_RefX1(reg);
	} else if(RefV == 1){
		V_Ref1V(reg);
		Reference_Voltage = 1024.0;
	} else if(RefV == 2){
		V_Ref2V(reg);
		Reference_Voltage = 2048.0;
	} else if(RefV == 3){
		V_RefX2(reg);
	}
	
	W_Control(reg);
	writeReg(reg);
}

/*
Input: uint16_t <0|1>
Turns chip off or on. Possible values:
0 - off
1 - on
*/
void TLV5638::powerControl(uint16_t powerState){
	if(powerState == 0) Power_Off(reg);
	else if(powerState == 1) Power_On(reg);
	
	writeReg(reg);
}

/*
Input: uint16_t <0|1>
Switches between slow and fast mode. Possible values:
0 - slow
1 - fast
*/
void TLV5638::speedControl(uint16_t speed){
	if(speed == 0) S_Slow(reg);
	else if(speed == 1) S_Fast(reg);
	
	writeReg(reg);
}

/*
Input: uint16_t <0-2047>, uint16_t <0-2>
Writes the voltage level to the channel. Does not take into account
the reference voltage, so U = level * 1 mV for 2V reference, and
U = level * 0,5 mV for 1V reference.
Channels:
0 - Channel B (and buffer)
1 - Buffer
2 - Channel A (and channel B <- buffer)
*/
void TLV5638::setLevel(uint16_t level, uint16_t channel){
	//level &= 2047;
	if(level > 2047) return;
	
	if(channel == 0) DACB_Buf(reg);
	else if(channel == 1) W_Buf(reg);
	else if(channel == 2) DACA(reg);
	else return;
	
	for(int i = 0; i < 12; i++){
		cwi(reg, i);
	}
	
	reg |= level;
	
	writeReg(reg);
}

/*
Input: uint16_t <0-2047>, uint16_t <0|1>
A little user friendlier than setLevel.
Channels:
0 - Channel A
1 - Channel B
*/
void TLV5638::simpleSetLevel(uint16_t level, uint16_t channel){
	if(channel == 0){
		setLevel(level, 2);
	} else if(channel == 1){
		//setLevel(level, 1);
		setLevel(level, 0);
	}
}

/*
Input: float <0.0-5000.0>
Allows setting custom reference voltage (0-5000mV). Does nothing if it's out of range.
*/
void TLV5638::setReference_Voltage(float RefV){
	if(RefV <= 5000.0 && RefV >= 0.0){
		float Reference_Voltage;
		Reference_Voltage = RefV;
	}
}

/*
Input: float <target voltage>
Calculates the level closest to the target voltage based on the reference voltage.
Return: uint16_t <level> if the calculated level is acceptable (0 <= level <= 2047), 65535 otherwise.
*/
uint16_t TLV5638::getVoltage(float Voltage){
	uint16_t level;
	
	level = (uint16_t)(2047*Voltage/Reference_Voltage);
	
	if(level <= 2047){
		return level;
	} else{
		return 65535;
	}
}

/*
Input: float <voltage>, uint16_t <0|1>
If the target voltage is within range, sets the output of the channel to the corresponding level.
Channels:
0 - Channel A
1 - Channel B
*/
void TLV5638::setVoltage(float Voltage, uint16_t channel){
	uint16_t level;
	level = getVoltage(Voltage);
	if(level <= 2047){
		simpleSetLevel(level, channel);
	}
}

