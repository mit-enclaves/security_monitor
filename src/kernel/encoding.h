#ifndef RISCV_CSR_ENCODING_H
#define RISCV_CSR_ENCODING_H

#define MSTATUS_MPP         0x00001800
#define MSTATUS_MPRV        0x00020000
#define MSTATUS_MXR         0x00080000

#define PRV_U 0
#define PRV_S 1
#define PRV_H 2
#define PRV_M 3

#define CSR_FFLAGS 0x1
#define CSR_FRM 0x2
#define CSR_FCSR 0x3
#define CSR_CYCLE 0xc00
#define CSR_TIME 0xc01
#define CSR_INSTRET 0xc02

#define CSR_MHPMCOUNTER3 0xb03
#define CSR_MHPMCOUNTER4 0xb04
#define CSR_MHPMEVENT3 0x323
#define CSR_MHPMEVENT4 0x324

#define OPCODE_MASK   0x7F
#define OPCODE_SYSTEM 0xb1110011
#endif //RISCV_CSR_ENCODING_H
