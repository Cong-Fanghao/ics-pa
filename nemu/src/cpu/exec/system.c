#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  // TODO();
  uint16_t limit = vaddr_read(id_dest->val, 2);
  uint32_t base = vaddr_read(id_dest->val + 2, 4);
  
  cpu.idtr.limit = limit;
  cpu.idtr.base = base;

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

make_EHelper(iret) {
  // TODO();
  uint32_t eip, cs, eflags;
  cpu_pop(&cpu, &eip);
  cpu_pop(&cpu, &cs);
  cpu_pop(&cpu, &eflags);
  
  cpu.eip = eip;
  cpu.cs = cs;
  cpu.eflags.val = eflags;
  
  cpu.eflags.IF = 1;

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
