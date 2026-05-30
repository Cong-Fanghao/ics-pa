#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void *new_page(void);

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  int size = fs_filesz(fd);
  int page_num = (size + PGSIZE - 1) / PGSIZE;
  
  void *pa = NULL;
  void *va = (void *)DEFAULT_ENTRY;
  
  for (int i = 0; i < page_num; i++) {
    pa = new_page();     
    _map(as, va, pa);              
    fs_read(fd, pa, PGSIZE);        
    va += PGSIZE;
  }
  
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
