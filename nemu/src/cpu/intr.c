#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  // 保存 EFLAGS
  rtlreg_t temp_eflags = cpu.eflags.val;
  rtl_push(&temp_eflags);
  
  // 保存 CS（转换为 32 位）
  rtlreg_t temp_cs = cpu.cs;
  rtl_push(&temp_cs);
  
  // 保存 EIP
  rtlreg_t temp_eip = ret_addr;
  rtl_push(&temp_eip);

  // 在 IDT 中找到中断号 NO 对应的门描述符首地址
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  Log("gate_addr=%d, cpu.idtr.base=%d, cpu.idtr.limit=%d", 
      gate_addr, cpu.idtr.base, cpu.idtr.limit);
  assert(gate_addr <= cpu.idtr.base + cpu.idtr.limit);

  // 根据门描述符的 offset 计算目标地址
  uint32_t off_15_0 = vaddr_read(gate_addr, 2);
  uint32_t off_32_16 = vaddr_read(gate_addr + sizeof(GateDesc) - 2, 2);
  uint32_t target_addr = (off_32_16 << 16) | off_15_0;

  decoding.is_jmp = 1;
  decoding.jmp_eip = target_addr;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
