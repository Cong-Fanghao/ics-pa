#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);

  pcb[i].cur_brk = pcb[i].max_brk = 0;
}

static PCB *select_proc(void) {
  static int counter = 0;
  static PCB *last = NULL;
  
  PCB *choice = (counter++ % 1000 == 0) ? &pcb[1] : &pcb[0];
  
  if (choice != last) {
    Log("sched: %s -> %s", 
        last ? (last == &pcb[0] ? "pal" : "hello") : "none",
        choice == &pcb[0] ? "pal" : "hello");
    last = choice;
  }
  
  return choice;
}

_RegSet *schedule(_RegSet *prev) {
  if (current) current->tf = prev;
  
  current = select_proc();
  
  _switch(&current->as);
  return current->tf;
}
