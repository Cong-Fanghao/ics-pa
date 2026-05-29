#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();
  // 压入 EFLAGS、CS、EIP
  cpu.eflags.val &= ~FL_IF;
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, cpu.eflags.val);
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, cpu.cs);
  cpu.esp -= 4;
  vaddr_write(cpu.esp, 4, ret_addr);

  // 从 IDT 中获取中断处理函数地址
  uint32_t idt_base = cpu.idtr.base;
  uint32_t entry = idt_base + NO * 8;

  uint32_t low = vaddr_read(entry, 4);
  uint16_t sel = low >> 16;
  uint16_t off_low = low & 0xffff;

  uint32_t high = vaddr_read(entry + 4, 4);
  uint16_t off_high = high & 0xffff;

  // 更新 CS 和 EIP，跳转到中断处理函数
  cpu.cs = sel;
  cpu.eip = (off_high << 16) | off_low;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
