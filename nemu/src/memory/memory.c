#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PTE_ADDR(pte) ((uint32_t)(pte) & ~0xfff)
#define PDX(va) (((uint32_t)(va) >> 22) & 0x3ff)
#define PTX(va) (((uint32_t)(va) >> 12) & 0x3ff)
#define OFF(va) ((uint32_t)(va) & 0xfff)

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if (map_NO != -1) {
    return mmio_read(addr, len, map_NO);
  }
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if (map_NO != -1) {
    mmio_write(addr, len, data, map_NO);
    return;
  }
  memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1))
  {
    int len1 = 0x1000 - OFF(addr);
    int len2 = len - len1;

    paddr_t paddr1 = page_translate(addr, false);
    paddr_t paddr2 = page_translate(addr + len1, false);

    uint32_t low = paddr_read(paddr1, len1);
    uint32_t high = paddr_read(paddr2, len2);

    return (high << (8 * len1)) | low;
  }
  else
  {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1))
  {
    int len1 = 0x1000 - OFF(addr);
    int len2 = len - len1;

    paddr_t paddr1 = page_translate(addr, true);
    paddr_t paddr2 = page_translate(addr + len1, true);

    uint32_t low = data & (~0u >> ((4 - len1) << 3));
    uint32_t high = data >> ((4 - len2) * 8);

    paddr_write(paddr1, len1, low);
    paddr_write(paddr2, len2, high);
    return;
  }
  else
  {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
}

paddr_t page_translate(vaddr_t addr, bool iswrite)
{
  CR0 cr0 = (CR0)cpu.CR0;

  if (cr0.paging && cr0.protect_enable)
  {
    CR3 cr3 = (CR3)cpu.CR3;

    PDE *pgdirs = (PDE *)PTE_ADDR(cr3.val);
    PDE pde = (PDE)paddr_read((uint32_t)(pgdirs + PDX(addr)), 4);
    assert(pde.present);

    PTE *ptab = (PTE *)PTE_ADDR(pde.val);
    PTE pte = (PTE)paddr_read((uint32_t)(ptab + PTX(addr)), 4);
    assert(pte.present);

    pde.accessed = 1;
    pte.accessed = 1;
    if (iswrite)
      pte.dirty = 1;

    paddr_write((uint32_t)(pgdirs + PDX(addr)), 4, pde.val);
    paddr_write((uint32_t)(ptab + PTX(addr)), 4, pte.val);

    paddr_t paddr = PTE_ADDR(pte.val) | OFF(addr);
    return paddr;
  }

  return addr; 
}