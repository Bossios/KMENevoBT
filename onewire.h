#ifndef ONEWIRE_H
#define ONEWIRE_H


// ������
#define P_DS PA7
  
#include <util/delay.h>
#include "avrlibdefs.h"

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec
 
#define MAX_DEVICES 2 

/*
 =======================================================================================================================
	������� OW_RESET -> ���������� ���� 1-Wire � ���������� ������� ������������ ���������.
	������ ������ ������ ���� ������� ������� 480 ���.
 =======================================================================================================================
*/
unsigned char ow_reset(void)
{
	u08	pr, sreg;

	sreg = SREG;  cli();

	cbi(PORTA, P_DS);           
	sbi(DDRA, P_DS);

	_delay_us(480);
	cbi(DDRA, P_DS);
	_delay_us(70);

	pr = !(inb(PINA) & (1<<P_DS));
	SREG = sreg;
	_delay_us(410);
	return(pr);	
}						

/*
 =======================================================================================================================
    WRITE_BIT -> ������ ���� � ����. 
 =======================================================================================================================
*/
void write_bit(char bitval)
{
	u08 sreg;

	sreg= SREG; cli();
	cbi(PORTA, P_DS);               /* ������������� ������ ������� ����� ������ (DQ) */
	sbi(DDRA, P_DS);
	                        /* ����� 10us */
	_delay_us(10);
	
	if(bitval) cbi(DDRA, P_DS);/* ������������ ������� ������� ����� ������ (DQ),���� ������������ 1 */
	                        /* ���� ��������� ���������� ��������� (70us) */
	_delay_us(70);
	
	cbi(DDRA, P_DS);               /* ������������ ������� ������� ����� ������ (DQ) */
	SREG = sreg;
}

/*
 =======================================================================================================================
    WRITE_BYTE -> ������ ����� � ����.
 =======================================================================================================================
*/
void write_byte(char val)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned char	i;
	unsigned char	temp;
	/*~~~~~~~~~~~~~~~~~*/


	for(i = 0; i < 8; i++)	/* ������ �� ������ ���� */
	{
		temp = val >> i;	/* �������� ���� ������ �� �������� 'i' */
		temp &= 0x01;		/* �������� ������� ��� � temp */
		write_bit(temp);	/* ���������� ��� � ���� */
	}
	sbi(PORTA, P_DS);               
	sbi(DDRA, P_DS);

	                        /* ������� ��������� ���������� ��������� */
	_delay_us(120);
		
}

/*
 =======================================================================================================================
    READ_BIT -> ������ ���� �� ����.
 =======================================================================================================================
*/
unsigned char read_bit(void)
{
	u08 sreg, r;

	sreg = SREG; cli();

	cbi(PORTA, P_DS);
	sbi(DDRA, P_DS);
	nop();nop();nop();nop();nop();nop();
	cbi(DDRA, P_DS);   

	_delay_us(10);
	r = inb(PINA) & (1<<P_DS);
	SREG = sreg;
	return r;/* ���������� �������� ����� ������ (DQ) */
}

/*
 =======================================================================================================================
    READ_BYTE -> ������ ����� �� ����.
 =======================================================================================================================
*/
unsigned char read_byte(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	i;
	unsigned char	value = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < 8; i++)
	{
		if(read_bit()) value |= 0x01 << i;/* ������ ���� ��� � ���������� ��� � ������ i */
		_delay_us(120);
		
	}

	return(value);
}

/*
 =======================================================================================================================
    calc_crc -> ������� ����������� �����.
 =======================================================================================================================
*/
char calc_crc(unsigned char* buff, char num_vals)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char	shift_reg = 0, 
	        data_bit, 
	        sr_lsb, 
	        fb_bit, 
	        i, 
	        j;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < num_vals; i++)
	{
		for(j = 0; j < 8; j++)
		{
			data_bit = (*buff >> j) & 0x01;
			sr_lsb = shift_reg & 0x01;
			fb_bit = (data_bit ^ sr_lsb) & 0x01;
			shift_reg = shift_reg >> 1;
			if(fb_bit)
			{
				shift_reg = shift_reg ^ 0x8c;
			}
		}
	buff++;
	}

	return(shift_reg);
}

#endif
