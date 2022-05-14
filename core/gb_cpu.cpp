//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"
#include "gb_core.h"
#include "gb_memory.h"
#include "gb_irq.h"
#include "gb_speed.h"
#include "gb_cpu.h"

static bool HALT;
static bool IME;

static v16 PC;
static v16 SP;
static v16 AF;
static v16 BC;
static v16 DE;
static v16 HL;

static u8  OPCODE;
static u32 MCYCLES;
static u8  REG_6;

static void (*MapNP[256])();
static void (*MapCB[256])();

static u8  *MapR8[8];
static v16 *MapAF[4];
static v16 *MapSP[4];

//-----------------------------------------------------------------------------
// Control
//-----------------------------------------------------------------------------

#define Z_FLAG 0x80
#define N_FLAG 0x40
#define H_FLAG 0x20
#define C_FLAG 0x10

#define SET_ZF(cc) ((cc) ? Z_FLAG : 0)
#define SET_HF(cc) ((cc) ? H_FLAG : 0)
#define SET_CF(cc) ((cc) ? C_FLAG : 0)

#define R8_DST (*MapR8[((OPCODE >> 3) & 0x07)])
#define R8_SRC (*MapR8[( OPCODE       & 0x07)])
#define R16_AF ( MapAF[((OPCODE >> 4) & 0x03)])
#define R16_SP ( MapSP[((OPCODE >> 4) & 0x03)])
#define BIT_OP (        (OPCODE >> 3) & 0x07  )

#define REG_A AF.b.hi
#define REG_F AF.b.lo
#define REG_B BC.b.hi
#define REG_C BC.b.lo
#define REG_D DE.b.hi
#define REG_E DE.b.lo
#define REG_H HL.b.hi
#define REG_L HL.b.lo

void gb_ResetCPU()
{
    HALT = false;

    OPCODE   = 0;
    MCYCLES = 0;

    PC.w = 0x0100;
    SP.w = 0xFFFE;
    IME  = false;

    if (gb_IsCGB()) { AF.w = 0x1180; BC.w = 0x0000; DE.w = 0xFF56; HL.w = 0x000D; }
    else            { AF.w = 0x01B0; BC.w = 0x0013; DE.w = 0x00D8; HL.w = 0x014D; }
}

void gb_FetchInstruction()
{
    OPCODE = gb_ReadByte(PC.w++);
}

void gb_ExecuteNP()
{
    (*MapNP[OPCODE])();
}

void gb_ExecuteCB()
{
    (*MapCB[OPCODE])();
}

s32 gb_ExecuteNextInstruction()
{
    gb_FetchInstruction();
    switch (OPCODE)
    {
    case 0xCB: gb_FetchInstruction(); gb_ExecuteCB(); break;
    default:                          gb_ExecuteNP();
    }

    return MCYCLES * 4;
}

bool gb_isHalted()
{
    return HALT;
}

//-----------------------------------------------------------------------------
// Common Operations
//-----------------------------------------------------------------------------

u8 gb_mu_ReadImm8()
{
    return gb_ReadByte(PC.w++);
}

u16 gb_mu_ReadImm16()
{
    v16 tmp;
    tmp.b.lo = gb_ReadByte(PC.w++);
    tmp.b.hi = gb_ReadByte(PC.w++);
    return tmp.w;
}

void gb_mu_PUSH(u16 v)
{
    v16 tmp;
    tmp.w = v;
    gb_WriteByte(--SP.w, tmp.b.hi);
    gb_WriteByte(--SP.w, tmp.b.lo);
}

u16 gb_mu_POP()
{
    v16 tmp;
    tmp.b.lo = gb_ReadByte(SP.w++);
    tmp.b.hi = gb_ReadByte(SP.w++);
    return tmp.w;
}

bool gb_mu_TestCC()
{
    switch ((OPCODE >> 3) & 0x03)
    {
    case 0x00: return !(REG_F & Z_FLAG);
    case 0x01: return  (REG_F & Z_FLAG);
    case 0x02: return !(REG_F & C_FLAG);
    case 0x03: return  (REG_F & C_FLAG);
    default:   return false;
    }
}

void gb_mu_ADD_A_x(u8 x)
{
    v16 tmp;
    tmp.w = REG_A + x;
    u8 v = tmp.b.lo;
    REG_F = SET_ZF(v == 0) | SET_HF(((REG_A ^ x) ^ v) & 0x10) | SET_CF(tmp.b.hi);
    REG_A = v;
}

void gb_mu_ADC_A_x(u8 x)
{
    v16 tmp;
    tmp.w = REG_A + x + ((REG_F & C_FLAG) ? 1 : 0);
    u8 v = tmp.b.lo;
    REG_F = SET_ZF(v == 0) | SET_HF(((REG_A ^ x) ^ v) & 0x10) | SET_CF(tmp.b.hi);
    REG_A = v;
}

void gb_mu_SUB_A_x(u8 x)
{
    v16 tmp;
    tmp.w = REG_A - x;
    u8 v = tmp.b.lo;
    REG_F = SET_ZF(v == 0) | N_FLAG | SET_HF(((REG_A ^ x) ^ v) & 0x10) | SET_CF(tmp.b.hi);
    REG_A = v;
}

void gb_mu_SBC_A_x(u8 x)
{
    v16 tmp;
    tmp.w = REG_A - x - ((REG_F & C_FLAG) ? 1 : 0);
    u8 v = tmp.b.lo;
    REG_F = SET_ZF(v == 0) | N_FLAG | SET_HF(((REG_A ^ x) ^ v) & 0x10) | SET_CF(tmp.b.hi);
    REG_A = v;
}

void gb_mu_CP_A_x(u8 x)
{
    v16 tmp;
    tmp.w = REG_A - x;
    u8 v = tmp.b.lo;
    REG_F = SET_ZF(v == 0) | N_FLAG | SET_HF(((REG_A ^ x) ^ v) & 0x10) | SET_CF(tmp.b.hi);
}

void gb_mu_AND_A_x(u8 x)
{
    REG_A &= x;
    REG_F = SET_ZF(REG_A == 0) | H_FLAG;
}

void gb_mu_OR_A_x(u8 x)
{
    REG_A |= x;
    REG_F = SET_ZF(REG_A == 0);
}

void gb_mu_XOR_A_x(u8 x)
{
    REG_A ^= x;
    REG_F = SET_ZF(REG_A == 0);
}

u8 gb_mu_INC_x(u8 x)
{
    u8 v = x + 1;
    REG_F = SET_ZF(v == 0) | SET_HF((v & 0x0F) == 0x00) | (REG_F & C_FLAG);
    return v;
}

u8 gb_mu_DEC_x(u8 x)
{
    u8 v = x - 1;
    REG_F = SET_ZF(v == 0) | N_FLAG | SET_HF((v & 0x0F) == 0x0F) | (REG_F & C_FLAG);
    return v;
}

u16 gb_mu_ADD_16(u16 op1, u16 op2)
{
    v32 v;
    v.d = op1 + op2;
    REG_F = SET_HF(((op1 ^ op2) ^ v.w.lo.w) & 0x1000) | SET_CF(v.w.hi.w);
    return v.w.lo.w;
}

u8 gb_mu_RLC_x(u8 x, bool z0)
{
    u8 v = (x << 1) | (x >> 7);
    REG_F = SET_ZF(!z0 && (v == 0)) | SET_CF(x & 0x80);
    return v;
}

u8 gb_mu_RL_x(u8 x, bool z0)
{
    u8 v = (x << 1) | ((REG_F & C_FLAG) >> 4);
    REG_F = SET_ZF(!z0 && (v == 0)) | SET_CF(x & 0x80);
    return v;
}

u8 gb_mu_RRC_x(u8 x, bool z0)
{
    u8 v = (x >> 1) | (x << 7);
    REG_F = SET_ZF(!z0 && (v == 0)) | SET_CF(x & 0x01);
    return v;
}

u8 gb_mu_RR_x(u8 x, bool z0)
{
    u8 v = (x >> 1) | ((REG_F & C_FLAG) << 3);
    REG_F = SET_ZF(!z0 && (v == 0)) | SET_CF(x & 0x01);
    return v;
}

u8 gb_mu_SLA_x(u8 x)
{
    u8 v = x << 1;
    REG_F = SET_ZF(v == 0) | SET_CF(x & 0x80);
    return v;
}

u8 gb_mu_SWAP_x(u8 x)
{
    u8 v = (x >> 4) | (x << 4);
    REG_F = SET_ZF(v == 0);
    return v;
}

u8 gb_mu_SRA_x(u8 x)
{
    u8 v = (x >> 1) | (x & 0x80);
    REG_F = SET_ZF(v == 0) | SET_CF(x & 0x01);
    return v;
}

u8 gb_mu_SRL_x(u8 x)
{
    u8 v = x >> 1;
    REG_F = SET_ZF(v == 0) | SET_CF(x & 0x01);
    return v;
}

void gb_mu_BIT_b_x(u8 b, u8 x)
{
    REG_F = SET_ZF(!BIT_FIELD(x, b, 1)) | H_FLAG | (REG_F & C_FLAG);
}

u8 gb_mu_SET_b_x(u8 b, u8 x)
{
    return x | (1U << b);
}

u8 gb_mu_RES_b_x(u8 b, u8 x)
{
    return x & ~(1U << b);
}

//-----------------------------------------------------------------------------
// 8-bit Load Group
//-----------------------------------------------------------------------------

void gb_LD_r8_r8() { // LD r8,r8'
    R8_DST = R8_SRC;
    MCYCLES = 1;
}

void gb_LD_r8_imm8() { // LD r8,imm8
    R8_DST = gb_mu_ReadImm8();
    MCYCLES = 2;
}

void gb_LD_r8_HLp() { // LD r8,(HL)
    R8_DST = gb_ReadByte(HL.w);
    MCYCLES = 2;
}

void gb_LD_HLp_r8() { // LD (HL),r8
    gb_WriteByte(HL.w, R8_SRC);
    MCYCLES = 2;
}

void gb_LD_HLp_imm8() { // LD (HL),imm8
    gb_WriteByte(HL.w, gb_mu_ReadImm8());
    MCYCLES = 3;
}

void gb_LD_A_r16p() { // LD A,(BC)/(DE)
    REG_A = gb_ReadByte(R16_AF->w);
    MCYCLES = 2;
}

void gb_LD_A_imm16p() { // LD A,(imm16)
    REG_A = gb_ReadByte(gb_mu_ReadImm16());
    MCYCLES = 4;
}

void gb_LD_r16p_A() { // LD (BC)/(DE),A
    gb_WriteByte(R16_AF->w, REG_A);
    MCYCLES = 2;
}

void gb_LD_imm16p_A() { // LD (imm16),A
    gb_WriteByte(gb_mu_ReadImm16(), REG_A);
    MCYCLES = 4;
}

void gb_LDH_A_imm8p() { // LDH A,($FF00+imm8)
    REG_A = gb_ReadIO(gb_mu_ReadImm8());
    MCYCLES = 3;
}

void gb_LDH_imm8p_A() { // LDH ($FF00+imm8),A
    gb_WriteIO(gb_mu_ReadImm8(), REG_A);
    MCYCLES = 3;
}

void gb_LDH_A_Cp() { // LD A,($FF00+C)
    REG_A = gb_ReadIO(REG_C);
    MCYCLES = 2;
}

void gb_LDH_Cp_A() { // LD ($FF00+C),A
    gb_WriteIO(REG_C, REG_A);
    MCYCLES = 2;
}

void gb_LDI_HLp_A() { // LDI (HL),A
    gb_WriteByte(HL.w++, REG_A);
    MCYCLES = 2;
}

void gb_LDI_A_HLp() { // LDI A,(HL)
    REG_A = gb_ReadByte(HL.w++);
    MCYCLES = 2;
}

void gb_LDD_HLp_A() { // LDD (HL),A
    gb_WriteByte(HL.w--, REG_A);
    MCYCLES = 2;
}

void gb_LDD_A_HLp() { // LDD A,(HL)
    REG_A = gb_ReadByte(HL.w--);
    MCYCLES = 2;
}

//-----------------------------------------------------------------------------
// 16-bit Load Group
//-----------------------------------------------------------------------------

void gb_LD_r16_imm16() { // LD r16,imm16
    R16_SP->w = gb_mu_ReadImm16();
    MCYCLES = 3;
}

void gb_LD_imm16p_SP() { // LD (imm16),SP
    u16 addr = gb_mu_ReadImm16();
    gb_WriteByte(addr++, SP.b.lo);
    gb_WriteByte(addr++, SP.b.hi);
    MCYCLES = 5;
}

void gb_LD_SP_HL() { // LD SP,HL
    SP.w = HL.w;
    MCYCLES = 2;
}

void gb_PUSH_r16() { // PUSH r16
    gb_mu_PUSH(R16_AF->w);
    MCYCLES = 4;
}

void gb_POP_r16() { // POP r16
    R16_AF->w = gb_mu_POP();
    MCYCLES = 3;
}

//-----------------------------------------------------------------------------
// 8-bit Arithmetic Group
//-----------------------------------------------------------------------------

void gb_ADD_A_r8() { // ADD A,r8
    gb_mu_ADD_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_ADD_A_imm8() { // ADD A,imm8
    gb_mu_ADD_A_x(gb_mu_ReadImm8());
    MCYCLES = 2; 
}

void gb_ADD_A_HLp() { // ADD A,(HL)
    gb_mu_ADD_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;    
}

void gb_ADC_A_r8() {  // ADC A,r8
    gb_mu_ADC_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_ADC_A_imm8() { // ADC A,imm8
    gb_mu_ADC_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_ADC_A_HLp() { // ADC A,(HL)
    gb_mu_ADC_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_SUB_A_r8() { // SUB A,r8
    gb_mu_SUB_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_SUB_A_imm8() { // SUB A,imm8
    gb_mu_SUB_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_SUB_A_HLp() { // SUB A,(HL)
    gb_mu_SUB_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_SBC_A_r8() { // SBC A,r8
    gb_mu_SBC_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_SBC_A_imm8() { // SBC A,imm8
    gb_mu_SBC_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_SBC_A_HLp() { // SBC A,(HL)
    gb_mu_SBC_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_AND_A_r8() { // AND A,r8
    gb_mu_AND_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_AND_A_imm8() { // AND A,imm8
    gb_mu_AND_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_AND_A_HLp() { // AND A,(HL)
    gb_mu_AND_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_OR_A_r8() { // OR A,r8
    gb_mu_OR_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_OR_A_imm8() { // OR A,imm8
    gb_mu_OR_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_OR_A_HLp() { // OR A,(HL)
    gb_mu_OR_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_XOR_A_r8() { // XOR A,r8
    gb_mu_XOR_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_XOR_A_imm8() { // XOR A,imm8
    gb_mu_XOR_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_XOR_A_HLp() { // XOR A,(HL)
    gb_mu_XOR_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_CP_A_r8() { // CP A,r8
    gb_mu_CP_A_x(R8_SRC);
    MCYCLES = 1;
}

void gb_CP_A_imm8() { // CP A,imm8
    gb_mu_CP_A_x(gb_mu_ReadImm8());
    MCYCLES = 2;
}

void gb_CP_A_HLp() { // CP A,(HL)
    gb_mu_CP_A_x(gb_ReadByte(HL.w));
    MCYCLES = 2;
}

void gb_INC_r8() { // INC r8
    R8_DST = gb_mu_INC_x(R8_DST);
    MCYCLES = 1;
}

void gb_INC_HLp() { // INC (HL)
    gb_WriteByte(HL.w, gb_mu_INC_x(gb_ReadByte(HL.w)));
    MCYCLES = 3;
}

void gb_DEC_r8() { // DEC r8
    R8_DST = gb_mu_DEC_x(R8_DST);
    MCYCLES = 1;
}

void gb_DEC_HLp() { // DEC (HL)
    gb_WriteByte(HL.w, gb_mu_DEC_x(gb_ReadByte(HL.w)));
    MCYCLES = 3;
}

void gb_DAA() { // DAA
    v16 A;
    A.w = REG_A;
    if (((A.w & 0x0F) > 0x09) || (REG_F & H_FLAG)) { A.w += (REG_F & N_FLAG) ? 0xFA : 0x06; }
    if (((A.w & 0xF0) > 0x90) || (REG_F & C_FLAG)) { A.w += (REG_F & N_FLAG) ? 0xA0 : 0x60; }
    REG_A = A.b.lo;
    REG_F = SET_ZF(REG_A == 0) | (REG_F & N_FLAG) | SET_CF(A.b.hi);
    MCYCLES = 1;
}

void gb_CPL() { // CPL A
    REG_A = ~REG_A;
    REG_F = (REG_F & Z_FLAG) | N_FLAG | H_FLAG | (REG_F & C_FLAG);
    MCYCLES = 1;
}

//-----------------------------------------------------------------------------
// 16-Bit Arithmetic Group
//-----------------------------------------------------------------------------

void gb_ADD_HL_r16() { // ADD HL,r16
    HL.w = gb_mu_ADD_16(HL.w, R16_SP->w);
    MCYCLES = 2;
}

void gb_INC_r16() { // INC r16
    (R16_SP->w)++;
    MCYCLES = 2;
}

void gb_DEC_r16() { // DEC r16
    (R16_SP->w)--;
    MCYCLES = 2;
}

void gb_ADD_SP_imm8() { // ADD SP,imm8
    SP.w = gb_mu_ADD_16(SP.w, (s16)(s8)gb_mu_ReadImm8());
    MCYCLES = 4;
}

void gb_LDHL_SP_imm8() { // LD HL,SP+imm8
    HL.w = gb_mu_ADD_16(SP.w, (s16)(s8)gb_mu_ReadImm8());
    MCYCLES = 3;
}

//-----------------------------------------------------------------------------
// Rotate and Shift Group
//-----------------------------------------------------------------------------

void gb_RLCA() { // RLCA
    REG_A = gb_mu_RLC_x(REG_A, true);
    MCYCLES = 1;
}

void gb_RLC_r8() { // RLC r8
    R8_SRC = gb_mu_RLC_x(R8_SRC, false);
    MCYCLES = 2;
}

void gb_RLC_HLp() { // RLC (HL)
    gb_WriteByte(HL.w, gb_mu_RLC_x(gb_ReadByte(HL.w), false));
    MCYCLES = 4;
}

void gb_RLA() { // RLA
    REG_A = gb_mu_RL_x(REG_A, true);
    MCYCLES = 1;
}

void gb_RL_r8() { // RL r8
    R8_SRC = gb_mu_RL_x(R8_SRC, false);
    MCYCLES = 2;
}

void gb_RL_HLp() { // RL (HL)
    gb_WriteByte(HL.w, gb_mu_RL_x(gb_ReadByte(HL.w), false));
    MCYCLES = 4;
}

void gb_RRCA() { // RRCA
    REG_A = gb_mu_RRC_x(REG_A, true);
    MCYCLES = 1;
}

void gb_RRC_r8() { // RRC r8
    R8_SRC = gb_mu_RRC_x(R8_SRC, false);
    MCYCLES = 2;
}

void gb_RRC_HLp() { // RRC (HL)
    gb_WriteByte(HL.w, gb_mu_RRC_x(gb_ReadByte(HL.w), false));
    MCYCLES = 4;
}

void gb_RRA() { // RRA
    REG_A = gb_mu_RR_x(REG_A, true);
    MCYCLES = 1;
}

void gb_RR_r8() { // RR r8
    R8_SRC = gb_mu_RR_x(R8_SRC, false);
    MCYCLES = 2;
}

void gb_RR_HLp() { // RR (HL)
    gb_WriteByte(HL.w, gb_mu_RR_x(gb_ReadByte(HL.w), false));
    MCYCLES = 4;
}

void gb_SLA_r8() { // SLA r8
    R8_SRC = gb_mu_SLA_x(R8_SRC);
    MCYCLES = 2;
}

void gb_SLA_HLp() { // SLA (HL)
    gb_WriteByte(HL.w, gb_mu_SLA_x(gb_ReadByte(HL.w)));
    MCYCLES = 4;
}

void gb_SWAP_r8() { // SWAP r8
    R8_SRC = gb_mu_SWAP_x(R8_SRC);
    MCYCLES = 2;
}

void gb_SWAP_HLp() { // SWAP (HL)
    gb_WriteByte(HL.w, gb_mu_SWAP_x(gb_ReadByte(HL.w)));
    MCYCLES = 4;
}

void gb_SRA_r8() { // SRA r8
    R8_SRC = gb_mu_SRA_x(R8_SRC);
    MCYCLES = 2;
}

void gb_SRA_HLp() { // SRA (HL)
    gb_WriteByte(HL.w, gb_mu_SRA_x(gb_ReadByte(HL.w)));
    MCYCLES = 4;
}

void gb_SRL_r8() { // SRL r8
    R8_SRC = gb_mu_SRL_x(R8_SRC);
    MCYCLES = 2;
}

void gb_SRL_HLp() { // SRL (HL)
    gb_WriteByte(HL.w, gb_mu_SRL_x(gb_ReadByte(HL.w)));
    MCYCLES = 4;
}

//-----------------------------------------------------------------------------
// Bit Set, Reset, and Test Group
//-----------------------------------------------------------------------------

void gb_BIT_b_r8() { // BIT b,r8
    gb_mu_BIT_b_x(BIT_OP, R8_SRC);
    MCYCLES = 2;
}

void gb_BIT_b_HLp() { // BIT b,(HL)
    gb_mu_BIT_b_x(BIT_OP, gb_ReadByte(HL.w));
    MCYCLES = 3;
}

void gb_SET_b_r8() { // SET b,r8
    R8_SRC = gb_mu_SET_b_x(BIT_OP, R8_SRC);
    MCYCLES = 2;
}

void gb_SET_b_HLp() { // SET b,(HL)
    gb_WriteByte(HL.w, gb_mu_SET_b_x(BIT_OP, gb_ReadByte(HL.w)));
    MCYCLES = 3;
}

void gb_RES_b_r8() { // RES b,r8
    R8_SRC = gb_mu_RES_b_x(BIT_OP, R8_SRC);
    MCYCLES = 2;
}

void gb_RES_b_HLp() { // RES b,(HL)
    gb_WriteByte(HL.w, gb_mu_RES_b_x(BIT_OP, gb_ReadByte(HL.w)));
    MCYCLES = 3;
}

//-----------------------------------------------------------------------------
// CPU Control Group
//-----------------------------------------------------------------------------

void gb_CCF() { // CCF
    REG_F = (REG_F & Z_FLAG) | ((REG_F & C_FLAG) ^ C_FLAG);
    MCYCLES = 1;
}

void gb_SCF() { // SCF
    REG_F = (REG_F & Z_FLAG) | C_FLAG;
    MCYCLES = 1;
}

void gb_NOP() { // NOP
    MCYCLES = 1;
}

void gb_HALT() { // HALT
    HALT = IME;
    MCYCLES = 1;
}

void gb_STOP() { // STOP
    PC.w++;
    MCYCLES = 1;
    gb_CGBSetSpeed();
}

void gb_DI() { // DI
    IME = false;
    MCYCLES = 1;
}

void gb_EI() { // EI
    IME = true;
    MCYCLES = 1;
}

//-----------------------------------------------------------------------------
// Jump Group
//-----------------------------------------------------------------------------

void gb_JP_imm16() { // JP imm16
    PC.w = gb_mu_ReadImm16();
    MCYCLES = 4;
}

void gb_JP_HL() { // JP HL
    PC.w = HL.w;
    MCYCLES = 1;
}

void gb_JP_cc_imm16() { // JP cc,imm16
    u16 addr = gb_mu_ReadImm16();
    if (!gb_mu_TestCC()) { MCYCLES = 3; return; }
    PC.w = addr;
    MCYCLES = 4;
}

void gb_JR_imm8() { // JR imm8
    s8 ofs = gb_mu_ReadImm8();
    PC.w += ofs;
    MCYCLES = 3;
}

void gb_JR_cc_imm8() { // JR cc,imm8
    s8 ofs = gb_mu_ReadImm8();
    if (!gb_mu_TestCC()) { MCYCLES = 2; return; }
    PC.w += ofs;
    MCYCLES = 3;
}

void gb_CALL_imm16() { // CALL imm16
    u16 addr = gb_mu_ReadImm16();
    gb_mu_PUSH(PC.w);
    PC.w = addr;
    MCYCLES = 6;
}

void gb_CALL_cc_imm16() { // CALL cc,imm16
    u16 addr = gb_mu_ReadImm16();
    if (!gb_mu_TestCC()) { MCYCLES = 3; return; }
    gb_mu_PUSH(PC.w);
    PC.w = addr;
    MCYCLES = 6;
}

void gb_RET() { // RET
    PC.w = gb_mu_POP();
    MCYCLES = 4;
}

void gb_RET_cc() { // RET cc
    if (!gb_mu_TestCC()) { MCYCLES = 2; return; }
    PC.w = gb_mu_POP();
    MCYCLES = 5;
}

void gb_RETI() { // RETI
    PC.w = gb_mu_POP();
    IME = true;
    MCYCLES = 4;
}

void gb_RST() { // RST p
    gb_mu_PUSH(PC.w);
    PC.w = OPCODE & 0x38;
    MCYCLES = 4;
}

//-----------------------------------------------------------------------------
// Interrupts
//-----------------------------------------------------------------------------

void gb_EnterInterrupt()
{
    IME = false;
    gb_mu_PUSH(PC.w);
}

void gb_IRQ40h_VBL()
{
    gb_EnterInterrupt();
    gb_AcknowledgeInterrupt(IRQ_VBL);
    PC.w = 0x0040;
}

void gb_IRQ48h_LCD()
{
    gb_EnterInterrupt();
    gb_AcknowledgeInterrupt(IRQ_LCD);
    PC.w = 0x0048;
}

void gb_IRQ50h_TMR()
{
    gb_EnterInterrupt();
    gb_AcknowledgeInterrupt(IRQ_TMR);
    PC.w = 0x0050;
}

void gb_IRQ58h_LNK()
{
    gb_EnterInterrupt();
    gb_AcknowledgeInterrupt(IRQ_LNK);
    PC.w = 0x0058;
}

void gb_IRQ60h_PAD()
{
    gb_EnterInterrupt();
    gb_AcknowledgeInterrupt(IRQ_PAD);
    PC.w = 0x0060;
}

void gb_CheckInterrupts()
{
    u8 irq = gb_IRQsPending();
    if (!irq) { return; }
    HALT = false;
    if (!IME) { return; }
    if      (irq & IRQ_VBL) { gb_IRQ40h_VBL(); }
    else if (irq & IRQ_LCD) { gb_IRQ48h_LCD(); }
    else if (irq & IRQ_TMR) { gb_IRQ50h_TMR(); }
    else if (irq & IRQ_LNK) { gb_IRQ58h_LNK(); }
    else if (irq & IRQ_PAD) { gb_IRQ60h_PAD(); }
}

//-----------------------------------------------------------------------------
// Instruction Map
//-----------------------------------------------------------------------------

void gb_UnknownOpcode(bool CB)
{
    gb_UnknownOpcodeHandler(CB, OPCODE, PC.w, AF.w, BC.w, DE.w, HL.w, SP.w, IME);
}

void gb_UnknownOpcodeNP()
{
    gb_UnknownOpcode(false);
}

void gb_UnknownOpcodeCB()
{
    gb_UnknownOpcode(true);
}

void gb_BuildInterpreter()
{
    MapR8[0] = &REG_B; MapR8[1] = &REG_C;
    MapR8[2] = &REG_D; MapR8[3] = &REG_E;
    MapR8[4] = &REG_H; MapR8[5] = &REG_L;
    MapR8[6] = &REG_6; MapR8[7] = &REG_A;

    MapSP[0] =      MapAF[0] = &BC;
    MapSP[1] =      MapAF[1] = &DE;
    MapSP[2] =      MapAF[2] = &HL;
    MapSP[3] = &SP; MapAF[3] = &AF;

for (s32 i = 0; i < 256; i++)
{
    MapNP[i] = gb_UnknownOpcodeNP;
    MapCB[i] = gb_UnknownOpcodeCB;
}

    MapNP[0x36] = gb_LD_HLp_imm8;
    MapNP[0x0A] = gb_LD_A_r16p;
    MapNP[0x1A] = gb_LD_A_r16p;
    MapNP[0xFA] = gb_LD_A_imm16p; // MOVED
    MapNP[0x02] = gb_LD_r16p_A;
    MapNP[0x12] = gb_LD_r16p_A;
    MapNP[0xEA] = gb_LD_imm16p_A; // MOVED
    MapNP[0xF0] = gb_LDH_A_imm8p; // MOVED
    MapNP[0xE0] = gb_LDH_imm8p_A; // MOVED
    MapNP[0xF2] = gb_LDH_A_Cp; // MOVED
    MapNP[0xE2] = gb_LDH_Cp_A; // MOVED
    MapNP[0x22] = gb_LDI_HLp_A; // MOVED
    MapNP[0x2A] = gb_LDI_A_HLp; // MOVED
    MapNP[0x32] = gb_LDD_HLp_A; // MOVED
    MapNP[0x3A] = gb_LDD_A_HLp; // MOVED

    MapNP[0x08] = gb_LD_imm16p_SP; // MOVED
    MapNP[0xF9] = gb_LD_SP_HL;

    MapNP[0xC6] = gb_ADD_A_imm8;
    MapNP[0x86] = gb_ADD_A_HLp;
    MapNP[0xCE] = gb_ADC_A_imm8;
    MapNP[0x8E] = gb_ADC_A_HLp;
    MapNP[0xD6] = gb_SUB_A_imm8;
    MapNP[0x96] = gb_SUB_A_HLp;
    MapNP[0xDE] = gb_SBC_A_imm8;
    MapNP[0x9E] = gb_SBC_A_HLp;
    MapNP[0xE6] = gb_AND_A_imm8;
    MapNP[0xA6] = gb_AND_A_HLp;
    MapNP[0xEE] = gb_XOR_A_imm8;
    MapNP[0xAE] = gb_XOR_A_HLp;
    MapNP[0xF6] = gb_OR_A_imm8;
    MapNP[0xB6] = gb_OR_A_HLp;
    MapNP[0xFE] = gb_CP_A_imm8;
    MapNP[0xBE] = gb_CP_A_HLp;
    MapNP[0x34] = gb_INC_HLp;
    MapNP[0x35] = gb_DEC_HLp;
    MapNP[0x27] = gb_DAA;
    MapNP[0x2F] = gb_CPL;

    MapNP[0xE8] = gb_ADD_SP_imm8; // MOVED
    MapNP[0xF8] = gb_LDHL_SP_imm8; // MOVED

    MapNP[0x07] = gb_RLCA;
    MapNP[0x17] = gb_RLA;
    MapNP[0x0F] = gb_RRCA;
    MapNP[0x1F] = gb_RRA;
    MapCB[0x06] = gb_RLC_HLp;
    MapCB[0x16] = gb_RL_HLp;
    MapCB[0x0E] = gb_RRC_HLp;
    MapCB[0x1E] = gb_RR_HLp;
    MapCB[0x26] = gb_SLA_HLp;
    MapCB[0x36] = gb_SWAP_HLp; // MOVED
    MapCB[0x2E] = gb_SRA_HLp;
    MapCB[0x3E] = gb_SRL_HLp;

    MapNP[0x3F] = gb_CCF;
    MapNP[0x37] = gb_SCF;
    MapNP[0x00] = gb_NOP;
    MapNP[0x76] = gb_HALT;
    MapNP[0x10] = gb_STOP; // MOVED
    MapNP[0xF3] = gb_DI;
    MapNP[0xFB] = gb_EI;

    MapNP[0xC3] = gb_JP_imm16;
    MapNP[0xE9] = gb_JP_HL;
    MapNP[0x18] = gb_JR_imm8;
    MapNP[0xCD] = gb_CALL_imm16;
    MapNP[0xC9] = gb_RET;
    MapNP[0xD9] = gb_RETI; // MOVED

for (int r = 0; r < 8; ++r)
{
    if (r == 6) { continue; }
for (int b = 0; b < 8; ++b)
{              
    MapCB[0x40 | (b << 3) | r] = gb_BIT_b_r8;
    MapCB[0xC0 | (b << 3) | r] = gb_SET_b_r8;
    MapCB[0x80 | (b << 3) | r] = gb_RES_b_r8;

    if (b == 6) { continue; }

    MapNP[0x40 | (r << 3) | b] = gb_LD_r8_r8;
}
}

for (int r = 0; r < 8; ++r)
{
    MapCB[0x40 | (r << 3) | 0x06] = gb_BIT_b_HLp;
    MapCB[0xC0 | (r << 3) | 0x06] = gb_SET_b_HLp;
    MapCB[0x80 | (r << 3) | 0x06] = gb_RES_b_HLp;

    MapNP[0xC0 | (r << 3) | 0x07] = gb_RST;

    if (r == 6) { continue; }

    MapNP[       (r << 3) | 0x06] = gb_LD_r8_imm8;
    MapNP[0x40 | (r << 3) | 0x06] = gb_LD_r8_HLp;
    MapNP[       (r << 3) | 0x04] = gb_INC_r8;
    MapNP[       (r << 3) | 0x05] = gb_DEC_r8;

    MapNP[0x70 | r] = gb_LD_HLp_r8;
    MapNP[0x80 | r] = gb_ADD_A_r8;
    MapNP[0x88 | r] = gb_ADC_A_r8;
    MapNP[0x90 | r] = gb_SUB_A_r8;
    MapNP[0x98 | r] = gb_SBC_A_r8;
    MapNP[0xA0 | r] = gb_AND_A_r8;
    MapNP[0xB0 | r] = gb_OR_A_r8;
    MapNP[0xA8 | r] = gb_XOR_A_r8;
    MapNP[0xB8 | r] = gb_CP_A_r8;

    MapCB[       r] = gb_RLC_r8;
    MapCB[0x10 | r] = gb_RL_r8;
    MapCB[0x08 | r] = gb_RRC_r8;
    MapCB[0x18 | r] = gb_RR_r8;
    MapCB[0x20 | r] = gb_SLA_r8;
    MapCB[0x28 | r] = gb_SRA_r8;
    MapCB[0x30 | r] = gb_SWAP_r8;
    MapCB[0x38 | r] = gb_SRL_r8;
}

for (int rr = 0; rr < 4; ++rr)
{
    MapNP[        rr << 4  | 0x01] = gb_LD_r16_imm16;
    MapNP[0xC0 | (rr << 4) | 0x05] = gb_PUSH_r16;
    MapNP[0xC0 | (rr << 4) | 0x01] = gb_POP_r16;
    MapNP[       (rr << 4) | 0x09] = gb_ADD_HL_r16;
    MapNP[       (rr << 4) | 0x03] = gb_INC_r16;
    MapNP[       (rr << 4) | 0x0B] = gb_DEC_r16;
    MapNP[0xC0 | (rr << 3) | 0x02] = gb_JP_cc_imm16;
    MapNP[0x20 | (rr << 3)       ] = gb_JR_cc_imm8;
    MapNP[0xC0 | (rr << 3) | 0x04] = gb_CALL_cc_imm16;
    MapNP[0xC0 | (rr << 3) | 0x00] = gb_RET_cc;
}
}
