#!/bin/bash
make -j8 -C `pwd` M=`pwd`/arch/x86/kvm modules
cd arch/x86/kvm
rmmod kvm-intel.ko
rmmod kvm.ko
insmod kvm.ko
insmod kvm-intel.ko
