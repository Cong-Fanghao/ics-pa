#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  Elf_Ehdr elf_header;
  
  ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));

  uint8_t ph_buffer[elf_header.e_phentsize * elf_header.e_phnum];
  ramdisk_read(ph_buffer, elf_header.e_phoff, sizeof(ph_buffer));

  for (int j = 0; j < elf_header.e_phnum; ++j) {
    Elf_Phdr *segment = (Elf_Phdr *)(ph_buffer + j * elf_header.e_phentsize);

    if (segment->p_type == PT_LOAD) {
      ramdisk_read((void *)segment->p_vaddr, segment->p_offset, segment->p_filesz);
      if (segment->p_memsz > segment->p_filesz) {
        uint32_t bss_start = segment->p_vaddr + segment->p_filesz;
        uint32_t bss_size  = segment->p_memsz - segment->p_filesz;
        memset((void *)bss_start, 0, bss_size);
      }
    }
  }

  return (uintptr_t)DEFAULT_ENTRY;
}
