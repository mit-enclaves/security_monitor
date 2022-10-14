#include "kernel.h"

long disabled_hart_mask;
uintptr_t mem_size;
volatile uint64_t* mtime;
volatile uint32_t* plic_priorities;
size_t plic_ndevs;
void* kernel_start;
void* kernel_end;

static void memory_init()
{
  mem_size = mem_size / PAGE_SIZE * PAGE_SIZE;
}

static void plic_init()
{
  for (size_t i = 1; i <= plic_ndevs; i++)
    plic_priorities[i] = 1;
}

static void hart_plic_init()
{
  // clear pending interrupts
  *HLS()->ipi = 0;
  //*HLS()->timecmp = -1ULL;
  write_csr(mip, 0);

  if (!plic_ndevs)
    return;

  size_t ie_words = (plic_ndevs + 8 * sizeof(uintptr_t) - 1) /
		(8 * sizeof(uintptr_t));
  for (size_t i = 0; i < ie_words; i++) {
     if (HLS()->plic_s_ie) {
        // Supervisor not always present
        HLS()->plic_s_ie[i] = ULONG_MAX;
     }
  }
  *HLS()->plic_m_thresh = 1;
  if (HLS()->plic_s_thresh) {
      // Supervisor not always present
      *HLS()->plic_s_thresh = 0;
  }
}

static void prci_test()
{
  assert(!(read_csr(mip) & MIP_MSIP));
  *HLS()->ipi = 1;
  assert(read_csr(mip) & MIP_MSIP);
  *HLS()->ipi = 0;
  assert(!(read_csr(mip) & MIP_MSIP));

  assert(!(read_csr(mip) & MIP_MTIP));
  *HLS()->timecmp = 0;
  assert(read_csr(mip) & MIP_MTIP);
  *HLS()->timecmp = -1ULL;
  assert(!(read_csr(mip) & MIP_MTIP));
}

// send S-mode interrupts and most exceptions straight to S-mode
static void delegate_traps()
{
  uintptr_t interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;

  /*
  uintptr_t exceptions =
    (1U << CAUSE_MISALIGNED_FETCH) |
    (1U << CAUSE_FETCH_PAGE_FAULT) |
    (1U << CAUSE_BREAKPOINT) |
    (1U << CAUSE_LOAD_PAGE_FAULT) |
    (1U << CAUSE_STORE_PAGE_FAULT) |
    (1U << CAUSE_USER_ECALL);
  */

  write_csr(mideleg, interrupts);
  //write_csr(medeleg, exceptions);
  assert(read_csr(mideleg) == interrupts);
  //assert(read_csr(medeleg) == exceptions);
}

void kernel_init(uintptr_t ftd_addr) {
  delegate_traps();
  hls_init(0);
  
  FDT_ADDR = ftd_addr;

  query_mem(FDT_ADDR);
  query_harts(FDT_ADDR);
  query_clint(FDT_ADDR);
  query_plic(FDT_ADDR);
  query_chosen(FDT_ADDR);

  plic_init();
  hart_plic_init();
  prci_test();
  memory_init();

  // Initialize the device tree
  filter_and_copy_device_tree();
}
