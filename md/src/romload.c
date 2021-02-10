// DGen/SDL v1.15+

// Module for loading in the different rom image types (.bin/.smd)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int load_bin_into(char *name,unsigned char *into)
{
  FILE *hand=NULL;
  int file_size=0;

  hand=fopen(name,"rb");
  if (hand==NULL)
    return -1;

  fseek(hand,0,SEEK_END);
  file_size=ftell(hand);
  fseek(hand,0,SEEK_SET);
  if (into==NULL) return file_size;

  fread(into,1,file_size,hand);
  fclose(hand);
  return 0;
}

/*
  WHAT YOU FIND IN THE 512 BYTES HEADER:

0: Number of blocks                           1
1: H03                                        *
2: SPLIT?                                     2
8: HAA                                        *
9: HBB                                        *
ALL OTHER BYTES: H00

1: This first byte should have the number of 16KB blocks the rom has.
The header isn't part of the formula, so this number is:
            [size of rom-512]/16386
   If the size is more than 255, the value should be H00.

2: This byte indicates if the ROM is a part of a splitted rom series. If
the rom is the last part of the series (or isn't a splitted rom at all),
this byte should be H00. In other cases should be H40. See "CREATING
SPLITTED ROMS" for details on this format.
*/

// 16k chunks, even bytes first then odd

static int load_smd_into(char *name,unsigned char *into)
{
  unsigned char head[512]={0};
  FILE *hand=NULL;
  int chunk_count=0,file_chunks=0;
  int file_size=0;
  unsigned char *chunk_buf=NULL;
  int got_to=0,i;

  hand=fopen(name,"rb");
  if (hand==NULL)
    return -1;

  fseek(hand,0,SEEK_END);
  file_size=ftell(hand);
  fseek(hand,0,SEEK_SET);

  if (fread(head,1,512,hand)!=512) { fclose(hand); return -1; }

  //chunk_count=head[0];
  // Sometimes header is wrong apparently

  file_chunks=((file_size-512)/16384);

  chunk_count=file_chunks;

  //if (chunk_count>file_chunks) chunk_count=file_chunks;

  if (into==NULL) return (chunk_count*16384);

  chunk_buf=malloc(16384);
  if (chunk_buf==NULL)
    {printf ("out of mem\n"); fclose(hand); return -1;}

  for (got_to=0,i=0; i<chunk_count; i++,got_to+=16384)
  {
    int j;
    // Deinterleave each chunk
    fread(chunk_buf,1,16384,hand);
    for (j=0;j<8192;j++)
      into[got_to+(j<<1)+1]=chunk_buf[j];
    for (j=0;j<8192;j++)
      into[got_to+(j<<1)+0]=chunk_buf[j+8192];
  }
  free(chunk_buf);

  fclose(hand);

  return 0;
}

// If 'into' is NULL returns rom size, otherwise expect
// 'into' to be a buffer big enough for the rom size
// (i.e. pass NULL, malloc, pass pointer, emulate, free pointer)

int load_rom_into(char *name,unsigned char *into)
{
  int format=0; // bin 0, smd 1
  int len;
  unsigned char magicbuf[10];
  FILE *romfile;
  if (name==NULL) return -1;

  /* Open the file and get the first little shnippit of it so we can check
   * the magic numbers on it. */
  if(!(romfile = fopen(name, "rb"))) return -1;
  fread(magicbuf, 10, 1, romfile);
  fclose(romfile);

  /* Check for the magic on various gzip-supported compressions */
  if((magicbuf[0] == 037 && magicbuf[1] == 036)  || /* compress'd (.Z)     */
     (magicbuf[0] == 037 && magicbuf[1] == 0213) || /* gzipped    (.gz)    */
     (magicbuf[0] == 037 && magicbuf[1] == 0236) || /* frozen     (.f,.z)  */
     (magicbuf[0] == 037 && magicbuf[1] == 0240) || /* LZH        (?)      */
     (magicbuf[0] == 'P' && magicbuf[1] == 'K'))    /* ZIP        (.zip ;) */
    {
      char temp[0x100], temp2[0x80];
      srand(time(NULL));
      /* Run it through gzip (I know this is cheap ;) */
      sprintf(temp2, "/tmp/dgenrom_%d_%d", rand(), rand());
      sprintf(temp, "gzip -S \"\" -cdq %s > %s", name, temp2);
      /* If gzip returned an error, stop */
      if(system(temp)) { remove(temp2); return -1; };
      /* Recurse with the new file */
      len = load_rom_into(temp2, into);
      remove(temp2);
      sync();
      return len;
    }
  /* Do bzip2 also */
  if(magicbuf[0] == 'B' && magicbuf[1] == 'Z' && magicbuf[2] == 'h')
    {
      /* Damn, this looks almost like the gzip stuff above. *lol* :) */
      char temp[0x100], temp2[0x80];
      srand(time(NULL));
      /* Go through bzip2 */
      sprintf(temp2, "/tmp/dgenrom_%d_%d", rand(), rand());
      sprintf(temp, "bzip2 -cd %s > %s", name, temp2);
      /* If we got an error, stop */
      if(system(temp)) { remove(temp2); return -1; };
      /* Recurse with the uncompressed file */
      len = load_rom_into(temp2, into);
      remove(temp2);
      sync();
      return len;
  }
  /* Next check for SMD magic */
  if(magicbuf[8] == 0xaa && magicbuf[9] == 0xbb)
    format = 1;
  /* Otherwise we can only hope it's binary */
  else format = 0;
	    
  switch (format)
  {
    case 1:  return load_smd_into(name,into);
    default: return load_bin_into(name,into);
  }
  return -1;
}
