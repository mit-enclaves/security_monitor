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

static void hart_plic_init(int hart)
{
  // clear pending interrupts
  *OTHER_HLS(hart)->ipi = 0;
  //*OTHER_HLS(hart)->timecmp = -1ULL;
  write_csr(mip, 0);

  if (!plic_ndevs)
    return;

  size_t ie_words = (plic_ndevs + 8 * sizeof(uintptr_t) - 1) /
    (8 * sizeof(uintptr_t));
  for (size_t i = 0; i < ie_words; i++) {
    if (OTHER_HLS(hart)->plic_s_ie) {
      // Supervisor not always present
      OTHER_HLS(hart)->plic_s_ie[i] = ULONG_MAX;
    }
  }
  *OTHER_HLS(hart)->plic_m_thresh = 1;
  if (OTHER_HLS(hart)->plic_s_thresh) {
    // Supervisor not always present
    *OTHER_HLS(hart)->plic_s_thresh = 0;
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

  FDT_ADDR = ftd_addr;

  //printm("query_mem\n");
  //query_mem(FDT_ADDR);
  printm("query_harts\n");
  query_harts(FDT_ADDR);
  printm("query_clint\n");
  query_clint(FDT_ADDR);
  printm("query_plic\n");
  query_plic(FDT_ADDR);
  printm("query_chosen\n");
  query_chosen(FDT_ADDR);
  printm("query over\n");

  /*
  plic_init();
  for(int hart = 0; hart < MAX_HARTS; ++hart) {
    hart_plic_init(hart);
  }
  */
  //prci_test();
  memory_init();

  // Initialize the device tree
  filter_and_copy_device_tree();
}
