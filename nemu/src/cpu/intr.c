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
  uint32_t entry_addr = idt_base + NO * 8;

  uint32_t low = vaddr_read(entry_addr, 4);
  uint16_t offset_low = low & 0xFFFF;
  uint16_t selector = low >> 16;

  uint32_t high = vaddr_read(entry_addr + 4, 4);
  uint16_t offset_high = high & 0xFFFF;

  uint32_t offset = (offset_high << 16) | offset_low;

  // 更新 CS 和 EIP，跳转到中断处理函数
  cpu.cs = selector;
  cpu.eip = offset;
}

void dev_raise_intr() {
  raise_intr(0x20, cpu.eip);
}
