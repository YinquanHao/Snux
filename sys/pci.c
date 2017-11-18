#include <sys/kprintf.h>
#include <sys/io.h>
#include <sys/defs.h>
#include <sys/pci.h>
#include <sys/ahci.h>


#define SATA_SIG_ATA    0x00000101
#define SATA_SIG_ATAPI  0xEB140101
#define SATA_SIG_SEMB   0xC33C0101
#define SATA_SIG_PM 0x96690101
#define AHCI_DEV_NULL 0
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1

hba_mem_t *abar;
int SATA_PORT_NUM = -1;


uint16_t pciReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t getVendorId(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func);
void checkAll();
uint8_t getClass(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getSubClass(uint8_t bus, uint8_t slot, uint8_t func);
void checkDevice(uint8_t bus, uint8_t device);
uint32_t pciReadLong (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t getBar5(uint8_t bus, uint8_t slot, uint8_t func);

int check_type(hba_port_t *port);


uint16_t pciReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    outl(0xCF8, address);
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
 }

 uint32_t pciReadLong (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
 	uint32_t address;
 	uint32_t lbus = (uint32_t)bus;
 	uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    outl(0xCF8, address);

    //uint32_t bs = inl(0xCFC);
    uint32_t ms = /*bs &*/ 0xA4000;
    outl(0xCFC,ms);
    tmp = (uint32_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffffffff);
    return (tmp);
 }


 uint16_t getVendorId(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t res = pciReadWord(bus,slot,func,0);
 	return res;
 }

uint16_t getDeviceId(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t res = pciReadWord(bus,slot,func,2);
 	return res;
 }

 void checkAll(){
 	for(uint16_t bus = 0; bus < 256; bus++) {
         for(uint16_t slot=0; slot < 32; slot++) {
             checkDevice((uint8_t)bus,(uint8_t) slot);
         }
     }
 }


 uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t temp = pciReadWord(bus,slot,func,0x0E);
 	uint8_t res = (uint8_t)(temp & 0x00FF);
 	return res;
 }


 uint8_t getClass(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t temp = pciReadWord(bus,slot,func,0x0A);
 	temp = (temp & 0xFFFF)>>8;
 	uint8_t res = (uint8_t)(temp);
 	return res;
 }

 uint8_t getSubClass(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t temp = pciReadWord(bus,slot,func,0x0A);
 	uint8_t res = (uint8_t)(temp & 0x00FF);
 	return res;
 }

 uint32_t getBar5(uint8_t bus, uint8_t slot, uint8_t func){
 	uint32_t res = pciReadLong(bus,slot,func,0x24);
 	return res;
 }


 uint8_t getSecondaryBus(uint8_t bus, uint8_t slot, uint8_t func){
 	uint16_t temp = pciReadWord(bus,slot,func,0x18);
 	uint8_t res = (uint8_t)((temp & 0xFFFF)>>8);
 	return res;
 }



 void checkBus(uint8_t bus) {
     uint8_t device;
 	for(device = 0; device < 32; device++) {
        checkDevice(bus, device);
    }
 }

/*
 void checkDevice(uint8_t bus, uint8_t device) {
     uint8_t func = 0;
     uint8_t class = getClass(bus, device, func);
     uint8_t subClass = getSubClass(bus, device, func);
     if( (class == 0x01) && (subClass== 0x06)) {
     	kprintf("vendorID: %x, device: %d, bar5: %x ", getVendorId(bus, device, func), getDeviceId(bus,device,func), getBar5(bus,device,func));
     	uint32_t address = getBar5(bus,device,func);
     	uint32_t a = address;
     	uint32_t *add = &a;
     	abar = (hba_mem_t*)(uint64_t)(*add);
     	probe_port(abar);
     }
 }
*/
 void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
     uint8_t class;
     uint8_t subClass;
     //uint8_t secondaryBus;
 
     class = getClass(bus, device, function);
     subClass = getSubClass(bus, device, function);
    // kprintf(" class: %x, subclass: %x ", class, subClass);
     if( (class == 0x01) && (subClass== 0x06)) {
     	kprintf("vendorID: %x, device: %d, bar5: %x ", getVendorId(bus, device, function), getDeviceId(bus,device,function), getBar5(bus,device,function));
     	uint32_t address = getBar5(bus,device,function);
     	uint32_t a = address;
     	uint32_t *add = &a;
     	abar = (hba_mem_t*)(uint64_t)(*add);
     	probe_port(abar);
     }else if((class == 0x06) && (subClass == 0x04)){
         //secondaryBus = getSecondaryBus(bus, device, function);
        checkBus(getSecondaryBus(bus, device, function));
     }


 }


 void checkDevice(uint8_t bus, uint8_t device) {
     uint8_t function = 0;
 
     uint16_t vendorID = getVendorId(bus, device, function);
     if(vendorID == 0xFFFF) return;        
     //kprintf("getVendorId %x", vendorID);
     checkFunction(bus, device, function);
     uint8_t headerType = getHeaderType(bus, device, function);
     if( (headerType & 0x80) != 0) {
         for(function = 1; function < 8; function++) {
             if(getVendorId(bus, device, function) != 0xFFFF) {
                 checkFunction(bus, device, function);
             }
         }
     }
 }



 void probe_port(hba_mem_t *abar){
	uint32_t pi = abar->pi;
	kprintf("pi:%x",pi);	
	int i = 0;
	while (i<32){
		if (pi & 1){	
			hba_port_t* temp = &(abar->ports[i]);


			port_rebase(temp,i);


			int dt = check_type(temp);
			if (dt == AHCI_DEV_SATA){
				kprintf("SATA drive found at port %d\n", i);
				SATA_PORT_NUM = i;
				//port_rebase(temp,i);
				if(i==1){
					char * buffer_1 = (char*)0x350000;
					int start=0;
					for(int i=0;i<100;i++){

						char * read_buffer = (char *)0x150000;
						for(int j=0;j<4000;j++){
							*(read_buffer+j) = i;

						}
						write_ahci(temp,start,0,8,read_buffer);
						start=start+8;
					}

					read_ahci(temp,793,0,800,buffer_1);
					for(int i=0;i<1028;i++){
						kprintf("%d",*(buffer_1+i));
					}
					kprintf("\n ");
				}

			}else if (dt == AHCI_DEV_SATAPI){
				kprintf("SATAPI drive found at port %d\n", i);
			}else if (dt == AHCI_DEV_SEMB){
				kprintf("SEMB drive found at port %d\n", i);
			}else if (dt == AHCI_DEV_PM){
				kprintf("PM drive found at port %d\n", i);
			}else{
				kprintf("No drive found at port %d\n", i);
			}
		}
		pi >>= 1;
		i ++;
	}
}

/*
uint8_t getHeaderType(uint8_t bus,uint8_t slot,uint8_t func){
	uint32_t lbus  = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint8_t offset = 0x0c;
	uint32_t address;
	address = (uint32_t)((lbus << 16) | (lslot << 11) |
	              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

	outl(0xCF8,address);
	return (inl(0xCFC)>>16)&0xff;
}

*/
int check_type(hba_port_t *port){
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 	//kprintf("%x,%x",ipm,det);
	/*if (det != 3&&det!=1)	
		return 0;
	if (ipm != 1&&ipm!=6)
		return 0;*/
	if(det==0&&ipm==0)
	return 0;
 
	switch (port->sig){
	case AHCI_DEV_SATAPI:
		return AHCI_DEV_SATAPI;
	case AHCI_DEV_SEMB:
		return AHCI_DEV_SEMB;
	case AHCI_DEV_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}
 
void port_rebase(hba_port_t *port, int portno){
	//kprintf("print rebase");
	abar->ghc = (uint32_t) (1 << 31);
    abar->ghc = (uint32_t) (1 << 0);
    abar->ghc = (uint32_t) (1 << 31);
    abar->ghc = (uint32_t) (1 << 1);
	stop_cmd(port);	
	port->clb = AHCI_BASE + (portno<<10);
	for(int i=0;i<1024;i++){
		*(char*)((port->clb)+i) = 0;
	}
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	for(int i=0;i<256;i++){
		*(char*)((port->clb)+i) = 0;
	}
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
	for (int i=0; i<32; i++){
		cmdheader[i].prdtl = 8;
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		for(int j=0;j<256;j++){
			*(char*)((cmdheader[i].ctba)+j) = 0;
		}

	}
	start_cmd(port);
}


void start_cmd(hba_port_t *port){

	while (port->cmd & HBA_PxCMD_CR);
 
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_SUD;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(hba_port_t *port){
	// Clear ST (bit0)
	//port->cmd &= ~HBA_PxCMD_ST;
	//port->cmd |= HBA_PxCMD_ST;




	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
	port->cmd &= ~HBA_PxCMD_FR;
	port->cmd &= ~HBA_PxCMD_CR;
	port->cmd &= ~HBA_PxCMD_SLU;
	port->cmd &= ~HBA_PxCMD_PAR;
	/*

	port->cmd &= ~HBA_PxCMD_SLU;
	port->cmd &= ~HBA_PxCMD_PAR;
	port->cmd |= HBA_PxCMD_CLO;
	port->cmd |= HBA_PxCMD_SUD;
	port->cmd |= HBA_PxCMD_POD;
	port->cmd &= ~HBA_PxCMD_ST;
	*/
}

int find_cmdslot(hba_port_t *port){

	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i<32; i++){
		if ((slots&1) == 0){
			return i;
		}
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}

int write_ahci(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, char* buf){
	port->is_rwc = (uint32_t)-1;
	int spin = 0;
	int slot = find_cmdslot(port);
	if(slot==-1){
		return -1;
	}
	//kprintf("find the slot %d \n",slot);

	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader= cmdheader+slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = 1;
	cmdheader->c = 1;              
    cmdheader->p = 1;               
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;


	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	int tblSize = sizeof(hba_cmd_tbl_t) +(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t);
	for(int i=0;i<tblSize;i++){
		*(uint8_t*)((cmdtbl)+i) = 0;
	}

	int i=0;

	for (i=0; i<cmdheader->prdtl-1; i++){
		cmdtbl->prdt_entry[i].i = 1;
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;
		count -= 16;
		buf += 4*1024;
	}

	cmdtbl->prdt_entry[i].i = 1;
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;

	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	
	cmdfis->command = 0x35;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
	
	//TODO:count has a number less than 16. whats happening here?
	cmdfis->count = count;

	port->ci = 1;

	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000){
		spin++;
	}
	if (spin == 1000000){
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;


	while (1){
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}
	return 1;
}


/*ref from osDev link:http://wiki.osdev.org/AHCI#Example_-_Read_hard_disk_sectors*/
int read_ahci(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, char* buf){
	port->is_rwc = (uint32_t)-1;
	int spin = 0;
	int slot = find_cmdslot(port);
	if(slot==-1){
		return -1;
	}

	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader= cmdheader+slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = 0;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;

	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	int tblSize = sizeof(hba_cmd_tbl_t) +(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t);
	for(int i=0;i<tblSize;i++){
		*(uint8_t*)((cmdtbl)+i) = 0;
	}
	int i=0;
	for (i=0; i<cmdheader->prdtl-1; i++){
		cmdtbl->prdt_entry[i].i = 1;
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;
		count -= 16;
		buf += 4*1024;
	}

	cmdtbl->prdt_entry[i].i = 1;
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;

	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = 0x25;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
	cmdfis->count = count;
	port->ci = 1; 

	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000){
		spin++;
	}
	if (spin == 1000000){
		kprintf("Port is hung\n");
		return -1;
	}
	port->ci = 1<<slot;
	while (1){
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES){
			kprintf("Read disk error\n");
			return -1;
		}
	}
	if (port->is_rwc & HBA_PxIS_TFES){
		kprintf("Read disk error\n");
		return -1;
	}
	return 1;
}
