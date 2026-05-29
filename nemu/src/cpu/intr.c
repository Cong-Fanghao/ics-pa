#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  rtlreg_t temp_eflags = cpu.eflags.val;
  rtl_push(&temp_eflags);
  rtlreg_t temp_cs = cpu.cs;
  rtl_push(&temp_cs);
  rtlreg_t temp_eip = ret_addr;
  rtl_push(&temp_eip);

  //在IDT中找到中断号NO对应的门描述符首地址
  vaddr_t gate_addr=cpu.idtr.base+NO*sizeof(GateDesc);

  //根据门描述符的offset计算目标地址，由两部分拼接而成
  uint32_t off_15_0=vaddr_read(gate_addr,2);
  uint32_t off_32_16=vaddr_read(gate_addr+sizeof(GateDesc)-2,2);
  uint32_t target_addr=(off_32_16<<16)+off_15_0;

  decoding.is_jmp=1;
  decoding.jmp_eip=target_addr;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
