# includeOS_ivshmem_example
includeOS application which use ivshmem to interact with other VMs on qemu
## Run
Make sure you have IncludeOS properly installed.
```mkdir build
cd build
cmake ..
make
sudo --preserve-env qemu-system-x86_64 --enable-kvm -drive file=seed.img,format=raw,if=ide,media=disk -device ivshmem-doorbell,vectors=2,chardev=ivshmem -chardev socket,path=/tmp/ivshmem_socket,id=ivshmem -m 1024 -nographic 
```

Then the unikernel is running and waiting for the interrupt from other VM

Use other vm to trigger the interrupt, see [ivshmem_example](https://github.com/Gavincrz/ivshmem_example)
