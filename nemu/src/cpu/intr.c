#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();
  cpu.eflags.val &= ~FL_IF;
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, cpu.eflags.val);
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, cpu.cs);
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, ret_addr);

  uint32_t idt_base = cpu.idtr.base;
  uint32_t entry = idt_base + NO * 8;

  uint32_t low = vaddr_read(entry, 4);
  uint16_t sel = low >> 16;
  uint16_t off_low = low & 0xffff;

  uint32_t high = vaddr_read(entry + 4, 4);
  uint16_t off_high = high & 0xffff;

  cpu.cs = sel;
  decoding.is_jmp = 1;
  decoding.jmp_eip = (off_high << 16) | off_low;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
