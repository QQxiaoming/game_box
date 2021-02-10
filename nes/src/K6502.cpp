/*===================================================================*/
/*                                                                   */
/*  K6502.cpp : 6502 Emulator                                        */
/*                                                                   */
/*  2000/5/10   InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "K6502.h"

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*-------------------------------------------------------------------*/
/*  Global valiables                                                 */
/*-------------------------------------------------------------------*/

// 6502 Register
uint16_t PC;
uint8_t SP;
uint8_t F;
uint8_t A;
uint8_t X;
uint8_t Y;

// The state of the IRQ pin
uint8_t IRQ_State;

// Wiring of the IRQ pin
uint8_t IRQ_Wiring;

// The state of the NMI pin
uint8_t NMI_State;

// Wiring of the NMI pin
uint8_t NMI_Wiring;

// The number of the clocks that it passed
uint16_t g_wPassedClocks;

// A table for the test
uint8_t g_byTestTable[256];

// Value and Flag Data
struct value_table_tag {
    uint8_t byValue;
    uint8_t byFlag;
};

// A table for ASL
struct value_table_tag g_ASLTable[256];

// A table for LSR
struct value_table_tag g_LSRTable[256];

// A table for ROL
struct value_table_tag g_ROLTable[2][256];

// A table for ROR
struct value_table_tag g_RORTable[2][256];

/*-------------------------------------------------------------------*/
/*  Operation Macros                                                 */
/*-------------------------------------------------------------------*/

// Clock Op.
#define CLK(a) g_wPassedClocks += (a);

// Addressing Op.
// Address
// (Indirect,X)
#define AA_IX K6502_ReadZpW(K6502_Read(PC++) + X)
// (Indirect),Y
#define AA_IY K6502_ReadZpW(K6502_Read(PC++)) + Y
// Zero Page
#define AA_ZP K6502_Read(PC++)
// Zero Page,X
#define AA_ZPX (uint8_t)(K6502_Read(PC++) + X)
// Zero Page,Y
#define AA_ZPY (uint8_t)(K6502_Read(PC++) + Y)
// Absolute
uint16_t AA_ABS_func(void)
{
    uint16_t temp0 = K6502_Read(PC++);
    uint16_t temp1 = (uint16_t)K6502_Read(PC++) << 8;
    return temp0 | temp1;
}
#define AA_ABS AA_ABS_func()
// Absolute2 ( PC-- )
uint16_t AA_ABS2_func(void)
{
    uint16_t temp0 = K6502_Read(PC++);
    uint16_t temp1 = (uint16_t)K6502_Read(PC) << 8;
    return temp0 | temp1;
}
#define AA_ABS2 AA_ABS2_func()
// Absolute,X
#define AA_ABSX AA_ABS + X
// Absolute,Y
#define AA_ABSY AA_ABS + Y

// Data
// (Indirect,X)
#define A_IX K6502_Read(AA_IX)
// (Indirect),Y
#define A_IY K6502_ReadIY()
// Zero Page
#define A_ZP K6502_ReadZp(AA_ZP)
// Zero Page,X
#define A_ZPX K6502_ReadZp(AA_ZPX)
// Zero Page,Y
#define A_ZPY K6502_ReadZp(AA_ZPY)
// Absolute
#define A_ABS K6502_Read(AA_ABS)
// Absolute,X
#define A_ABSX K6502_ReadAbsX()
// Absolute,Y
#define A_ABSY K6502_ReadAbsY()
// Immediate
#define A_IMM K6502_Read(PC++)

// Flag Op.
#define SETF(a) F |= (a)
#define RSTF(a) F &= ~(a)
#define TEST(a)            \
    RSTF(FLAG_N | FLAG_Z); \
    SETF(g_byTestTable[a])

// Load & Store Op.
#define STA(a) K6502_Write((a), A);
#define STX(a) K6502_Write((a), X);
#define STY(a) K6502_Write((a), Y);
#define LDA(a) \
    A = (a);   \
    TEST(A);
#define LDX(a) \
    X = (a);   \
    TEST(X);
#define LDY(a) \
    Y = (a);   \
    TEST(Y);

// Stack Op.
#define PUSH(a) K6502_Write(BASE_STACK + SP--, (a))
#define PUSHW(a)    \
    PUSH((a) >> 8); \
    PUSH((a)&0xff)
#define POP(a) a = K6502_Read(BASE_STACK + ++SP)
#define POPW(a) \
    POP(a);     \
    a |= (K6502_Read(BASE_STACK + ++SP) << 8)

// Logical Op.
#define ORA(a) \
    A |= (a);  \
    TEST(A)
#define AND(a) \
    A &= (a);  \
    TEST(A)
#define EOR(a) \
    A ^= (a);  \
    TEST(A)
#define BIT(a)                      \
    byD0 = (a);                     \
    RSTF(FLAG_N | FLAG_V | FLAG_Z); \
    SETF((byD0 & (FLAG_N | FLAG_V)) | ((byD0 & A) ? 0 : FLAG_Z));
#define CMP(a)                      \
    wD0 = (uint16_t)A - (a);            \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    SETF(g_byTestTable[wD0 & 0xff] | (wD0 < 0x100 ? FLAG_C : 0));
#define CPX(a)                      \
    wD0 = (uint16_t)X - (a);            \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    SETF(g_byTestTable[wD0 & 0xff] | (wD0 < 0x100 ? FLAG_C : 0));
#define CPY(a)                      \
    wD0 = (uint16_t)Y - (a);            \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    SETF(g_byTestTable[wD0 & 0xff] | (wD0 < 0x100 ? FLAG_C : 0));

// Math Op. (A D flag isn't being supported.)
#define ADC(a)                                                             \
    byD0 = (a);                                                            \
    wD0 = A + byD0 + (F & FLAG_C);                                         \
    byD1 = (uint8_t)wD0;                                                      \
    RSTF(FLAG_N | FLAG_V | FLAG_Z | FLAG_C);                               \
    SETF(g_byTestTable[byD1] |                                             \
         ((~(A ^ byD0) & (A ^ byD1) & 0x80) ? FLAG_V : 0) | (wD0 > 0xff)); \
    A = byD1;

#define SBC(a)                                                             \
    byD0 = (a);                                                            \
    wD0 = A - byD0 - (~F & FLAG_C);                                        \
    byD1 = (uint8_t)wD0;                                                      \
    RSTF(FLAG_N | FLAG_V | FLAG_Z | FLAG_C);                               \
    SETF(g_byTestTable[byD1] |                                             \
         (((A ^ byD0) & (A ^ byD1) & 0x80) ? FLAG_V : 0) | (wD0 < 0x100)); \
    A = byD1;

#define DEC(a)              \
    wA0 = a;                \
    byD0 = K6502_Read(wA0); \
    --byD0;                 \
    K6502_Write(wA0, byD0); \
    TEST(byD0)
#define INC(a)              \
    wA0 = a;                \
    byD0 = K6502_Read(wA0); \
    ++byD0;                 \
    K6502_Write(wA0, byD0); \
    TEST(byD0)

// Shift Op.
#define ASLA                        \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    SETF(g_ASLTable[A].byFlag);     \
    A = g_ASLTable[A].byValue
#define ASL(a)                      \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    wA0 = a;                        \
    byD0 = K6502_Read(wA0);         \
    SETF(g_ASLTable[byD0].byFlag);  \
    K6502_Write(wA0, g_ASLTable[byD0].byValue)
#define LSRA                        \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    SETF(g_LSRTable[A].byFlag);     \
    A = g_LSRTable[A].byValue
#define LSR(a)                      \
    RSTF(FLAG_N | FLAG_Z | FLAG_C); \
    wA0 = a;                        \
    byD0 = K6502_Read(wA0);         \
    SETF(g_LSRTable[byD0].byFlag);  \
    K6502_Write(wA0, g_LSRTable[byD0].byValue)
#define ROLA                          \
    byD0 = F & FLAG_C;                \
    RSTF(FLAG_N | FLAG_Z | FLAG_C);   \
    SETF(g_ROLTable[byD0][A].byFlag); \
    A = g_ROLTable[byD0][A].byValue
#define ROL(a)                           \
    byD1 = F & FLAG_C;                   \
    RSTF(FLAG_N | FLAG_Z | FLAG_C);      \
    wA0 = a;                             \
    byD0 = K6502_Read(wA0);              \
    SETF(g_ROLTable[byD1][byD0].byFlag); \
    K6502_Write(wA0, g_ROLTable[byD1][byD0].byValue)
#define RORA                          \
    byD0 = F & FLAG_C;                \
    RSTF(FLAG_N | FLAG_Z | FLAG_C);   \
    SETF(g_RORTable[byD0][A].byFlag); \
    A = g_RORTable[byD0][A].byValue
#define ROR(a)                           \
    byD1 = F & FLAG_C;                   \
    RSTF(FLAG_N | FLAG_Z | FLAG_C);      \
    wA0 = a;                             \
    byD0 = K6502_Read(wA0);              \
    SETF(g_RORTable[byD1][byD0].byFlag); \
    K6502_Write(wA0, g_RORTable[byD1][byD0].byValue)

// Jump Op.
#define JSR        \
    wA0 = AA_ABS2; \
    PUSHW(PC);     \
    PC = wA0;
#define BRA(a)                                      \
    if (a) {                                        \
        wA0 = PC;                                   \
        PC += (char)K6502_Read(PC);                 \
        CLK(3 + ((wA0 & 0x0100) != (PC & 0x0100))); \
        ++PC;                                       \
    } else {                                        \
        ++PC;                                       \
        CLK(2);                                     \
    }
#define JMP(a) PC = a;



/*===================================================================*/
/*                                                                   */
/*                K6502_Init() : Initialize K6502                    */
/*                                                                   */
/*===================================================================*/
void K6502_Init(void) {
    /*
     *  Initialize K6502
     *
     *  You must call this function only once at first.
     */

    uint8_t idx;
    uint8_t idx2;

    // The establishment of the IRQ pin
    NMI_Wiring = NMI_State = 1;
    IRQ_Wiring = IRQ_State = 1;

    // Make a table for the test
    idx = 0;
    do {
        if (idx == 0)
            g_byTestTable[0] = FLAG_Z;
        else if (idx > 127)
            g_byTestTable[idx] = FLAG_N;
        else
            g_byTestTable[idx] = 0;

        ++idx;
    } while (idx != 0);

    // Make a table ASL
    idx = 0;
    do {
        g_ASLTable[idx].byValue = idx << 1;
        g_ASLTable[idx].byFlag = 0;

        if (idx > 127) g_ASLTable[idx].byFlag = FLAG_C;

        if (g_ASLTable[idx].byValue == 0)
            g_ASLTable[idx].byFlag |= FLAG_Z;
        else if (g_ASLTable[idx].byValue & 0x80)
            g_ASLTable[idx].byFlag |= FLAG_N;

        ++idx;
    } while (idx != 0);

    // Make a table LSR
    idx = 0;
    do {
        g_LSRTable[idx].byValue = idx >> 1;
        g_LSRTable[idx].byFlag = 0;

        if (idx & 1) g_LSRTable[idx].byFlag = FLAG_C;

        if (g_LSRTable[idx].byValue == 0) g_LSRTable[idx].byFlag |= FLAG_Z;

        ++idx;
    } while (idx != 0);

    // Make a table ROL
    for (idx2 = 0; idx2 < 2; ++idx2) {
        idx = 0;
        do {
            g_ROLTable[idx2][idx].byValue = (idx << 1) | idx2;
            g_ROLTable[idx2][idx].byFlag = 0;

            if (idx > 127) g_ROLTable[idx2][idx].byFlag = FLAG_C;

            if (g_ROLTable[idx2][idx].byValue == 0)
                g_ROLTable[idx2][idx].byFlag |= FLAG_Z;
            else if (g_ROLTable[idx2][idx].byValue & 0x80)
                g_ROLTable[idx2][idx].byFlag |= FLAG_N;

            ++idx;
        } while (idx != 0);
    }

    // Make a table ROR
    for (idx2 = 0; idx2 < 2; ++idx2) {
        idx = 0;
        do {
            g_RORTable[idx2][idx].byValue = (idx >> 1) | (idx2 << 7);
            g_RORTable[idx2][idx].byFlag = 0;

            if (idx & 1) g_RORTable[idx2][idx].byFlag = FLAG_C;

            if (g_RORTable[idx2][idx].byValue == 0)
                g_RORTable[idx2][idx].byFlag |= FLAG_Z;
            else if (g_RORTable[idx2][idx].byValue & 0x80)
                g_RORTable[idx2][idx].byFlag |= FLAG_N;

            ++idx;
        } while (idx != 0);
    }
}

/*===================================================================*/
/*                                                                   */
/*                K6502_Reset() : Reset a CPU                        */
/*                                                                   */
/*===================================================================*/
void K6502_Reset(void) {
    /*
     *  Reset a CPU
     *
     */

    // Reset Registers
    PC = K6502_ReadW(VECTOR_RESET);
    SP = 0xFF;
    A = X = Y = 0;
    F = FLAG_Z | FLAG_R | FLAG_I;

    // Set up the state of the Interrupt pin.
    NMI_State = NMI_Wiring;
    IRQ_State = IRQ_Wiring;

    // Reset Passed Clocks
    g_wPassedClocks = 0;
}

/*===================================================================*/
/*                                                                   */
/*    K6502_Set_Int_Wiring() : Set up wiring of the interrupt pin    */
/*                                                                   */
/*===================================================================*/
void K6502_Set_Int_Wiring(uint8_t byNMI_Wiring, uint8_t byIRQ_Wiring) {
    /*
     * Set up wiring of the interrupt pin
     *
     */

    NMI_Wiring = byNMI_Wiring;
    IRQ_Wiring = byIRQ_Wiring;
}

/*===================================================================*/
/*                                                                   */
/*  K6502_Step() :                                                   */
/*          Only the specified number of the clocks execute Op.      */
/*                                                                   */
/*===================================================================*/
void K6502_Step(uint16_t wClocks) {
    /*
     *  Only the specified number of the clocks execute Op.
     *
     *  Parameters
     *    uint16_t wClocks              (Read)
     *      The number of the clocks
     */

    uint8_t byCode;

    uint16_t wA0;
    uint8_t byD0;
    uint8_t byD1;
    uint16_t wD0;

    // Dispose of it if there is an interrupt requirement
    if (NMI_State != NMI_Wiring) {
        // NMI Interrupt
        NMI_State = NMI_Wiring;
        CLK(7);

        PUSHW(PC);
        PUSH(F & ~FLAG_B);

        RSTF(FLAG_D);
        SETF(FLAG_I);

        PC = K6502_ReadW(VECTOR_NMI);
    } else if (IRQ_State != IRQ_Wiring) {
        // IRQ Interrupt
        // Execute IRQ if an I flag isn't being set
        if (!(F & FLAG_I)) {
            IRQ_State = IRQ_Wiring;
            CLK(7);

            PUSHW(PC);
            PUSH(F & ~FLAG_B);

            RSTF(FLAG_D);
            SETF(FLAG_I);

            PC = K6502_ReadW(VECTOR_IRQ);
        }
    }

    // It has a loop until a constant clock passes
    while (g_wPassedClocks < wClocks) {
        // Read an instruction
        byCode = K6502_Read(PC++);

        // Execute an instruction.
        switch (byCode) {
            case 0x00:  // BRK
                ++PC;
                PUSHW(PC);
                SETF(FLAG_B);
                PUSH(F);
                SETF(FLAG_I);
                RSTF(FLAG_D);
                PC = K6502_ReadW(VECTOR_IRQ);
                CLK(7);
                break;

            case 0x01:  // ORA (Zpg,X)
                ORA(A_IX);
                CLK(6);
                break;

            case 0x05:  // ORA Zpg
                ORA(A_ZP);
                CLK(3);
                break;

            case 0x06:  // ASL Zpg
                ASL(AA_ZP);
                CLK(5);
                break;

            case 0x08:  // PHP
                SETF(FLAG_B);
                PUSH(F);
                CLK(3);
                break;

            case 0x09:  // ORA #Oper
                ORA(A_IMM);
                CLK(2);
                break;

            case 0x0A:  // ASL A
                ASLA;
                CLK(2);
                break;

            case 0x0D:  // ORA Abs
                ORA(A_ABS);
                CLK(4);
                break;

            case 0x0e:  // ASL Abs
                ASL(AA_ABS);
                CLK(6);
                break;

            case 0x10:  // BPL Oper
                BRA(!(F & FLAG_N));
                break;

            case 0x11:  // ORA (Zpg),Y
                ORA(A_IY);
                CLK(5);
                break;

            case 0x15:  // ORA Zpg,X
                ORA(A_ZPX);
                CLK(4);
                break;

            case 0x16:  // ASL Zpg,X
                ASL(AA_ZPX);
                CLK(6);
                break;

            case 0x18:  // CLC
                RSTF(FLAG_C);
                CLK(2);
                break;

            case 0x19:  // ORA Abs,Y
                ORA(A_ABSY);
                CLK(4);
                break;

            case 0x1D:  // ORA Abs,X
                ORA(A_ABSX);
                CLK(4);
                break;

            case 0x1E:  // ASL Abs,X
                ASL(AA_ABSX);
                CLK(7);
                break;

            case 0x20:  // JSR Abs
                JSR;
                CLK(6);
                break;

            case 0x21:  // AND (Zpg,X)
                AND(A_IX);
                CLK(6);
                break;

            case 0x24:  // BIT Zpg
                BIT(A_ZP);
                CLK(3);
                break;

            case 0x25:  // AND Zpg
                AND(A_ZP);
                CLK(3);
                break;

            case 0x26:  // ROL Zpg
                ROL(AA_ZP);
                CLK(5);
                break;

            case 0x28:  // PLP
                POP(F);
                SETF(FLAG_R);
                CLK(4);
                break;

            case 0x29:  // AND #Oper
                AND(A_IMM);
                CLK(2);
                break;

            case 0x2A:  // ROL A
                ROLA;
                CLK(2);
                break;

            case 0x2C:  // BIT Abs
                BIT(A_ABS);
                CLK(4);
                break;

            case 0x2D:  // AND Abs
                AND(A_ABS);
                CLK(4);
                break;

            case 0x2E:  // ROL Abs
                ROL(AA_ABS);
                CLK(6);
                break;

            case 0x30:  // BMI Oper
                BRA(F & FLAG_N);
                break;

            case 0x31:  // AND (Zpg),Y
                AND(A_IY);
                CLK(5);
                break;

            case 0x35:  // AND Zpg,X
                AND(A_ZPX);
                CLK(4);
                break;

            case 0x36:  // ROL Zpg,X
                ROL(AA_ZPX);
                CLK(6);
                break;

            case 0x38:  // SEC
                SETF(FLAG_C);
                CLK(2);
                break;

            case 0x39:  // AND Abs,Y
                AND(A_ABSY);
                CLK(4);
                break;

            case 0x3D:  // AND Abs,X
                AND(A_ABSX);
                CLK(4);
                break;

            case 0x3E:  // ROL Abs,X
                ROL(AA_ABSX);
                CLK(7);
                break;

            case 0x40:  // RTI
                POP(F);
                SETF(FLAG_R);
                POPW(PC);
                CLK(6);
                break;

            case 0x41:  // EOR (Zpg,X)
                EOR(A_IX);
                CLK(6);
                break;

            case 0x45:  // EOR Zpg
                EOR(A_ZP);
                CLK(3);
                break;

            case 0x46:  // LSR Zpg
                LSR(AA_ZP);
                CLK(5);
                break;

            case 0x48:  // PHA
                PUSH(A);
                CLK(3);
                break;

            case 0x49:  // EOR #Oper
                EOR(A_IMM);
                CLK(2);
                break;

            case 0x4A:  // LSR A
                LSRA;
                CLK(2);
                break;

            case 0x4C:  // JMP Abs
                JMP(AA_ABS);
                CLK(3);
                break;

            case 0x4D:  // EOR Abs
                EOR(A_ABS);
                CLK(4);
                break;

            case 0x4E:  // LSR Abs
                LSR(AA_ABS);
                CLK(6);
                break;

            case 0x50:  // BVC
                BRA(!(F & FLAG_V));
                break;

            case 0x51:  // EOR (Zpg),Y
                EOR(A_IY);
                CLK(5);
                break;

            case 0x55:  // EOR Zpg,X
                EOR(A_ZPX);
                CLK(4);
                break;

            case 0x56:  // LSR Zpg,X
                LSR(AA_ZPX);
                CLK(6);
                break;

            case 0x58:  // CLI
                byD0 = F;
                RSTF(FLAG_I);
                CLK(2);
                if ((byD0 & FLAG_I) && IRQ_State != IRQ_Wiring) {
                    IRQ_State = IRQ_Wiring;
                    CLK(7);

                    PUSHW(PC);
                    PUSH(F & ~FLAG_B);

                    RSTF(FLAG_D);
                    SETF(FLAG_I);

                    PC = K6502_ReadW(VECTOR_IRQ);
                }
                break;

            case 0x59:  // EOR Abs,Y
                EOR(A_ABSY);
                CLK(4);
                break;

            case 0x5D:  // EOR Abs,X
                EOR(A_ABSX);
                CLK(4);
                break;

            case 0x5E:  // LSR Abs,X
                LSR(AA_ABSX);
                CLK(7);
                break;

            case 0x60:  // RTS
                POPW(PC);
                ++PC;
                CLK(6);
                break;

            case 0x61:  // ADC (Zpg,X)
                ADC(A_IX);
                CLK(6);
                break;

            case 0x65:  // ADC Zpg
                ADC(A_ZP);
                CLK(3);
                break;

            case 0x66:  // ROR Zpg
                ROR(AA_ZP);
                CLK(5);
                break;

            case 0x68:  // PLA
                POP(A);
                TEST(A);
                CLK(4);
                break;

            case 0x69:  // ADC #Oper
                ADC(A_IMM);
                CLK(2);
                break;

            case 0x6A:  // ROR A
                RORA;
                CLK(2);
                break;

            case 0x6C:  // JMP (Abs)
                JMP(K6502_ReadW2(AA_ABS));
                CLK(5);
                break;

            case 0x6D:  // ADC Abs
                ADC(A_ABS);
                CLK(4);
                break;

            case 0x6E:  // ROR Abs
                ROR(AA_ABS);
                CLK(6);
                break;

            case 0x70:  // BVS
                BRA(F & FLAG_V);
                break;

            case 0x71:  // ADC (Zpg),Y
                ADC(A_IY);
                CLK(5);
                break;

            case 0x75:  // ADC Zpg,X
                ADC(A_ZPX);
                CLK(4);
                break;

            case 0x76:  // ROR Zpg,X
                ROR(AA_ZPX);
                CLK(6);
                break;

            case 0x78:  // SEI
                SETF(FLAG_I);
                CLK(2);
                break;

            case 0x79:  // ADC Abs,Y
                ADC(A_ABSY);
                CLK(4);
                break;

            case 0x7D:  // ADC Abs,X
                ADC(A_ABSX);
                CLK(4);
                break;

            case 0x7E:  // ROR Abs,X
                ROR(AA_ABSX);
                CLK(7);
                break;

            case 0x81:  // STA (Zpg,X)
                STA(AA_IX);
                CLK(6);
                break;

            case 0x84:  // STY Zpg
                STY(AA_ZP);
                CLK(3);
                break;

            case 0x85:  // STA Zpg
                STA(AA_ZP);
                CLK(3);
                break;

            case 0x86:  // STX Zpg
                STX(AA_ZP);
                CLK(3);
                break;

            case 0x88:  // DEY
                --Y;
                TEST(Y);
                CLK(2);
                break;

            case 0x8A:  // TXA
                A = X;
                TEST(A);
                CLK(2);
                break;

            case 0x8C:  // STY Abs
                STY(AA_ABS);
                CLK(4);
                break;

            case 0x8D:  // STA Abs
                STA(AA_ABS);
                CLK(4);
                break;

            case 0x8E:  // STX Abs
                STX(AA_ABS);
                CLK(4);
                break;

            case 0x90:  // BCC
                BRA(!(F & FLAG_C));
                break;

            case 0x91:  // STA (Zpg),Y
                STA(AA_IY);
                CLK(6);
                break;

            case 0x94:  // STY Zpg,X
                STY(AA_ZPX);
                CLK(4);
                break;

            case 0x95:  // STA Zpg,X
                STA(AA_ZPX);
                CLK(4);
                break;

            case 0x96:  // STX Zpg,Y
                STX(AA_ZPY);
                CLK(4);
                break;

            case 0x98:  // TYA
                A = Y;
                TEST(A);
                CLK(2);
                break;

            case 0x99:  // STA Abs,Y
                STA(AA_ABSY);
                CLK(5);
                break;

            case 0x9A:  // TXS
                SP = X;
                CLK(2);
                break;

            case 0x9D:  // STA Abs,X
                STA(AA_ABSX);
                CLK(5);
                break;

            case 0xA0:  // LDY #Oper
                LDY(A_IMM);
                CLK(2);
                break;

            case 0xA1:  // LDA (Zpg,X)
                LDA(A_IX);
                CLK(6);
                break;

            case 0xA2:  // LDX #Oper
                LDX(A_IMM);
                CLK(2);
                break;

            case 0xA4:  // LDY Zpg
                LDY(A_ZP);
                CLK(3);
                break;

            case 0xA5:  // LDA Zpg
                LDA(A_ZP);
                CLK(3);
                break;

            case 0xA6:  // LDX Zpg
                LDX(A_ZP);
                CLK(3);
                break;

            case 0xA8:  // TAY
                Y = A;
                TEST(A);
                CLK(2);
                break;

            case 0xA9:  // LDA #Oper
                LDA(A_IMM);
                CLK(2);
                break;

            case 0xAA:  // TAX
                X = A;
                TEST(A);
                CLK(2);
                break;

            case 0xAC:  // LDY Abs
                LDY(A_ABS);
                CLK(4);
                break;

            case 0xAD:  // LDA Abs
                LDA(A_ABS);
                CLK(4);
                break;

            case 0xAE:  // LDX Abs
                LDX(A_ABS);
                CLK(4);
                break;

            case 0xB0:  // BCS
                BRA(F & FLAG_C);
                break;

            case 0xB1:  // LDA (Zpg),Y
                LDA(A_IY);
                CLK(5);
                break;

            case 0xB4:  // LDY Zpg,X
                LDY(A_ZPX);
                CLK(4);
                break;

            case 0xB5:  // LDA Zpg,X
                LDA(A_ZPX);
                CLK(4);
                break;

            case 0xB6:  // LDX Zpg,Y
                LDX(A_ZPY);
                CLK(4);
                break;

            case 0xB8:  // CLV
                RSTF(FLAG_V);
                CLK(2);
                break;

            case 0xB9:  // LDA Abs,Y
                LDA(A_ABSY);
                CLK(4);
                break;

            case 0xBA:  // TSX
                X = SP;
                TEST(X);
                CLK(2);
                break;

            case 0xBC:  // LDY Abs,X
                LDY(A_ABSX);
                CLK(4);
                break;

            case 0xBD:  // LDA Abs,X
                LDA(A_ABSX);
                CLK(4);
                break;

            case 0xBE:  // LDX Abs,Y
                LDX(A_ABSY);
                CLK(4);
                break;

            case 0xC0:  // CPY #Oper
                CPY(A_IMM);
                CLK(2);
                break;

            case 0xC1:  // CMP (Zpg,X)
                CMP(A_IX);
                CLK(6);
                break;

            case 0xC4:  // CPY Zpg
                CPY(A_ZP);
                CLK(3);
                break;

            case 0xC5:  // CMP Zpg
                CMP(A_ZP);
                CLK(3);
                break;

            case 0xC6:  // DEC Zpg
                DEC(AA_ZP);
                CLK(5);
                break;

            case 0xC8:  // INY
                ++Y;
                TEST(Y);
                CLK(2);
                break;

            case 0xC9:  // CMP #Oper
                CMP(A_IMM);
                CLK(2);
                break;

            case 0xCA:  // DEX
                --X;
                TEST(X);
                CLK(2);
                break;

            case 0xCC:  // CPY Abs
                CPY(A_ABS);
                CLK(4);
                break;

            case 0xCD:  // CMP Abs
                CMP(A_ABS);
                CLK(4);
                break;

            case 0xCE:  // DEC Abs
                DEC(AA_ABS);
                CLK(6);
                break;

            case 0xD0:  // BNE
                BRA(!(F & FLAG_Z));
                break;

            case 0xD1:  // CMP (Zpg),Y
                CMP(A_IY);
                CLK(5);
                break;

            case 0xD5:  // CMP Zpg,X
                CMP(A_ZPX);
                CLK(4);
                break;

            case 0xD6:  // DEC Zpg,X
                DEC(AA_ZPX);
                CLK(6);
                break;

            case 0xD8:  // CLD
                RSTF(FLAG_D);
                CLK(2);
                break;

            case 0xD9:  // CMP Abs,Y
                CMP(A_ABSY);
                CLK(4);
                break;

            case 0xDD:  // CMP Abs,X
                CMP(A_ABSX);
                CLK(4);
                break;

            case 0xDE:  // DEC Abs,X
                DEC(AA_ABSX);
                CLK(7);
                break;

            case 0xE0:  // CPX #Oper
                CPX(A_IMM);
                CLK(2);
                break;

            case 0xE1:  // SBC (Zpg,X)
                SBC(A_IX);
                CLK(6);
                break;

            case 0xE4:  // CPX Zpg
                CPX(A_ZP);
                CLK(3);
                break;

            case 0xE5:  // SBC Zpg
                SBC(A_ZP);
                CLK(3);
                break;

            case 0xE6:  // INC Zpg
                INC(AA_ZP);
                CLK(5);
                break;

            case 0xE8:  // INX
                ++X;
                TEST(X);
                CLK(2);
                break;

            case 0xE9:  // SBC #Oper
                SBC(A_IMM);
                CLK(2);
                break;

            case 0xEA:  // NOP
                CLK(2);
                break;

            case 0xEC:  // CPX Abs
                CPX(A_ABS);
                CLK(4);
                break;

            case 0xED:  // SBC Abs
                SBC(A_ABS);
                CLK(4);
                break;

            case 0xEE:  // INC Abs
                INC(AA_ABS);
                CLK(6);
                break;

            case 0xF0:  // BEQ
                BRA(F & FLAG_Z);
                break;

            case 0xF1:  // SBC (Zpg),Y
                SBC(A_IY);
                CLK(5);
                break;

            case 0xF5:  // SBC Zpg,X
                SBC(A_ZPX);
                CLK(4);
                break;

            case 0xF6:  // INC Zpg,X
                INC(AA_ZPX);
                CLK(6);
                break;

            case 0xF8:  // SED
                SETF(FLAG_D);
                CLK(2);
                break;

            case 0xF9:  // SBC Abs,Y
                SBC(A_ABSY);
                CLK(4);
                break;

            case 0xFD:  // SBC Abs,X
                SBC(A_ABSX);
                CLK(4);
                break;

            case 0xFE:  // INC Abs,X
                INC(AA_ABSX);
                CLK(7);
                break;

                /*-----------------------------------------------------------*/
                /*  Unlisted Instructions ( thanks to virtualnes )           */
                /*-----------------------------------------------------------*/

            case 0x1A:  // NOP (Unofficial)
            case 0x3A:  // NOP (Unofficial)
            case 0x5A:  // NOP (Unofficial)
            case 0x7A:  // NOP (Unofficial)
            case 0xDA:  // NOP (Unofficial)
            case 0xFA:  // NOP (Unofficial)
                CLK(2);
                break;

            case 0x80:  // DOP (CYCLES 2)
            case 0x82:  // DOP (CYCLES 2)
            case 0x89:  // DOP (CYCLES 2)
            case 0xC2:  // DOP (CYCLES 2)
            case 0xE2:  // DOP (CYCLES 2)
                PC++;
                CLK(2);
                break;

            case 0x04:  // DOP (CYCLES 3)
            case 0x44:  // DOP (CYCLES 3)
            case 0x64:  // DOP (CYCLES 3)
                PC++;
                CLK(3);
                break;

            case 0x14:  // DOP (CYCLES 4)
            case 0x34:  // DOP (CYCLES 4)
            case 0x54:  // DOP (CYCLES 4)
            case 0x74:  // DOP (CYCLES 4)
            case 0xD4:  // DOP (CYCLES 4)
            case 0xF4:  // DOP (CYCLES 4)
                PC++;
                CLK(4);
                break;

            case 0x0C:  // TOP
            case 0x1C:  // TOP
            case 0x3C:  // TOP
            case 0x5C:  // TOP
            case 0x7C:  // TOP
            case 0xDC:  // TOP
            case 0xFC:  // TOP
                PC += 2;
                CLK(4);
                break;

            default:  // Unknown Instruction
                CLK(2);
            #if 1
                InfoNES_MessageBox( "0x%02x is unknown instruction.\n", byCode ) ;
            #endif
                break;

        } /* end of switch ( byCode ) */

    } /* end of while ... */

    // Correct the number of the clocks
    g_wPassedClocks -= wClocks;
}

// Addressing Op.
// Data
// Absolute,X
uint8_t K6502_ReadAbsX(void) {
    uint16_t wA0, wA1;
    wA0 = AA_ABS;
    wA1 = wA0 + X;
    CLK((wA0 & 0x0100) != (wA1 & 0x0100));
    return K6502_Read(wA1);
};
// Absolute,Y
uint8_t K6502_ReadAbsY(void) {
    uint16_t wA0, wA1;
    wA0 = AA_ABS;
    wA1 = wA0 + Y;
    CLK((wA0 & 0x0100) != (wA1 & 0x0100));
    return K6502_Read(wA1);
};
// (Indirect),Y
uint8_t K6502_ReadIY(void) {
    uint16_t wA0, wA1;
    wA0 = K6502_ReadZpW(K6502_Read(PC++));
    wA1 = wA0 + Y;
    CLK((wA0 & 0x0100) != (wA1 & 0x0100));
    return K6502_Read(wA1);
};

/*===================================================================*/
/*                                                                   */
/*                  6502 Reading/Writing Operation                   */
/*                                                                   */
/*===================================================================*/

/*===================================================================*/
/*                                                                   */
/*            K6502_ReadZp() : Reading from the zero page            */
/*                                                                   */
/*===================================================================*/
uint8_t K6502_ReadZp(uint8_t byAddr) {
    /*
     *  Reading from the zero page
     *
     *  Parameters
     *    uint8_t byAddr              (Read)
     *      An address inside the zero page
     *
     *  Return values
     *    Read Data
     */

    return RAM[byAddr];
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Read() : Reading operation                    */
/*                                                                   */
/*===================================================================*/
uint8_t K6502_Read(uint16_t wAddr) {
    /*
     *  Reading operation
     *
     *  Parameters
     *    uint16_t wAddr              (Read)
     *      Address to read
     *
     *  Return values
     *    Read data
     *
     *  Remarks
     *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
     *    0x2000 - 0x3fff  PPU
     *    0x4000 - 0x5fff  Sound
     *    0x6000 - 0x7fff  SRAM ( Battery Backed )
     *    0x8000 - 0xffff  ROM
     *
     */
    uint8_t byRet;

    switch (wAddr & 0xe000) {
        case 0x0000: /* RAM */
            return RAM[wAddr & 0x7ff];

        case 0x2000:                  /* PPU */
            if ((wAddr & 0x7) == 0x7) /* PPU Memory */
            {
                uint16_t addr = PPU_Addr;

                // Increment PPU Address
                PPU_Addr += PPU_Increment;
                addr &= 0x3fff;

                // Set return value;
                byRet = PPU_R7;

                // Read PPU Memory
                PPU_R7 = PPUBANK[addr >> 10][addr & 0x3ff];

                return byRet;
            } else if ((wAddr & 0x7) == 0x4) /* SPR_RAM I/O Register */
            {
                return SPRRAM[PPU_R3++];
            } else if ((wAddr & 0x7) == 0x2) /* PPU Status */
            {
                // Set return value
                byRet = PPU_R2;

                // Reset a V-Blank flag
                PPU_R2 &= ~R2_IN_VBLANK;

                // Reset address latch
                PPU_Latch_Flag = 0;

                // Make a Nametable 0 in V-Blank
                if (PPU_Scanline >= SCAN_VBLANK_START &&
                    !(PPU_R0 & R0_NMI_VB)) {
                    PPU_R0 &= ~R0_NAME_ADDR;
                    PPU_NameTableBank = NAME_TABLE0;
                }
                return byRet;
            }
            break;

        case 0x4000: /* Sound */
            if (wAddr == 0x4015) {
                // APU control
                byRet = APU_Reg[0x15];
                if (ApuC1Atl > 0) byRet |= (1 << 0);
                if (ApuC2Atl > 0) byRet |= (1 << 1);
                if (!ApuC3Holdnote) {
                    if (ApuC3Atl > 0) byRet |= (1 << 2);
                } else {
                    if (ApuC3Llc > 0) byRet |= (1 << 2);
                }
                if (ApuC4Atl > 0) byRet |= (1 << 3);

                // FrameIRQ
                APU_Reg[0x15] &= ~0x40;
                return byRet;
            } else if (wAddr == 0x4016) {
                // Set Joypad1 data
                byRet = (uint8_t)((PAD1_Latch >> PAD1_Bit) & 1) | 0x40;
                PAD1_Bit = (PAD1_Bit == 23) ? 0 : (PAD1_Bit + 1);
                return byRet;
            } else if (wAddr == 0x4017) {
                // Set Joypad2 data
                byRet = (uint8_t)((PAD2_Latch >> PAD2_Bit) & 1) | 0x40;
                PAD2_Bit = (PAD2_Bit == 23) ? 0 : (PAD2_Bit + 1);
                return byRet;
            } else {
                /* Return Mapper Register*/
                return MapperReadApu(wAddr);
            }
            break;
            // The other sound registers are not readable.

        case 0x6000: /* SRAM */
            if (ROM_SRAM) {
                return SRAM[wAddr & 0x1fff];
            } else { /* SRAM BANK */
                return SRAMBANK[wAddr & 0x1fff];
            }

        case 0x8000: /* ROM BANK 0 */
            return ROMBANK0[wAddr & 0x1fff];

        case 0xa000: /* ROM BANK 1 */
            return ROMBANK1[wAddr & 0x1fff];

        case 0xc000: /* ROM BANK 2 */
            return ROMBANK2[wAddr & 0x1fff];

        case 0xe000: /* ROM BANK 3 */
            return ROMBANK3[wAddr & 0x1fff];
    }

    return (wAddr >> 8); /* when a register is not readable the upper half
                            address is returned. */
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Write() : Writing operation                    */
/*                                                                   */
/*===================================================================*/
void K6502_Write(uint16_t wAddr, uint8_t byData) {
    /*
     *  Writing operation
     *
     *  Parameters
     *    uint16_t wAddr              (Read)
     *      Address to write
     *
     *    uint8_t byData             (Read)
     *      Data to write
     *
     *  Remarks
     *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
     *    0x2000 - 0x3fff  PPU
     *    0x4000 - 0x5fff  Sound
     *    0x6000 - 0x7fff  SRAM ( Battery Backed )
     *    0x8000 - 0xffff  ROM
     *
     */

    switch (wAddr & 0xe000) {
        case 0x0000: /* RAM */
            RAM[wAddr & 0x7ff] = byData;
            break;

        case 0x2000: /* PPU */
            switch (wAddr & 0x7) {
                case 0: /* 0x2000 */
                    PPU_R0 = byData;
                    PPU_Increment = (PPU_R0 & R0_INC_ADDR) ? 32 : 1;
                    PPU_NameTableBank = NAME_TABLE0 + (PPU_R0 & R0_NAME_ADDR);
                    PPU_BG_Base =
                        (PPU_R0 & R0_BG_ADDR) ? ChrBuf + 256 * 64 : ChrBuf;
                    PPU_SP_Base =
                        (PPU_R0 & R0_SP_ADDR) ? ChrBuf + 256 * 64 : ChrBuf;
                    PPU_SP_Height = (PPU_R0 & R0_SP_SIZE) ? 16 : 8;

                    // Account for Loopy's scrolling discoveries
                    PPU_Temp =
                        (PPU_Temp & 0xF3FF) | ((((uint16_t)byData) & 0x0003) << 10);
                    break;

                case 1: /* 0x2001 */
                    PPU_R1 = byData;
                    break;

                case 2: /* 0x2002 */
                #if 1 
                    PPU_R2 = byData;     // 0x2002 is not writable
                #endif
                    break;

                case 3: /* 0x2003 */
                    // Sprite RAM Address
                    PPU_R3 = byData;
                    break;

                case 4: /* 0x2004 */
                    // Write data to Sprite RAM
                    SPRRAM[PPU_R3++] = byData;
                    break;

                case 5: /* 0x2005 */
                    // Set Scroll Register
                    if (PPU_Latch_Flag) {
                        // V-Scroll Register
                        PPU_Scr_V_Next = (byData > 239) ? 0 : byData;
                        PPU_Scr_V_Byte_Next = PPU_Scr_V_Next >> 3;
                        PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 7;

                        // Added : more Loopy Stuff
                        PPU_Temp = (PPU_Temp & 0xFC1F) |
                                   ((((uint16_t)byData) & 0xF8) << 2);
                        PPU_Temp = (PPU_Temp & 0x8FFF) |
                                   ((((uint16_t)byData) & 0x07) << 12);
                    } else {
                        // H-Scroll Register
                        PPU_Scr_H_Next = byData;
                        PPU_Scr_H_Byte_Next = PPU_Scr_H_Next >> 3;
                        PPU_Scr_H_Bit_Next = PPU_Scr_H_Next & 7;

                        // Added : more Loopy Stuff
                        PPU_Temp = (PPU_Temp & 0xFFE0) |
                                   ((((uint16_t)byData) & 0xF8) >> 3);
                    }
                    PPU_Latch_Flag ^= 1;
                    break;

                case 6: /* 0x2006 */
                    // Set PPU Address
                    if (PPU_Latch_Flag) {
                        /* Low */
                    #if 1
                        PPU_Addr = ( PPU_Addr & 0xff00 ) | ( (uint16_t)byData );
                    #else
                        PPU_Temp =
                            (PPU_Temp & 0xFF00) | (((uint16_t)byData) & 0x00FF);
                        PPU_Addr = PPU_Temp;
                    #endif
                        InfoNES_SetupScr();
                    } else {
                        /* High */
                    #if 1
                        PPU_Addr = ( PPU_Addr & 0x00ff ) | ( (uint16_t)( byData & 0x3f ) << 8 );
                        InfoNES_SetupScr();
                    #else
                        PPU_Temp = (PPU_Temp & 0x00FF) |
                                   ((((uint16_t)byData) & 0x003F) << 8);
                    #endif
                    }
                    PPU_Latch_Flag ^= 1;
                    break;

                case 7: /* 0x2007 */
                {
                    uint16_t addr = PPU_Addr;

                    // Increment PPU Address
                    PPU_Addr += PPU_Increment;
                    addr &= 0x3fff;

                    // Write to PPU Memory
                    if (addr < 0x2000 && byVramWriteEnable) {
                        // Pattern Data
                        ChrBufUpdate |= (1 << (addr >> 10));
                        PPUBANK[addr >> 10][addr & 0x3ff] = byData;
                    } else if (addr < 0x3f00) /* 0x2000 - 0x3eff */
                    {
                        // Name Table and mirror
                        PPUBANK[addr >> 10][addr & 0x3ff] = byData;
                        PPUBANK[(addr ^ 0x1000) >> 10][addr & 0x3ff] = byData;
                    } else if (!(addr & 0xf)) /* 0x3f00 or 0x3f10 */
                    {
                        // Palette mirror
                        PPURAM[0x3f10] = PPURAM[0x3f14] = PPURAM[0x3f18] =
                            PPURAM[0x3f1c] = PPURAM[0x3f00] = PPURAM[0x3f04] =
                                PPURAM[0x3f08] = PPURAM[0x3f0c] = byData;
                        PalTable[0x00] = PalTable[0x04] = PalTable[0x08] =
                            PalTable[0x0c] = PalTable[0x10] = PalTable[0x14] =
                                PalTable[0x18] = PalTable[0x1c] =
                                    NesPalette[byData] | 0x8000;
                    } else if (addr & 3) {
                        // Palette
                        PPURAM[addr] = byData;
                        PalTable[addr & 0x1f] = NesPalette[byData];
                    }
                } break;
            }
            break;

        case 0x4000: /* Sound */
            switch (wAddr & 0x1f) {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x08:
                case 0x09:
                case 0x0a:
                case 0x0b:
                case 0x0c:
                case 0x0d:
                case 0x0e:
                case 0x0f:
                case 0x10:
                case 0x11:
                case 0x12:
                case 0x13:
                    // Call Function corresponding to Sound Registers
                    if (!APU_Mute) pAPUSoundRegs[wAddr & 0x1f](wAddr, byData);
                    break;

                case 0x14: /* 0x4014 */
                    // Sprite DMA
                    switch (byData >> 5) {
                        case 0x0: /* RAM */
                            InfoNES_MemoryCopy(
                                SPRRAM, &RAM[((uint16_t)byData << 8) & 0x7ff],
                                SPRRAM_SIZE);
                            break;

                        case 0x3: /* SRAM */
                            InfoNES_MemoryCopy(
                                SPRRAM, &SRAM[((uint16_t)byData << 8) & 0x1fff],
                                SPRRAM_SIZE);
                            break;

                        case 0x4: /* ROM BANK 0 */
                            InfoNES_MemoryCopy(
                                SPRRAM, &ROMBANK0[((uint16_t)byData << 8) & 0x1fff],
                                SPRRAM_SIZE);
                            break;

                        case 0x5: /* ROM BANK 1 */
                            InfoNES_MemoryCopy(
                                SPRRAM, &ROMBANK1[((uint16_t)byData << 8) & 0x1fff],
                                SPRRAM_SIZE);
                            break;

                        case 0x6: /* ROM BANK 2 */
                            InfoNES_MemoryCopy(
                                SPRRAM, &ROMBANK2[((uint16_t)byData << 8) & 0x1fff],
                                SPRRAM_SIZE);
                            break;

                        case 0x7: /* ROM BANK 3 */
                            InfoNES_MemoryCopy(
                                SPRRAM, &ROMBANK3[((uint16_t)byData << 8) & 0x1fff],
                                SPRRAM_SIZE);
                            break;
                    }
                    break;

                case 0x15: /* 0x4015 */
                    InfoNES_pAPUWriteControl(wAddr, byData);
                #if 1
                    /* Unknown */
                    if ( byData & 0x10 ) 
                    {
                        byData &= ~0x80;
                    }
                #endif
                    break;

                case 0x16: /* 0x4016 */
                    // Reset joypad
                    if (!(APU_Reg[0x16] & 1) && (byData & 1)) {
                        PAD1_Bit = 0;
                        PAD2_Bit = 0;
                    }
                    break;

                case 0x17: /* 0x4017 */
                    // Frame IRQ
                    FrameStep = 0;
                    if (!(byData & 0xc0)) {
                        FrameIRQ_Enable = 1;
                    } else {
                        FrameIRQ_Enable = 0;
                    }
                    break;
            }

            if (wAddr <= 0x4017) {
                /* Write to APU Register */
                APU_Reg[wAddr & 0x1f] = byData;
            } else {
                /* Write to APU */
                MapperApu(wAddr, byData);
            }
            break;

        case 0x6000: /* SRAM */
            SRAM[wAddr & 0x1fff] = byData;

            /* Write to SRAM, when no SRAM */
            if (!ROM_SRAM) {
                MapperSram(wAddr, byData);
            }
            break;

        case 0x8000: /* ROM BANK 0 */
        case 0xa000: /* ROM BANK 1 */
        case 0xc000: /* ROM BANK 2 */
        case 0xe000: /* ROM BANK 3 */
            // Write to Mapper
            MapperWrite(wAddr, byData);
            break;
    }
}

// Reading/Writing operation (uint16_t version)
uint16_t K6502_ReadW(uint16_t wAddr) {
    return K6502_Read(wAddr) | (uint16_t)K6502_Read(wAddr + 1) << 8;
};
void K6502_WriteW(uint16_t wAddr, uint16_t wData) {
    K6502_Write(wAddr, wData & 0xff);
    K6502_Write(wAddr + 1, wData >> 8);
};
uint16_t K6502_ReadZpW(uint8_t byAddr) {
    return K6502_ReadZp(byAddr) | (K6502_ReadZp(byAddr + 1) << 8);
};

// 6502's indirect absolute jmp(opcode: 6C) has a bug (added at 01/08/15 )
uint16_t K6502_ReadW2(uint16_t wAddr) {
    if (0x00ff == (wAddr & 0x00ff)) {
        return K6502_Read(wAddr) | (uint16_t)K6502_Read(wAddr - 0x00ff) << 8;
    } else {
        return K6502_Read(wAddr) | (uint16_t)K6502_Read(wAddr + 1) << 8;
    }
}
