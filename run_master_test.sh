#!/bin/bash

cd /mnt/efs/fs1/riscy-OOO
source ./setup.sh

cd /mnt/efs/fs1/riscy-OOO/procs/build/RV64G_OOO.core_2.core_SMALL.cache_LARGE.tso.l1_cache_lru.secure_flush.check_deadlock.shmem_refill/awsf1
#cd /mnt/efs/fs1/riscy-OOO/procs/build/RV64G_OOO.core_2.core_SMALL.cache_LARGE.tso.l1_cache_lru.secure_flush.check_deadlock.shmem_refill.no_llc_part/awsf1
#cd /mnt/efs/fs1/riscy-OOO/procs/build/RV64G_OOO.core_2.core_SMALL.cache_LARGE.tso.l1_cache_lru.secure_flush.check_deadlock.shmem_refill.no_llc_part.no_L1_bypass/awsf1
for i in {1} 
do
  sudo fpga-load-local-image -S 0 -I agfi-0445ec30d794d2a6a
  #sudo fpga-load-local-image -S 0 -I agfi-097b793816851114b
  #sudo fpga-load-local-image -S 0 -I agfi-0acf29c9efb44e820 
  
  bin/ubuntu.exe --core-num 2  --mem-size 2048 --ignore-user-stucks 1000000 --rom /mnt/efs/fs1/riscy-OOO/procs/rom/out/rom_core_2 --elf /mnt/efs/fs1/security_monitor/build/master_test.elf
done
