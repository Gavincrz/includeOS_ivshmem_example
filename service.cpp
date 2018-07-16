// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <service>
#include <cstdio>
#include <os>
#include <kernel/pci_manager.hpp>
#include <hw/devices.hpp>
#include <hw/pci_device.hpp>
#include <virtio/virtio.hpp>

#define IVSHMEM_VENDOR 0x11101af4

static uint16_t ivshmem_addr;
static hw::PCI_Device *pci_dev;
static uint16_t ivshmem_iobase;
static uint8_t current_cpu;

void init_ivshmem()
{
  // find the device
  const int bus = 0;
  for (uint16_t device = 0; device < 255; ++device)
  {
    const uint16_t pci_addr = bus * 256 + device;
    const uint32_t id = hw::PCI_Device::read_dword(pci_addr, PCI::CONFIG_VENDOR);
    const uint16_t vendor = id & 0xffff;
    const uint16_t product = (id >> 16) & 0xffff;

    if (id == IVSHMEM_VENDOR)
    {
      hw::PCI_Device::class_revision_t devclass;
      // print something for experiment
      devclass.reg = hw::PCI_Device::read_dword(pci_addr, PCI::CONFIG_CLASS_REV);
      printf("id: 0x%x, classcode: 0x%x, pci_addr:0x%x \n", id, devclass.classcode, pci_addr);
      printf("vendor: 0x%x, product: 0x%x\n", vendor, product);
      ivshmem_addr = pci_addr;

      // initialization
      pci_dev = new hw::PCI_Device(ivshmem_addr, IVSHMEM_VENDOR, devclass.reg);
      // find and store capabilities
      pci_dev->parse_capabilities();
      // find BARs etc.
      pci_dev->probe_resources();

      // initialize MSI-X if available
      if (pci_dev->msix_cap())
      {
        pci_dev->init_msix();
        uint8_t msix_vectors = pci_dev->get_msix_vectors();
        if (msix_vectors)
        {
          INFO2("[x] Device has %u MSI-X vectors", msix_vectors);
          current_cpu = SMP::cpu_id();
          INFO2("[x] Current_cpu: %d", current_cpu);

          // // setup all the MSI-X vectors
          // for (int i = 0; i < msix_vectors; i++)
          // {
          //   auto irq = Events::get().subscribe(nullptr);
          //   dev.setup_msix_vector(current_cpu, IRQ_BASE + irq);
          //   // store IRQ for later
          //   this->irqs.push_back(irq);
          // }
        }
        else
        INFO2("[ ] No MSI-X vectors");
      } else {
        INFO2("[ ] No MSI-X vectors");
      }



      delete pci_dev;
      return;
    }
  }
}




void Service::start(const std::string& args)
{
  printf("Hello world - OS included!\n");
  printf("Args = %s\n", args.c_str());
  init_ivshmem();
}
