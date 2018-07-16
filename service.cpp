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

#define IVSHMEM_VENDOR 0x11101af4

void scan_bus(const uint8_t classcode, const int bus)
{
  for (uint16_t device = 0; device < 255; ++device)
  {
    const uint16_t pci_addr = bus * 256 + device;
    const uint32_t id = hw::PCI_Device::read_dword(pci_addr, PCI::CONFIG_VENDOR);
    const uint16_t vendor = id & 0xffff;
    const uint16_t product = (id >> 16) & 0xffff;

    if (id == IVSHMEM_VENDOR)
    {
      hw::PCI_Device::class_revision_t devclass;
      devclass.reg = hw::PCI_Device::read_dword(pci_addr, PCI::CONFIG_CLASS_REV);
      printf("id: 0x%x, classcode: 0x%x, pci_addr:0x%x \n", id, devclass.classcode, pci_addr);
      printf("vendor: 0x%x, product: 0x%x\n", vendor, product);
    }
  }
}


void Service::start(const std::string& args)
{
  printf("Hello world - OS included!\n");
  printf("Args = %s\n", args.c_str());
  printf("Try giving the service less memory, eg. 5MB in vm.json\n");
  printf("Version: %s   arch: %s\n", OS::version().c_str(), OS::arch().c_str());
  scan_bus(PCI::OLD, 0);
}
