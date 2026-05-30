#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

extern void raise_intr(uint8_t NO,vaddr_t ret_addr);

make_EHelper(lidt) {
// TODO();
  cpu.idtr.limit = id_dest->val;   // limit 已经被预加载到 val

  t1 = id_dest->addr + 2;         // base 字段的地址
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
  uint8_t NO=id_dest->val&0xff;
  raise_intr(NO,decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(int3)
{
  raise_intr(3, decoding.seq_eip);
  print_asm("int3");

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  // TODO();
  rtl_pop(&cpu.eip);        // 弹出 EIP（返回地址）
  
  rtl_pop(&t0);             // 弹出 CS（32位，但只取低16位）
  cpu.cs = t0;
  
  rtl_pop(&t1);             // 弹出 EFLAGS
  memcpy(&cpu.eflags, &t1, sizeof(cpu.eflags));  // union 类型必须用 memcpy

  decoding.is_jmp = 1;      // 设置跳转标志
  decoding.jmp_eip = cpu.eip; // 跳转目标为弹出的返回地址

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
