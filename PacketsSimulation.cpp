/*****************************************
*
*	PACKETS MANAGING SIMULATION IN C/C++
*
*		Jiri Sikora ~ 10.10.2021
*
*****************************************/

#define uint8_t unsigned char

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ENUMS

enum OP_CODE 
{
	OP_NULL,
	OP_VAL,
	OP_VOLTAGE_READ,
	OP_ID_GET,
};

enum COMPUTE_ERROR
{
	CE_NONE,
	CE_FEW_ARGS,
	CE_MANY_ARGS,
};

// STRUCTURES

struct PACKET
{
	uint8_t val;
	OP_CODE opc;
};

/*****************************************
*
*		PACKETS BUFFERING & READING
*
*****************************************/

// Packets arguments buffer for temporarily storage
#define ARGS_BUFFER_SIZE 64
int argsBuff[ARGS_BUFFER_SIZE];
int argsBuffIndex = 0;

// Functions to manage packets computing, buffering and reading
void clearArgsBuff();
void insertArgToBuff(uint8_t val);
COMPUTE_ERROR validateSize(int targetSize);
void printCErrorMessage(COMPUTE_ERROR ce);
uint8_t argsBuffGetByte();
int argsBuffGetInt();
float argsBuffGetFloat();
void computePacket(PACKET p);

// Fully written functions
void clearArgsBuff()
{
	argsBuffIndex = 0;
}

void insertArgToBuff(uint8_t val)
{
	if (argsBuffIndex >= ARGS_BUFFER_SIZE)
	{
		printf("[ERROR]: Arguments buffer overflow!\n");
		return;
	}
	
	argsBuff[argsBuffIndex] = val;
	argsBuffIndex++;
}

COMPUTE_ERROR validateSize(int targetSize)
{
	COMPUTE_ERROR ce = CE_NONE;
	
	if (argsBuffIndex > targetSize)
	{
		ce = CE_MANY_ARGS;
	} 
	else if (argsBuffIndex < targetSize)
	{
		ce = CE_FEW_ARGS;
	}
	
	clearArgsBuff();
	
	printCErrorMessage(ce);
	
	return ce;
}

void printCErrorMessage(COMPUTE_ERROR ce)
{
	if (!ce) return;
	
	argsBuffIndex = 0;
	
	switch (ce)
	{
		case CE_MANY_ARGS: printf("[ERROR]: Too many arguments!\n"); break;
		case CE_FEW_ARGS: printf("[ERROR]: Too few arguments!\n"); break;
	}
}

uint8_t argsBuffGetByte()
{
	if (argsBuffIndex >= ARGS_BUFFER_SIZE)
	{
		printf("[ERROR]: Getting byte index out of buffer size!\n");
		return 0;
	}
	
	uint8_t b = argsBuff[argsBuffIndex];
	argsBuffIndex++;
	
	return b;
}

int argsBuffGetInt()
{
	uint8_t intBuff [4];
	
	for (int i = 0; i < 4; i++)
	{
		intBuff[i] = argsBuffGetByte();
	}
	
	int finalInt;
	memcpy(&finalInt, intBuff, sizeof(finalInt));
	
	return finalInt;
}

float argsBuffGetFloat()
{
	uint8_t floatBuff [4];
	
	for (int i = 0; i < 4; i++)
	{
		floatBuff[i] = argsBuffGetByte();
	}
	
	float finalFloat;
	memcpy(&finalFloat, floatBuff, sizeof(finalFloat));
	
	return finalFloat;
}

void computePacket(PACKET p)
{
	switch (p.opc)
	{
		case OP_VAL:
			
			insertArgToBuff(p.val);
			
			break;
		
		case OP_VOLTAGE_READ:
			
			if (validateSize(sizeof(float))) break;
			
			printf("[OUT]: Voltage read is: %.2f\n", argsBuffGetFloat());
			
			break;
			
		case OP_ID_GET:
			
			if (validateSize(sizeof(int))) break;
			
			printf("[OUT]: ID get is: %d\n", argsBuffGetInt());
			
			break;
	}
	
	if (p.opc != OP_VAL) clearArgsBuff();
}

/*****************************************
*
*		PACKETS BUILDING & SENDING
*
*****************************************/

// Packets buffer array variables
#define PACKETS_BUFF_SIZE 1024
PACKET packetsBuff [PACKETS_BUFF_SIZE];
int packetsBuffIndex = 0;

// Functions to manage packet buffer array
void clearPackets();
void insertPacket(OP_CODE opc);
void insertPacket(OP_CODE opc, uint8_t val);
void insertFloatPackets(float floatVal);
void insertIntPackets(int intVal);
bool sendPackets();

// Fully written functions
void clearPackets()
{
	//printf("========== Clering packets buffer ==========\n");
	packetsBuffIndex = 0;
}

void insertPacket(OP_CODE opc)
{
	insertPacket(opc, 0);
}

void insertPacket(OP_CODE opc, uint8_t val)
{
	if (packetsBuffIndex >= PACKETS_BUFF_SIZE)
	{
		printf("[ERROR]: Packets buffer overflow!\n");
		return;
	}
	
	packetsBuff[packetsBuffIndex].opc = opc;
	packetsBuff[packetsBuffIndex].val = val;
	packetsBuffIndex++;
}

void insertFloatPackets(float floatVal)
{
	uint8_t floatBuff [sizeof(float)];
	
	memcpy(floatBuff, &floatVal, sizeof(float));
	
	int i;
	for (i = 0; i < sizeof(float); i++)
	{
		insertPacket(OP_VAL, floatBuff[i]);
	}
}

void insertIntPackets(int intVal)
{
	uint8_t intBuff [sizeof(int)];
	
	memcpy(intBuff, &intVal, sizeof(int));
	
	int i;
	for (i = 0; i < sizeof(int); i++)
	{
		insertPacket(OP_VAL, intBuff[i]);
	}
}

bool sendPackets()
{
	printf("========== Sending %d packets ==========\n", packetsBuffIndex);
	int i;
	for (i = 0; i < packetsBuffIndex; i++)
	{
		printf(" pack.%d ~ opc: %d val: %d\n", i, packetsBuff[i].opc, packetsBuff[i].val);
		computePacket(packetsBuff[i]);
	}
	//printf("========== Successfully sended! ==========\n");
	
	// clear the buffer of packets
	clearPackets();
	
	return 1;
}


/*****************************************
*
*		MAIN PROGRAM FUNCTIOS TEST
*
*****************************************/

int main()
{
	// insert voltage read
	insertFloatPackets(2.45);
	insertPacket(OP_VOLTAGE_READ);
	
	// again, another test
	insertFloatPackets(24.53);
	insertPacket(OP_VOLTAGE_READ);
	
	// send full buffer of packets
	sendPackets();
	
	// insert id
	insertIntPackets(689);
	insertPacket(OP_ID_GET);
	
	// send full buffer of packets
	sendPackets();
}
