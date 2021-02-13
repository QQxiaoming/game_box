/*===================================================================*/
/*                                                                   */
/*  InfoNES_K6502.h : Header file for K6502                                  */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_K6502_H_INCLUDED
#define InfoNES_K6502_H_INCLUDED

#include <stdint.h>

/* 6502 Flags */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

/* Stack Address */
#define BASE_STACK 0x100

/* Interrupt Vectors */
#define VECTOR_NMI 0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ 0xfffe

// NMI Request
#define NMI_REQ NMI_State = 0;

// IRQ Request
#define IRQ_REQ IRQ_State = 0;

// Emulator Operation
void K6502_Init(void);
void K6502_Reset(void);
void K6502_Set_Int_Wiring(uint8_t byNMI_Wiring, uint8_t byIRQ_Wiring);
void K6502_Step(uint16_t wClocks);

// I/O Operation (User definition)
uint8_t K6502_Read(uint16_t wAddr);
uint16_t K6502_ReadW(uint16_t wAddr);
uint16_t K6502_ReadW2(uint16_t wAddr);
uint8_t K6502_ReadZp(uint8_t byAddr);
uint16_t K6502_ReadZpW(uint8_t byAddr);
uint8_t K6502_ReadAbsX(void);
uint8_t K6502_ReadAbsY(void);
uint8_t K6502_ReadIY(void);

void K6502_Write(uint16_t wAddr, uint8_t byData);
void K6502_WriteW(uint16_t wAddr, uint16_t wData);

// The state of the IRQ pin
extern uint8_t IRQ_State;

// The state of the NMI pin
extern uint8_t NMI_State;

// The number of the clocks that it passed
extern uint16_t g_wPassedClocks;

#endif /* !InfoNES_K6502_H_INCLUDED */
