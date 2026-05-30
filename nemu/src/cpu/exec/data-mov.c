#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop(&t2);
  operand_write(id_dest, &t2);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  // TODO();
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_sext(&cpu.edx, &cpu.eax, 2);
  }
  else {
    // TODO();
    rtl_msb(&t0, &cpu.eax, 4);
    rtl_sub(&cpu.edx, &tzero, &t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}

make_EHelper(xchg)
{
  rtl_mv(&t0, &id_src->val);
  rtl_mv(&t1, &id_dest->val);
  operand_write(id_dest, &t0);
  operand_write(id_src, &t1);
  print_asm_template2(xchg);
}

make_EHelper(movs)
{
  vaddr_t src = cpu.esi;
  vaddr_t dest = cpu.edi;

  if (id_dest->width == 1)
  {
    uint8_t data = vaddr_read(src, 1);
    vaddr_write(dest, 1, data);
  }
  else if (id_dest->width == 2)
  {
    uint16_t data = vaddr_read(src, 2);
    vaddr_write(dest, 2, data);
  }
  else
  {
    uint32_t data = vaddr_read(src, 4);
    vaddr_write(dest, 4, data);
  }

  if (cpu.eflags.DF == 0)
  {
    cpu.esi += id_dest->width;
    cpu.edi += id_dest->width;
  }
  else
  {
    cpu.esi -= id_dest->width;
    cpu.edi -= id_dest->width;
  }

  print_asm("movs");
}

make_EHelper(bsr)
{
  uint32_t val = id_src->val;
  int i;
  for (i = 31; i >= 0; i--)
  {
    if (val & (1 << i))
    {
      t0 = i;
      break;
    }
  }
  operand_write(id_dest, &t0);
  print_asm_template2(bsr);
}

make_EHelper(rol)
{
  rtl_shl(&t0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t0);
  print_asm_template2(rol);
}

make_EHelper(ror)
{
  rtl_shr(&t0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t0);
  print_asm_template2(ror);
}

make_EHelper(rcl)
{
  rtl_shl(&t0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t0);
  print_asm_template2(rcl);
}

make_EHelper(rcr)
{
  rtl_shr(&t0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t0);
  print_asm_template2(rcr);
}
