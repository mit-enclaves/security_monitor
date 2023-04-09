[1mdiff --git a/src/init/init.S b/src/init/init.S[m
[1mindex a0f4fb4..8613bc3 100644[m
[1m--- a/src/init/init.S[m
[1m+++ b/src/init/init.S[m
[36m@@ -23,6 +23,17 @@[m [mli t1, MENTRY_FRAME_SIZE[m
 sub sp, sp, t0    # sp = stack_ptr - (mhartid*STACK_SIZE)[m
 sub sp, sp, t1    # sp = stack_ptr - (mhartid*STACK_SIZE) - MENTRY_FRAME_SIZE[m
 [m
[32m+[m[32m# core 0 zero out bss region[m
[32m+[m[32mcsrr t0, mhartid[m
[32m+[m[32mbne t0, zero, 2f[m
[32m+[m[32mla t0, _bss_start[m
[32m+[m[32mla t1, _bss_end[m
[32m+[m[32mbeq t0, t1, 2f[m
[32m+[m[32m1:sd zero, 0(t0)[m
[32m+[m[32maddi t0, t0, REGBYTES[m
[32m+[m[32mbne t0, t1, 1b[m
[32m+[m[32m2:[m
[32m+[m
 # Run SM's high-level initialization routine; safe for all cores to run[m
 # call into sm_init(uintptr_t fdt_addr)[m
 call sm_init[m
[1mdiff --git a/src/kernel/fdt.c b/src/kernel/fdt.c[m
[1mindex fef7fe4..2756ca1 100644[m
[1m--- a/src/kernel/fdt.c[m
[1m+++ b/src/kernel/fdt.c[m
[36m@@ -18,7 +18,7 @@[m [mvoid filter_and_copy_device_tree(void) {[m
   filter_plic(out_fdt_addr);[m
   //filter_compat(out_fdt_addr, "riscv,clint0");[m
   //filter_compat(out_fdt_addr, "riscv,debug-013");[m
[31m-  //fdt_print(src_fdt_addr);[m
[32m+[m[32m  fdt_print(src_fdt_addr);[m
   //printm("After Filtering");[m
   //fdt_print(out_fdt_addr);[m
 }[m
[1mdiff --git a/src/kernel/kernel_init.c b/src/kernel/kernel_init.c[m
[1mindex 66da622..01ca0c8 100644[m
[1m--- a/src/kernel/kernel_init.c[m
[1m+++ b/src/kernel/kernel_init.c[m
[36m@@ -40,6 +40,9 @@[m [mstatic void hart_plic_init()[m
   write_csr(mip, 0);[m
   assert(!(read_csr(mip) & MIP_MTIP));[m
 [m
[32m+[m[32m  printm("HLS is at address %lx\n", hls);[m
[32m+[m[32m  printm("plic_ndevs is at address %lx and has value %d\n", &plic_ndevs, plic_ndevs);[m
[32m+[m
   if (!plic_ndevs)[m
     return;[m
 [m
[1mdiff --git a/src/sm.lds.in b/src/sm.lds.in[m
[1mindex 7d0943d..7f6e336 100644[m
[1m--- a/src/sm.lds.in[m
[1m+++ b/src/sm.lds.in[m
[36m@@ -37,10 +37,15 @@[m [mSECTIONS[m
     *(.data)  /* initialized data */[m
     *(.data.*)[m
     *(.data.stacks)[m
[32m+[m[32m    *(.sdata)[m
[32m+[m[32m    *(.sdata.*)[m
   } > SM =0x0[m
 [m
[32m+[m[32m  _bss_start = .;[m
   .bss ALIGN(64)  : {[m
     *(.bss)   /* uninitialized data */[m
     *(.bss.*)[m
[32m+[m[32m    *(.sbss*)[m
   } > SM =0x0[m
[32m+[m[32m  _bss_end = .;[m
 }[m
