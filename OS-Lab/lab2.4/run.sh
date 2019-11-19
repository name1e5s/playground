#!/bin/bash

if [ -f "rootfs.ext2.gz" ]
then
	gzip -d rootfs.ext2.gz
fi

qemu-system-x86_64 -M pc -kernel bzImage  -drive file=rootfs.ext2,if=virtio,format=raw -append "rootwait root=/dev/vda" -net nic,model=virtio -net user
