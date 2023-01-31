#include "kernel.h"
#include <platform.h>

void static truly_illegal_insn(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc, uintptr_t mstatus, insn_t insn) {
  printm("Truly illegal instruction with encoding %lx\n", insn);
  uintptr_t mtval = read_csr(mtval);
  bad_trap(mcause, mepc, mtval);
}

static inline int emulate_read_csr(int num, uintptr_t mstatus, uintptr_t* result) {
  uintptr_t counteren = -1;
  if (EXTRACT_FIELD(mstatus, MSTATUS_MPP) == PRV_U)
    counteren = read_csr(scounteren);

  switch (num)
  {
    case CSR_SATP:
      *result = read_csr(satp);
      return 0;
    case CSR_CYCLE:
      if (!((counteren >> (CSR_CYCLE - CSR_CYCLE)) & 1))
        return -1;
      *result = read_csr(mcycle);
      return 0;
    case CSR_TIME:
      if (!((counteren >> (CSR_TIME - CSR_CYCLE)) & 1))
        return -1;
      *result = *mtime;
      //*result = *mtime;
      return 0;
    case CSR_INSTRET:
      if (!((counteren >> (CSR_INSTRET - CSR_CYCLE)) & 1))
        return -1;
      *result = read_csr(minstret);
      return 0;
    case CSR_MHPMCOUNTER3:
      if (!((counteren >> (3 + CSR_MHPMCOUNTER3 - CSR_MHPMCOUNTER3)) & 1))
        return -1;
      *result = read_csr(mhpmcounter3);
      return 0;
    case CSR_MHPMCOUNTER4:
      if (!((counteren >> (3 + CSR_MHPMCOUNTER4 - CSR_MHPMCOUNTER3)) & 1))
        return -1;
      *result = read_csr(mhpmcounter4);
      return 0;
    case CSR_MHPMEVENT3:
      *result = read_csr(mhpmevent3);
      return 0;
    case CSR_MHPMEVENT4:
      *result = read_csr(mhpmevent4);
      return 0;
  }
  return -1;
}

static inline int emulate_write_csr(int num, uintptr_t value, uintptr_t mstatus) {
  switch (num) {
    case CSR_SATP:
        // Make sure the instruction comes from S mode
        assert((EXTRACT_FIELD(mstatus, MSTATUS_MPP) == PRV_S));
        if((value >> SATP_MODE_OFFSET) == SATP_MODE_SV39) {
          write_csr(satp, value);
        }
        return 0;
    case CSR_CYCLE: write_csr(mcycle, value); return 0;
    case CSR_INSTRET: write_csr(minstret, value); return 0;
    case CSR_MHPMCOUNTER3: write_csr(mhpmcounter3, value); return 0;
    case CSR_MHPMCOUNTER4: write_csr(mhpmcounter4, value); return 0;
    case CSR_MHPMEVENT3: write_csr(mhpmevent3, value); return 0;
    case CSR_MHPMEVENT4: write_csr(mhpmevent4, value); return 0;
  }
  return -1;
}

void static emulate_system_opcode(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc, uintptr_t mstatus, insn_t insn) {
  int rs1_num = (insn >> 15) & 0x1f;
  uintptr_t rs1_val = GET_RS1(insn, regs);
  uintptr_t rd = GET_RM(insn);
  int csr_num = (uint32_t)insn >> 20;
  uintptr_t csr_val, new_csr_val;

  uintptr_t funct3 = GET_FUNCT3(insn);
  uintptr_t funct7 = GET_FUNCT7(insn);

  if((funct3 == 0) && (rd == 0) && (funct7 == SFENCE_VMA_FUNCT7)) {
    // Make sure the instruction comes from S mode
    assert((EXTRACT_FIELD(mstatus, MSTATUS_MPP) == PRV_S));
    asm volatile ("sfence.vma");
    return;
  }
  
  if (emulate_read_csr(csr_num, mstatus, &csr_val))
    return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);

  int do_write = rs1_num;
  switch (rd)
  {
    case 0: return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);
    case 1: new_csr_val = rs1_val; do_write = 1; break;
    case 2: new_csr_val = csr_val | rs1_val; break;
    case 3: new_csr_val = csr_val & ~rs1_val; break;
    case 4: return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);
    case 5: new_csr_val = rs1_num; do_write = 1; break;
    case 6: new_csr_val = csr_val | rs1_num; break;
    case 7: new_csr_val = csr_val & ~rs1_num; break;
  }

  if (do_write && emulate_write_csr(csr_num, new_csr_val, mstatus))
    return truly_illegal_insn(regs, mcause, mepc, mstatus, insn);

  SET_RD(insn, regs, csr_val);
}

void illegal_instruction_trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc) {
  uintptr_t mstatus = read_csr(mstatus);
  insn_t insn = read_csr(mtval);
  
  if (insn == 0)
    insn = get_insn(mepc, &mstatus);

  //write_csr(mepc, mepc + 4);

  if (((insn & OPCODE_MASK) ^ OPCODE_SYSTEM) == 0) {
    emulate_system_opcode(regs, mcause, mepc, mstatus, insn);
  }
  else {
    truly_illegal_insn(regs, mcause, mepc, mstatus, insn);
  }
}
