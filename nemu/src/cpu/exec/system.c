#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();
void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  // TODO();
  t1 = id_dest->val;
  rtl_lm(&t0, &t1, 2);
  cpu.idtr.limit = t0;

  t1 = id_dest->val + 2;
  rtl_lm(&t0, &t1, 4);
  cpu.idtr.base = t0;

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  // TODO();
  uint8_t intr_no = id_dest->val;
  raise_intr(intr_no, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(int3) {
  raise_intr(3, decoding.seq_eip);
  print_asm("int3");
#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  // TODO();
  uint32_t ret_eip, cs, eflags;

  // 弹出 EIP、CS、EFLAGS
  ret_eip = vaddr_read(cpu.esp, 4);
  cpu.esp += 4;
  cs = vaddr_read(cpu.esp, 4);
  cpu.esp += 4;
  eflags = vaddr_read(cpu.esp, 4);
  cpu.esp += 4;

  // 恢复寄存器
  cpu.eip = ret_eip;
  cpu.cs = cs;
  cpu.eflags.val = eflags;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  // TODO();
  rtl_li(&t0, pio_read(id_src->val, id_dest->width));
  operand_write(id_dest, &t0);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  // TODO();
  rtl_sr(R_EAX, id_dest->width, &tzero);
  pio_write(id_dest->val, id_src->width, id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(cli) {
  cpu.eflags.IF = 0;
  print_asm("cli");
#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

