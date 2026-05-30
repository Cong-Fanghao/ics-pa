#include "common.h"
#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename)
{
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY,0,RAMDISK_SIZE);//PA3.1
  int fd = fs_open(filename, 0, 0);
  Log("filename=%s,fd=%d", filename, fd);
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); // 把文件整个读入内存DEFAULT_ENTRY处
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
