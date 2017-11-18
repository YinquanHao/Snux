#ifndef __PCI_H_
#define __PCI_H_
#include <sys/ahci.h>
uint16_t pciReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t getVendorId(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func);
void checkAll();
uint8_t getClass(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getSubClass(uint8_t bus, uint8_t slot, uint8_t func);
void checkDevice(uint8_t bus, uint8_t device);
uint32_t pciReadLong (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t getBar5(uint8_t bus, uint8_t slot, uint8_t func);

void probe_port(hba_mem_t *abar);
void start_cmd(hba_port_t *port);
void stop_cmd(hba_port_t *port);
void port_rebase(hba_port_t *port, int portno);
int find_cmdslot(hba_port_t *port);
int write_ahci(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, char* buf);
int read_ahci(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, char* buf);

#endif