#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();
  memcpy(&t1, &cpu.eflags, sizeof(cpu.eflags));
  rtl_li(&t0, t1);
  rtl_push(&t0);

  /* 2. 压栈 CS（必须保证是 32 位宽度） */
  t0 = cpu.cs;          // 先扩展到 32 位临时变量
  rtl_push(&t0);

  /* 3. 压栈返回地址 EIP */
  rtl_li(&t0, ret_addr);
  rtl_push(&t0);

  /* 4. 计算门描述符地址并做严格边界检查 */
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  Log("NO=0x%x, gate_addr=0x%x, idtr.base=0x%x, idtr.limit=0x%x", 
      NO, gate_addr, cpu.idtr.base, cpu.idtr.limit);
  
  // 严格检查：整个门描述符（8字节）必须在 IDT 范围内
  assert(gate_addr + sizeof(GateDesc) - 1 <= cpu.idtr.base + cpu.idtr.limit);

  /* 5. 读取门描述符中的 32 位 offset */
  uint32_t off_15_0  = vaddr_read(gate_addr, 2);
  uint32_t off_32_16 = vaddr_read(gate_addr + sizeof(GateDesc) - 2, 2);
  uint32_t target_addr = (off_32_16 << 16) | off_15_0;

  // ****** 最关键的诊断日志 ******
  Log("off_15_0=0x%x, off_32_16=0x%x, target_addr=0x%x", 
      off_15_0, off_32_16, target_addr);

  decoding.is_jmp = 1;
  decoding.jmp_eip = target_addr;
}

void dev_raise_intr() {
  if (cpu.eflags.IF) {

    }
}

