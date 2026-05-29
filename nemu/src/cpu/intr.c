#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  cpu.eflags.val &= ~FL_IF;

  /* 压栈 EFLAGS, CS, EIP */
  cpu.esp -= 4;  vaddr_write(cpu.esp, 4, cpu.eflags.val);
  cpu.esp -= 4;  vaddr_write(cpu.esp, 4, cpu.cs);
  cpu.esp -= 4;  vaddr_write(cpu.esp, 4, ret_addr);

  /* 从 IDT 取中断门 */
  uint32_t entry = cpu.idtr.base + NO * 8;

  uint32_t low  = vaddr_read(entry,     4);
  uint32_t high = vaddr_read(entry + 4, 4);

  uint16_t off_low  = low  & 0xffff;  // ✓
  uint16_t sel      = low  >> 16;      // ✓
  uint16_t off_high = high >> 16;      // ✅ 修正：是 >>16 不是 &0xffff

  cpu.cs = sel;
  decoding.is_jmp = 1;
  decoding.jmp_eip = ((uint32_t)off_high << 16) | off_low;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
