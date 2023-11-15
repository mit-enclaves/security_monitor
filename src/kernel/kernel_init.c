#include "kernel.h"

long disabled_hart_mask;
uintptr_t mem_size;
volatile uint64_t* mtime;
volatile uint32_t* plic_priorities;
size_t plic_ndevs;
void* kernel_start;
void* kernel_end;

// Weirdly enough, bigger values seems to create an undefined behaviour...
#define T_MAX (UINT_MAX)

hls_t* hls_init(uintptr_t hart_id)
{
  hls_t* hls = OTHER_HLS(hart_id);
  memset(hls, 0, sizeof(*hls));
  return hls;
}

static void memory_init()
{
  mem_size = mem_size / PAGE_SIZE * PAGE_SIZE;
}

/*
static void plic_init()
{
  for (size_t i = 1; i <= plic_ndevs; i++)
    plic_priorities[i] = 1;
}
*/

static void hart_plic_init()
{
  // clear pending interrupts
  hls_t *hls = HLS();
  *hls->ipi = 0;
  *hls->timecmp = T_MAX;
  write_csr(mip, 0);
  assert(!(read_csr(mip) & MIP_MTIP));

  if (!plic_ndevs)
    return;

  size_t ie_words = (plic_ndevs + 8 * sizeof(uintptr_t) - 1) /
    (8 * sizeof(uintptr_t));
  for (size_t i = 0; i < ie_words; i++) {
    if (hls->plic_s_ie) {
      // Supervisor not always present
      hls->plic_s_ie[i] = UINT32_MAX;
    }
  }
  *hls->plic_m_thresh = 1;
  if (hls->plic_s_thresh) {
    // Supervisor not always present
    *hls->plic_s_thresh = 0;
  }
}

static void prci_test()
{
  hls_t *hls = HLS();
  assert(!(read_csr(mip) & MIP_MSIP));
  *hls->ipi = 1;
  assert(read_csr(mip) & MIP_MSIP);
  *hls->ipi = 0;
  assert(!(read_csr(mip) & MIP_MSIP));

  assert(!(read_csr(mip) & MIP_MTIP));
  *hls->timecmp = 0;
  assert(read_csr(mip) & MIP_MTIP);
  *hls->timecmp = T_MAX;
  assert(!(read_csr(mip) & MIP_MTIP));
}

// send S-mode interrupts and most exceptions straight to S-mode
static void delegate_traps()
{
  uintptr_t interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
  
     uintptr_t exceptions =
     (1U << CAUSE_MISALIGNED_FETCH) |
     (1U << CAUSE_FETCH_PAGE_FAULT) |
     (1U << CAUSE_BREAKPOINT) |
     (1U << CAUSE_LOAD_PAGE_FAULT) |
     (1U << CAUSE_STORE_PAGE_FAULT) |
     (1U << CAUSE_USER_ECALL);

  write_csr(mideleg, interrupts);
  write_csr(medeleg, exceptions);
  assert(read_csr(mideleg) == interrupts);
  assert(read_csr(medeleg) == exceptions);
}

void kernel_init(uintptr_t ftd_addr) {
  // Make sure mstatus and mie (software interrupts enabled) are initialized here
  
  hls_init(0);

  FDT_ADDR = ftd_addr;

  printm("query_mem\n");
  query_mem(FDT_ADDR);
  printm("query_harts\n");
  query_harts(FDT_ADDR);
  printm("query_clint\n");
  query_clint(FDT_ADDR);
  printm("query_plic\n");
  query_plic(FDT_ADDR);
  printm("query_chosen\n");
  query_chosen(FDT_ADDR);
  printm("query over\n");

  //plic_init();
  hart_plic_init();
  prci_test();
  memory_init();

  // Initialize the device tree
  filter_and_copy_device_tree();
}

void kernel_init_other_core(uintptr_t core_id) {
  // Make sure mstatus and mie (software interrupts enabled) are initialized here
  delegate_traps();
  hart_plic_init();
  prci_test();
}
