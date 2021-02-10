// DGen v1.13+
// Megadrive's VDP C++ module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md.h"

int md_vdp::get_screen_info(struct dgen_sinfo *si)
{
  // Release the pointers to vdp Data to an external function
  si->vram=vram; si->cram=cram; si->vsram=vsram;
  si->vdp_reg=reg;
  // NB - if you change anything, remember to set 'dirt' accordingly
  return 0;
}

int md_vdp::reset()
{
  if (!ok) return 1;

  rw_mode=0x00; rw_addr=0; rw_dma=0;
  memset(mem,0,0x10100);
  memset(reg,0,0x20);
  memset(dirt,0xff,0x35); // mark everything as changed
  return 0;
}

md_vdp::md_vdp()
{
  ok=0;
  belongs=0; // Don't know which megadrive vdp belongs to yet

  mem=vram=cram=vsram=NULL;
  mem=new unsigned char[0x10100+0x35]; //0x20+0x10+0x4+1 for dirt
  if (mem==0) return;
  vram=mem+0x00000; cram=mem+0x10000; vsram=mem+0x10080;
  dirt=mem+0x10100; // VRAM/CRAM/Reg dirty buffer bitfield
  // Also in 0x34 are global dirt flags (inclduing VSRAM this time)

  highpal=new unsigned int[64]; if (highpal==0) return;

  Bpp = Bpp_times8 = 0;

  ok=1;
  reset();

}

md_vdp::~md_vdp()
{
  free(mem); 
  mem=vram=cram=vsram=NULL;
  ok=0;
}

int md_vdp::dma_len()
{ return (reg[0x14]<<8)+reg[0x13]; }

int md_vdp::dma_addr()
{
  int addr=0;
  addr=(reg[0x17]&0x7f)<<17;
  addr+=reg[0x16]<<9;
  addr+=reg[0x15]<<1;
  return addr;
}


// DMA can read from anywhere
unsigned char md_vdp::dma_mem_read(int addr)
{
  return belongs->misc_readbyte(addr);
}

// Must go through these calls to update the dirty flags
int md_vdp::poke_vram(int addr,unsigned char d)
{
// Keeping GCC happy over unused vars. [PKH]
//  int diff=0;
  addr&=0xffff;
  if (vram[addr]!=d)
  {
    // Store dirty information down to 256 byte level in bits
    int byt,bit;
    byt=addr>>8; bit=byt&7; byt>>=3; byt&=0x1f;
    dirt[0x00+byt]|=(1<<bit); dirt[0x34]|=1;
    vram[addr]=d;
  }
  return 0;
}
int md_vdp::poke_cram(int addr,unsigned char d)
{
//  int diff=0;
  addr&=0x007f;
  if (cram[addr]!=d)
  {
    // Store dirty information down to 1byte level in bits
    int byt,bit;
    byt=addr; bit=byt&7; byt>>=3; byt&=0x0f;
    dirt[0x20+byt]|=(1<<bit); dirt[0x34]|=2;
    cram[addr]=d; 
  }

  return 0;
}
int md_vdp::poke_vsram(int addr,unsigned char d)
{
//  int diff=0; 
  addr&=0x007f;
  if (vsram[addr]!=d)
  { dirt[0x34]|=4; vsram[addr]=d; }
  return 0;
}

int md_vdp::putword(unsigned short d)
{
//  int diff=0;
  // Called by dma or a straight write
  switch(rw_mode)
  {
    case 0x04: poke_vram (rw_addr+0,d>>8); poke_vram (rw_addr+1,d&0xff); break;
    case 0x0c: poke_cram (rw_addr+0,d>>8); poke_cram (rw_addr+1,d&0xff); break;
    case 0x14: poke_vsram(rw_addr+0,d>>8); poke_vsram(rw_addr+1,d&0xff); break;
  }
  rw_addr+=reg[15];
  return 0;
}

int md_vdp::putbyte(unsigned char d)
{
//  int diff=0;
  // Called by dma or a straight write
  switch(rw_mode)
  {
    case 0x04: poke_vram (rw_addr,d>>8); break;
    case 0x0c: poke_cram (rw_addr,d>>8); break;
    case 0x14: poke_vsram(rw_addr,d>>8); break;
  }
  rw_addr+=reg[15];
  return 0;
}

#undef MAYCHANGE

unsigned short md_vdp::readword()
{
  // Called by a straight read only
  unsigned short result=0x0000;
  switch(rw_mode)
  {
    case 0x00: result=( vram[(rw_addr+0)&0xffff]<<8)+
                        vram[(rw_addr+1)&0xffff]; break;
    case 0x20: result=( cram[(rw_addr+0)&0x007f]<<8)+
                        cram[(rw_addr+1)&0x007f]; break;
    case 0x10: result=(vsram[(rw_addr+0)&0x007f]<<8)+
                       vsram[(rw_addr+1)&0x007f]; break;
  }
  rw_addr+=reg[15];
  return result;
}

unsigned char md_vdp::readbyte()
{
  // Called by a straight read only
  unsigned char result=0x00;
  switch(rw_mode)
  {
    case 0x00: result= vram[(rw_addr+0)&0xffff]; break;
    case 0x20: result= cram[(rw_addr+0)&0x007f]; break;
    case 0x10: result=vsram[(rw_addr+0)&0x007f]; break;
  }
  rw_addr+=reg[15];
  return result;
}


int md_vdp::command(unsigned int cmd)
{
// Decode 32-bit VDP command
  rw_dma=((cmd&0x80)==0x80);
  rw_mode= cmd&0x00000070;
  rw_mode|=(cmd&0xc0000000)>>28;
// mode writes: 04=VRAM 0C=CRAM 14=VSRAM
// mode reads:  00=VRAM 20=CRAM 10=VSRAM
  rw_addr= (cmd&0x00000003)<<14;
  rw_addr|=(cmd&0x3fff0000)>>16;
  // If not dma (or we need a fill),
  // we are set up to write any data sent to vdp data reg

  // if it's a dma request do it straight away
  if (rw_dma)
  {
    int mode=(reg[0x17]>>6)&3;
    int s=0,d=0,i=0,len=0;
    s=dma_addr(); d=rw_addr; len=dma_len();
    switch (mode)
    {
      case 0: case 1:
        for (i=0;i<len;i++)
        {
          unsigned short val;
          val= dma_mem_read(s++); val<<=8;
          val|=dma_mem_read(s++); putword(val);
        }
      break;
      case 2:
        // Done later on
      break;
      case 3:
        for (i=0;i<len;i++)
        {
          unsigned short val;
          val= vram[(s++)&0xffff]; val<<=8;
          val|=vram[(s++)&0xffff]; putword(val);
        }
      break;
    }
  }

  return 0;
}

int md_vdp::writeword(unsigned short d)
{
  if (rw_dma)
  {
    // This is the 'done later on' bit for words
    // Do a dma fill if it's set up:
    if (((reg[0x17]>>6)&3)==2)
    {
      int i,len;
      len=dma_len();
      for (i=0;i<len;i++)
        putword(d);
      return 0;
    }
  }
  else
  {
    putword(d);
    return 0;
  }
  return 0;
}

int md_vdp::writebyte(unsigned char d)
{
  if (rw_dma)
  {
    // This is the 'done later on' bit for bytes
    // Do a dma fill if it's set up:
    if (((reg[0x17]>>6)&3)==2)
    {
      int i,len;
      len=dma_len();
      for (i=0;i<len;i++)
        putbyte(d);
      return 0;
    }
  }
  else
  {
    putbyte(d);
    return 0;
  }

  return 0;
}

