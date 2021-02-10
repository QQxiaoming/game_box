// DGen/SDL v1.15+
// Megadrive C++ module - misc memory

#include <stdio.h>
#include "md.h"
// md.h also has include fm.h

// This is to handle memory accesses outside of ROM and RAM
// e.g. c00000 and a00000 addresses
// REMEMBER NOT TO USE ANY STATIC variables, because they
// will exist thoughout ALL megadrives!

unsigned char md::z80_read(unsigned int a)
{
  if (a<0x2000) return z80ram[a&0xffff];


  if ((a&0xfffc)==0x4000) // 00 01 02 03 - not sure about reads
  {
    return myfm_read(a&3);
  }

  if (a>=0x8000)
  {
    int addr68k;
    addr68k=z80_bank68k<<15;
    addr68k+=a&0x7fff;

    return misc_readbyte(addr68k);
  }

  return 0;
}

unsigned short md::z80_port_read(unsigned short a)
{
  int ret=0;

//  dprintf("z80 port read %.2x return %.4x\n",a,ret);

  return ret;
}
void md::z80_port_write(unsigned short a,unsigned char v)
{
//  dprintf("z80 port write %.2x %.2x\n",a,v);
}


void md::z80_write(unsigned int a,unsigned char v)
{
  if ((a&0xfffc)==0x4000) // 00 01 02 03
  {
    myfm_write(a&3,v,1);
    return ;
  }
  if (a==0x7f11)
  { mysn_write(v); return; }

  if (a==0x6000)
  {
    z80_bank68k>>=1;
    z80_bank68k+=(v&1)<<8;
    z80_bank68k&=0x1ff; // 9 bits and filled in the new top one
    return ;
  }
  if (a<0x2000) { z80ram[a&0xffff]=v; return; }


  if (a>=0x8000)
  {
    int addr68k;
    addr68k=z80_bank68k<<15;
    addr68k+=a&0x7fff;
    misc_writebyte(addr68k,v);
    return;
  }

//  dprintf("z80 write %.4x %.2x\n",a,v);
  return;
}


unsigned md::misc_readbyte(unsigned a)
{
  unsigned char ret=0;
  a&=0x00ffffff; // in case stars didn't clip to 24-bit bus


  // In case it's a rom read after all
  if (a<0xa00000) {
    // Saveram
    if((save_active)     && (save_len) &&
       (a >= save_start) && ((a - save_start) < save_len)) {
      return saveram[(a^1) - save_start];
    }
    else if ((signed)(a^1)<romlen) { ret=rom[(a^1)]; goto end; }
  }

  // In case it's a ram read after all
  if (a>=0xe00000) if (a<=0xffffff) { ret=ram[(a^1)&0xffff]; goto end; }

  // GFX data read (byte)
  if ((a&0xfffffffe)==0x00c00000) // 00 or 01
    { ret=vdp.readbyte(); goto end; }


  if ((a&0xfffffffc)==0xa04000)
  {
    ret=myfm_read(a&3); goto end;
  }

  if ((a>=0xa00000)&&(a<0xa08000))
  {
    // Read from z80's memory
    ret=z80_read(a&0x7fff); goto end;
  }

  // I/O port access
  if ((a&0xfffffe)==0xa10002)
  {
    if (aoo3_six==3)
    {
      // Extended pad info
      if (aoo3_toggle==0) ret=((pad[0]>>8)&0x30)+0x00;
      else ret=((pad[0])&0x30)+0x40+((pad[0]>>16)&0xf);
    }
    else
    {
      if (aoo3_toggle==0)
      {
        if (aoo3_six==4) ret=((pad[0]>>8)&0x30)+0x00+0x0f;
        else             ret=((pad[0]>>8)&0x30)+0x00+(pad[0]&3);
      }
      else ret=((pad[0])&0x30)+0x40+(pad[0]&15);
    }
    goto end;
  }

  if ((a&0xfffffe)==0xa10004)
  {
    if (aoo5_six==3)
    {
      // Extended pad info
      if (aoo5_toggle==0x00) ret=((pad[1]>>8)&0x30)+0x00;
      else ret=((pad[1])&0x30)+0x40+((pad[1]>>16)&0xf);
    }
    else
    {
      if (aoo5_toggle==0x00)
      {
        if (aoo5_six==4) ret=((pad[1]>>8)&0x30)+0x00+0x0f;
        else             ret=((pad[1]>>8)&0x30)+0x00+(pad[1]&3);
      }
      else ret=((pad[1])&0x30)+0x40+(pad[1]&15);
    }
    goto end;
  }

  if (a==0xa10001)
  {
    // overseas/pal/disk/0/ md version (0-f) - may make games act different!
    if ((country_ver&0xff0)==0xff0)
    {
      // autodetect country
      int headcode=0x80,avail=0;
      int i;
      for (i=0;i<3;i++)
      {
        int ch=misc_readbyte(0x1f0+i);
             if (ch=='U') avail|=1;
        else if (ch=='E') avail|=2;
        else if (ch=='J') avail|=4;
      }
           if (avail&1) headcode=0x80;
      else if (avail&2) headcode=0x80;
      else if (avail&4) headcode=0x00;
      ret=headcode+(country_ver&0x0f) | (pal? 0x40 : 0);
    }
    else ret=country_ver;
    goto end;
  }

  if (a==0xa11000) {ret=0xff; goto end; }
  if (a==0xa11001) {ret=0xff; goto end; }
  if (a==0xa11100)
  {
    ret=z80_online; goto end;
  }
  if (a==0xa11101) {ret=0x00; goto end; }
  if (a==0xa11200) {ret=0xff; goto end; }
  if (a==0xa11201) {ret=0xff; goto end; }

  // Genecyst style - toggles fifo full/empty
  if (a==0xc00004)
  {
    // This is the genecyst fudge for FIFO full/empty
    coo4^=0x03;
    ret=coo4; goto end;
  }
  if (a==0xc00005)
  {
    // This is the genecyst fudge for in h-blank
    //coo5^=0x04;
    ret=coo5 | (pal? 1 : 0); goto end;
  }

  if (a==0xc00008) { ret=calculate_coo8(); goto end; }
  if (a==0xc00009) { ret=calculate_coo9(); goto end; }


  end:
  return ret;
}

void md::misc_writebyte(unsigned a,unsigned d)
{
  a&=0x00ffffff; // in case stars didn't clip to 24-bit bus

  // Saveram write
  if(save_len && !save_prot) if(a >= save_start)
    if ((a - save_start) < save_len)
      { saveram[(a^1) - save_start] = d; return; }

  // In case it's a ram write after all

  if (a>=0xe00000) if (a<=0xffffff) { ram[(a^1)&0xffff]=d; return; }

  // GFX data write (byte)
  if ((a&0xfffffffe)==0x00c00000) // 00 or 01
    { vdp.writebyte(d); return; }

  if ((a&0xfffffffc)==0xa04000)
  {
    myfm_write(a&3,d,1);
    return;
  }

  if (a==0xc00011)
  {
    mysn_write(d); return;
  }

  if (a==0xa11100)
  {
//d8 (W) 0: BUSREQ CANCEL
//       1: BUSREQ REQUEST
    if (d==1) z80_online=0;
    else z80_online=1;
    return;
  }
  if (a==0xa11101) return;
  if (a==0xa11200)
  {
    if (d==0)
      mz80reset();
    return;
  }
  if (a==0xa11201) return;

  if ((a>=0xa00000)&&(a<0xa08000))
  {
    // Write into z80's memory
    z80_write(a&0x7fff,d);
    return;
  }

  // I/O port access
  if (a==0xa10003)
  {
    if (aoo3_six>=0 && (d&0x40)==0 && aoo3_toggle ) aoo3_six++;

    if (aoo3_six>0xc00000) aoo3_six&=~0x400000;
    // keep it circling around a high value

    if (d&0x40) aoo3_toggle=1; else aoo3_toggle=0;
    aoo3_six_timeout=0;
    return;
  }
  if (a==0xa10005)
  {
    if (aoo5_six>=0 && (d&0x40)==0 && aoo5_toggle ) aoo5_six++;

    if (aoo5_six>0xc00000) aoo5_six&=~0x400000;
    // keep it circling around a high value

    if (d&0x40) aoo5_toggle=1; else aoo5_toggle=0;
    aoo5_six_timeout=0;
    return;
  }
  // Saveram status (thanks Steve :)
  if(a==0xa130f1)
  {
    // Bit 0: 0=rom active, 1=sram active
    // Bit 1: 0=writeable,  1=write protect
    save_active = d & 1;
    save_prot   = d & 2;
  }
}

unsigned md::misc_readword(unsigned a)
{
  unsigned int ret=0;

  a&=0x00ffffff; // in case stars didn't clip to 24-bit bus

  if ((a&0xfffffffc)==0x00c00000) // 00 or 02
  {
    ret=vdp.readword();
    goto end;
  }

  // else pass onto readbyte
  ret =misc_readbyte(a)<<8;
  ret|=misc_readbyte(a+1);
  goto end;

  end:
  return ret;
}

void md::misc_writeword(unsigned a,unsigned d)
{
  a&=0x00ffffff; // in case stars didn't clip to 24-bit bus

  // GFX data write (word)
  if ((a&0xfffffffc)==0x00c00000) // 00 or 02
  {
    vdp.writeword(d);
    return;
  }

  if ((a&0xfffffffc)==0x00c00004) // 04 or 06
  {
    if (coo_waiting)
    {
      // Okay completed the vdp command
      coo_cmd|=d; coo_waiting=0; 

      vdp.command(coo_cmd);
      return;
    }
    if ((d&0xc000)==0x8000)
    {
      int addr; addr=d>>8; addr&=0x1f;
      if (vdp.reg[addr]!=(d&0xff))
      {
        // Store dirty information down to 1byte level in bits
        int byt,bit;
        byt=addr; bit=byt&7; byt>>=3; byt&=0x03;
        vdp.dirt[0x30+byt]|=(1<<bit); vdp.dirt[0x34]|=8;
      }
      vdp.reg[addr]=d&0xff;
      return;
    }
    coo_cmd=d<<16; coo_waiting=1;
    return;
  }

  // else pass onto writebyte
  misc_writebyte(a,d>>8);
  misc_writebyte(a+1,d&255);
}

