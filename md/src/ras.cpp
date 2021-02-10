// DGen/SDL v1.16+
// New raster effects engine
// I'd like to thank the Mac folks for giving me a good template to work from.
// This is just a cheap rehash of their code, except friendlier to other bit
// depths. :) I also put in a few little optimizations, like blank checking
// and especially the sprites.

#include <stdio.h>
#include <stdlib.h>
#include "md.h"

// This is marked each time the palette is updated. Handy for the 8bpp
// implementation, so we don't waste time changing the palette unnecessarily.
int pal_dirty;

// Macros, to route draw_tile and draw_tile_solid to the right handler
#define draw_tile(which, line, where) \
	switch(Bpp)\
	  {\
	  case 1:\
	    draw_tile1((which),(line),(where)); break;\
	  case 2:\
	    draw_tile2((which),(line),(where)); break;\
	  case 3:\
	    draw_tile3((which),(line),(where)); break;\
	  case 4:\
	    draw_tile4((which),(line),(where)); break;\
	  }

#define draw_tile_solid(which, line, where) \
	switch(Bpp)\
	  {\
	  case 1:\
	    draw_tile1_solid((which),(line),(where)); break;\
	  case 2:\
	    draw_tile2_solid((which),(line),(where)); break;\
	  case 3:\
	    draw_tile3_solid((which),(line),(where)); break;\
	  case 4:\
	    draw_tile4_solid((which),(line),(where)); break;\
	  }

// Silly utility function, get a big-endian word
#ifdef WORDS_BIGENDIAN
static inline int get_word(unsigned char *where)
  { return (int)(*(unsigned short*)where); }
#else
static inline int get_word(unsigned char *where) 
  { return (where[0] << 8) | where[1]; }
#endif

// Tile pixel masks
#ifdef WORDS_BIGENDIAN
#  define PIXEL0 (0xf0000000)
#  define PIXEL1 (0x0f000000)
#  define PIXEL2 (0x00f00000)
#  define PIXEL3 (0x000f0000)
#  define PIXEL4 (0x0000f000)
#  define PIXEL5 (0x00000f00)
#  define PIXEL6 (0x000000f0)
#  define PIXEL7 (0x0000000f)
#  define SHIFT0 (28)
#  define SHIFT1 (24)
#  define SHIFT2 (20)
#  define SHIFT3 (16)
#  define SHIFT4 (12)
#  define SHIFT5 ( 8)
#  define SHIFT6 ( 4)
#  define SHIFT7 ( 0)
#else // WORDS_BIGENDIAN
#  define PIXEL0 (0x000000f0)
#  define PIXEL1 (0x0000000f)
#  define PIXEL2 (0x0000f000)
#  define PIXEL3 (0x00000f00)
#  define PIXEL4 (0x00f00000)
#  define PIXEL5 (0x000f0000)
#  define PIXEL6 (0xf0000000)
#  define PIXEL7 (0x0f000000)
#  define SHIFT0 ( 4)
#  define SHIFT1 ( 0)
#  define SHIFT2 (12)
#  define SHIFT3 ( 8)
#  define SHIFT4 (20)
#  define SHIFT5 (16)
#  define SHIFT6 (28)
#  define SHIFT7 (24)
#endif // WORDS_BIGENDIAN

#ifdef ASM_TILES
extern "C" {
  void _asm_tiles_init(unsigned char *vram,
  		       unsigned char *reg,
		       unsigned *highpal);
  
  void _drawtile1(int which, int line, unsigned char *where);
  void _drawtile1_solid(int which, int line, unsigned char *where);
  void _drawtile2(int which, int line, unsigned char *where);
  void _drawtile2_solid(int which, int line, unsigned char *where);
  void _drawtile3(int which, int line, unsigned char *where);
  void _drawtile3_solid(int which, int line, unsigned char *where);
  void _drawtile4(int which, int line, unsigned char *where);
  void _drawtile4_solid(int which, int line, unsigned char *where);
}

// Pass off these calls to assembler counterparts
inline void md_vdp::draw_tile1_solid(int which, int line, unsigned char *where)
  { _drawtile1_solid(which, line, where); }

inline void md_vdp::draw_tile1(int which, int line, unsigned char *where)
  { _drawtile1(which, line, where); }

inline void md_vdp::draw_tile2_solid(int which, int line, unsigned char *where)
  { _drawtile2_solid(which, line, where); }

inline void md_vdp::draw_tile2(int which, int line, unsigned char *where)
  { _drawtile2(which, line, where); }

inline void md_vdp::draw_tile3_solid(int which, int line, unsigned char *where)
  { _drawtile3_solid(which, line, where); }

inline void md_vdp::draw_tile3(int which, int line, unsigned char *where)
  { _drawtile3(which, line, where); }

inline void md_vdp::draw_tile4_solid(int which, int line, unsigned char *where)
  { _drawtile4_solid(which, line, where); }

inline void md_vdp::draw_tile4(int which, int line, unsigned char *where)
  { _drawtile4(which, line, where); }

#else // ASM_TILES

// Blit tile solidly, for 1 byte-per-pixel
inline void md_vdp::draw_tile1_solid(int which, int line, unsigned char *where)
{
  unsigned tile, pal;

  pal = (which >> 9 & 0x30); // Determine which 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      *(where  ) = ((tile & PIXEL7)>>SHIFT7) | pal;
      *(where+1) = ((tile & PIXEL6)>>SHIFT6) | pal;
      *(where+2) = ((tile & PIXEL5)>>SHIFT5) | pal;
      *(where+3) = ((tile & PIXEL4)>>SHIFT4) | pal;
      *(where+4) = ((tile & PIXEL3)>>SHIFT3) | pal;
      *(where+5) = ((tile & PIXEL2)>>SHIFT2) | pal;
      *(where+6) = ((tile & PIXEL1)>>SHIFT1) | pal;
      *(where+7) = ((tile & PIXEL0)>>SHIFT0) | pal;
    } else {
      *(where  ) = ((tile & PIXEL0)>>SHIFT0) | pal;
      *(where+1) = ((tile & PIXEL1)>>SHIFT1) | pal;
      *(where+2) = ((tile & PIXEL2)>>SHIFT2) | pal;
      *(where+3) = ((tile & PIXEL3)>>SHIFT3) | pal;
      *(where+4) = ((tile & PIXEL4)>>SHIFT4) | pal;
      *(where+5) = ((tile & PIXEL5)>>SHIFT5) | pal;
      *(where+6) = ((tile & PIXEL6)>>SHIFT6) | pal;
      *(where+7) = ((tile & PIXEL7)>>SHIFT7) | pal;
    }
} 

// Blit tile, leaving color zero transparent, for 1 byte per pixel
inline void md_vdp::draw_tile1(int which, int line, unsigned char *where)
{
  unsigned tile, pal;

  pal = (which >> 9 & 0x30); // Determine which 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));
  // If the tile is all 0's, why waste the time?
  if(!tile) return;

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      if(tile & PIXEL7) *(where  ) = ((tile & PIXEL7)>>SHIFT7) | pal;
      if(tile & PIXEL6) *(where+1) = ((tile & PIXEL6)>>SHIFT6) | pal;
      if(tile & PIXEL5) *(where+2) = ((tile & PIXEL5)>>SHIFT5) | pal;
      if(tile & PIXEL4) *(where+3) = ((tile & PIXEL4)>>SHIFT4) | pal;
      if(tile & PIXEL3) *(where+4) = ((tile & PIXEL3)>>SHIFT3) | pal;
      if(tile & PIXEL2) *(where+5) = ((tile & PIXEL2)>>SHIFT2) | pal;
      if(tile & PIXEL1) *(where+6) = ((tile & PIXEL1)>>SHIFT1) | pal;
      if(tile & PIXEL0) *(where+7) = ((tile & PIXEL0)>>SHIFT0) | pal;
    } else {
      if(tile & PIXEL0) *(where  ) = ((tile & PIXEL0)>>SHIFT0) | pal;
      if(tile & PIXEL1) *(where+1) = ((tile & PIXEL1)>>SHIFT1) | pal;
      if(tile & PIXEL2) *(where+2) = ((tile & PIXEL2)>>SHIFT2) | pal;
      if(tile & PIXEL3) *(where+3) = ((tile & PIXEL3)>>SHIFT3) | pal;
      if(tile & PIXEL4) *(where+4) = ((tile & PIXEL4)>>SHIFT4) | pal;
      if(tile & PIXEL5) *(where+5) = ((tile & PIXEL5)>>SHIFT5) | pal;
      if(tile & PIXEL6) *(where+6) = ((tile & PIXEL6)>>SHIFT6) | pal;
      if(tile & PIXEL7) *(where+7) = ((tile & PIXEL7)>>SHIFT7) | pal;
    }
} 

// Blit tile solidly, for 2 byte-per-pixel
inline void md_vdp::draw_tile2_solid(int which, int line, unsigned char *where)
{
  unsigned tile, temp, *pal;
  unsigned short *wwhere = (unsigned short*)where;

  pal = highpal + (which >> 9 & 0x30); // Determine which 16-color palette
  temp = *pal; *pal = highpal[reg[7]&0x3f]; // Get background color

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      *(wwhere  ) = pal[((tile & PIXEL7)>>SHIFT7)];
      *(wwhere+1) = pal[((tile & PIXEL6)>>SHIFT6)];
      *(wwhere+2) = pal[((tile & PIXEL5)>>SHIFT5)];
      *(wwhere+3) = pal[((tile & PIXEL4)>>SHIFT4)];
      *(wwhere+4) = pal[((tile & PIXEL3)>>SHIFT3)];
      *(wwhere+5) = pal[((tile & PIXEL2)>>SHIFT2)];
      *(wwhere+6) = pal[((tile & PIXEL1)>>SHIFT1)];
      *(wwhere+7) = pal[((tile & PIXEL0)>>SHIFT0)];
    } else {
      *(wwhere  ) = pal[((tile & PIXEL0)>>SHIFT0)];
      *(wwhere+1) = pal[((tile & PIXEL1)>>SHIFT1)];
      *(wwhere+2) = pal[((tile & PIXEL2)>>SHIFT2)];
      *(wwhere+3) = pal[((tile & PIXEL3)>>SHIFT3)];
      *(wwhere+4) = pal[((tile & PIXEL4)>>SHIFT4)];
      *(wwhere+5) = pal[((tile & PIXEL5)>>SHIFT5)];
      *(wwhere+6) = pal[((tile & PIXEL6)>>SHIFT6)];
      *(wwhere+7) = pal[((tile & PIXEL7)>>SHIFT7)];
    }
  // Restore the original color
  *pal = temp;
} 

// Blit tile, leaving color zero transparent, for 2 byte per pixel
inline void md_vdp::draw_tile2(int which, int line, unsigned char *where)
{
  unsigned tile, *pal;
  unsigned short *wwhere = (unsigned short*)where;

  pal = highpal + (which >> 9 & 0x30); // Determine which 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));
  // If the tile is all 0's, why waste the time?
  if(!tile) return;

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      if(tile & PIXEL7) *(wwhere  ) = pal[((tile & PIXEL7)>>SHIFT7)];
      if(tile & PIXEL6) *(wwhere+1) = pal[((tile & PIXEL6)>>SHIFT6)];
      if(tile & PIXEL5) *(wwhere+2) = pal[((tile & PIXEL5)>>SHIFT5)];
      if(tile & PIXEL4) *(wwhere+3) = pal[((tile & PIXEL4)>>SHIFT4)];
      if(tile & PIXEL3) *(wwhere+4) = pal[((tile & PIXEL3)>>SHIFT3)];
      if(tile & PIXEL2) *(wwhere+5) = pal[((tile & PIXEL2)>>SHIFT2)];
      if(tile & PIXEL1) *(wwhere+6) = pal[((tile & PIXEL1)>>SHIFT1)];
      if(tile & PIXEL0) *(wwhere+7) = pal[((tile & PIXEL0)>>SHIFT0)];
    } else {
      if(tile & PIXEL0) *(wwhere  ) = pal[((tile & PIXEL0)>>SHIFT0)];
      if(tile & PIXEL1) *(wwhere+1) = pal[((tile & PIXEL1)>>SHIFT1)];
      if(tile & PIXEL2) *(wwhere+2) = pal[((tile & PIXEL2)>>SHIFT2)];
      if(tile & PIXEL3) *(wwhere+3) = pal[((tile & PIXEL3)>>SHIFT3)];
      if(tile & PIXEL4) *(wwhere+4) = pal[((tile & PIXEL4)>>SHIFT4)];
      if(tile & PIXEL5) *(wwhere+5) = pal[((tile & PIXEL5)>>SHIFT5)];
      if(tile & PIXEL6) *(wwhere+6) = pal[((tile & PIXEL6)>>SHIFT6)];
      if(tile & PIXEL7) *(wwhere+7) = pal[((tile & PIXEL7)>>SHIFT7)];
    }
} 

inline void md_vdp::draw_tile3_solid(int which, int line, unsigned char *where)
{
  unsigned tile; 
  unsigned char *pal;

  pal = ((unsigned char*)highpal) + (which >> 7 & 0xc0); // Determine 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));

  // Blit the tile (this is UGLY! Do you know of a better way? :P)
  if(which & 0x800)
    {
      *(where   ) = pal[((tile & 0x0f000000)>>22)+1];
      *(where+1 ) = pal[((tile & 0x0f000000)>>22)+2];
      *(where+2 ) = pal[((tile & 0x0f000000)>>22)  ];
      *(where+3 ) = pal[((tile & 0xf0000000)>>26)+1];
      *(where+4 ) = pal[((tile & 0xf0000000)>>26)+2];
      *(where+5 ) = pal[((tile & 0xf0000000)>>26)  ];
      *(where+6 ) = pal[((tile & 0x000f0000)>>14)+1];
      *(where+7 ) = pal[((tile & 0x000f0000)>>14)+2];
      *(where+8 ) = pal[((tile & 0x000f0000)>>14)  ];
      *(where+9 ) = pal[((tile & 0x00f00000)>>18)+1];
      *(where+10) = pal[((tile & 0x00f00000)>>18)+2];
      *(where+11) = pal[((tile & 0x00f00000)>>18)  ];
      *(where+12) = pal[((tile & 0x00000f00)>> 6)+1];
      *(where+13) = pal[((tile & 0x00000f00)>> 6)+2];
      *(where+14) = pal[((tile & 0x00000f00)>> 6)  ];
      *(where+15) = pal[((tile & 0x0000f000)>>10)+1];
      *(where+16) = pal[((tile & 0x0000f000)>>10)+2];
      *(where+17) = pal[((tile & 0x0000f000)>>10)  ];
      *(where+18) = pal[((tile & 0x0000000f)<< 2)+1];
      *(where+19) = pal[((tile & 0x0000000f)<< 2)+2];
      *(where+20) = pal[((tile & 0x0000000f)<< 2)  ];
      *(where+21) = pal[((tile & 0x000000f0)>> 2)+1];
      *(where+22) = pal[((tile & 0x000000f0)>> 2)+2];
      *(where+23) = pal[((tile & 0x000000f0)>> 2)  ];
    } else {
      *(where   ) = pal[((tile & 0x000000f0)>> 2)+1];
      *(where+ 1) = pal[((tile & 0x000000f0)>> 2)+2];
      *(where+ 2) = pal[((tile & 0x000000f0)>> 2)  ];
      *(where+ 3) = pal[((tile & 0x0000000f)<< 2)+1];
      *(where+ 4) = pal[((tile & 0x0000000f)<< 2)+2];
      *(where+ 5) = pal[((tile & 0x0000000f)<< 2)  ];
      *(where+ 6) = pal[((tile & 0x0000f000)>>10)+1];
      *(where+ 7) = pal[((tile & 0x0000f000)>>10)+2];
      *(where+ 8) = pal[((tile & 0x0000f000)>>10)  ];
      *(where+ 9) = pal[((tile & 0x00000f00)>> 6)+1];
      *(where+10) = pal[((tile & 0x00000f00)>> 6)+2];
      *(where+11) = pal[((tile & 0x00000f00)>> 6)  ];
      *(where+12) = pal[((tile & 0x00f00000)>>18)+1];
      *(where+13) = pal[((tile & 0x00f00000)>>18)+2];
      *(where+14) = pal[((tile & 0x00f00000)>>18)  ];
      *(where+15) = pal[((tile & 0x000f0000)>>14)+1];
      *(where+16) = pal[((tile & 0x000f0000)>>14)+2];
      *(where+17) = pal[((tile & 0x000f0000)>>14)  ];
      *(where+18) = pal[((tile & 0xf0000000)>>26)+1];
      *(where+19) = pal[((tile & 0xf0000000)>>26)+2];
      *(where+20) = pal[((tile & 0xf0000000)>>26)  ];
      *(where+21) = pal[((tile & 0x0f000000)>>22)+1];
      *(where+22) = pal[((tile & 0x0f000000)>>22)+2];
      *(where+23) = pal[((tile & 0x0f000000)>>22)  ];
    }
}

inline void md_vdp::draw_tile3(int which, int line, unsigned char *where)
{
  unsigned tile; 
  unsigned char *pal;

  pal = ((unsigned char*)highpal) + (which >> 7 & 0xc0); // Determine 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));
  // If it's empty, why waste the time?
  if(!tile) return;

  // Blit the tile (this is UGLY! Do you know of a better way? :P)
  if(which & 0x800)
    {
      // The numbers are a little wierd, because of little endian. *gag*
      // <SARCASM>Thanks Intel!</SARCASM>
      if(tile & 0x0f000000) { *(where   ) = pal[((tile & 0x0f000000)>>22)+1];
      			      *(where+1 ) = pal[((tile & 0x0f000000)>>22)+2];
      			      *(where+2 ) = pal[((tile & 0x0f000000)>>22)  ]; }
      if(tile & 0xf0000000) { *(where+3 ) = pal[((tile & 0xf0000000)>>26)+1];
      			      *(where+4 ) = pal[((tile & 0xf0000000)>>26)+2];
      			      *(where+5 ) = pal[((tile & 0xf0000000)>>26)  ]; }
      if(tile & 0x000f0000) { *(where+6 ) = pal[((tile & 0x000f0000)>>14)+1];
      			      *(where+7 ) = pal[((tile & 0x000f0000)>>14)+2];
      			      *(where+8 ) = pal[((tile & 0x000f0000)>>14)  ]; }
      if(tile & 0x00f00000) { *(where+9 ) = pal[((tile & 0x00f00000)>>18)+1];
      			      *(where+10) = pal[((tile & 0x00f00000)>>18)+2];
      			      *(where+11) = pal[((tile & 0x00f00000)>>18)  ]; }
      if(tile & 0x00000f00) { *(where+12) = pal[((tile & 0x00000f00)>> 6)+1];
      			      *(where+13) = pal[((tile & 0x00000f00)>> 6)+2];
      			      *(where+14) = pal[((tile & 0x00000f00)>> 6)  ]; }
      if(tile & 0x0000f000) { *(where+15) = pal[((tile & 0x0000f000)>>10)+1];
      			      *(where+16) = pal[((tile & 0x0000f000)>>10)+2];
      			      *(where+17) = pal[((tile & 0x0000f000)>>10)  ]; }
      if(tile & 0x0000000f) { *(where+18) = pal[((tile & 0x0000000f)<< 2)+1];
      			      *(where+19) = pal[((tile & 0x0000000f)<< 2)+2];
      			      *(where+20) = pal[((tile & 0x0000000f)<< 2)  ]; }
      if(tile & 0x000000f0) { *(where+21) = pal[((tile & 0x000000f0)>> 2)+1];
      			      *(where+22) = pal[((tile & 0x000000f0)>> 2)+2];
      			      *(where+23) = pal[((tile & 0x000000f0)>> 2)  ]; }
    } else {
      if(tile & 0x000000f0) { *(where   ) = pal[((tile & 0x000000f0)>> 2)+1];
      			      *(where+ 1) = pal[((tile & 0x000000f0)>> 2)+2];
      			      *(where+ 2) = pal[((tile & 0x000000f0)>> 2)  ]; }
      if(tile & 0x0000000f) { *(where+ 3) = pal[((tile & 0x0000000f)<< 2)+1];
      			      *(where+ 4) = pal[((tile & 0x0000000f)<< 2)+2];
      			      *(where+ 5) = pal[((tile & 0x0000000f)<< 2)  ]; }
      if(tile & 0x0000f000) { *(where+ 6) = pal[((tile & 0x0000f000)>>10)+1];
      			      *(where+ 7) = pal[((tile & 0x0000f000)>>10)+2];
      			      *(where+ 8) = pal[((tile & 0x0000f000)>>10)  ]; }
      if(tile & 0x00000f00) { *(where+ 9) = pal[((tile & 0x00000f00)>> 6)+1];
      			      *(where+10) = pal[((tile & 0x00000f00)>> 6)+2];
      			      *(where+11) = pal[((tile & 0x00000f00)>> 6)  ]; }
      if(tile & 0x00f00000) { *(where+12) = pal[((tile & 0x00f00000)>>18)+1];
      			      *(where+13) = pal[((tile & 0x00f00000)>>18)+2];
      			      *(where+14) = pal[((tile & 0x00f00000)>>18)  ]; }
      if(tile & 0x000f0000) { *(where+15) = pal[((tile & 0x000f0000)>>14)+1];
      			      *(where+16) = pal[((tile & 0x000f0000)>>14)+2];
      			      *(where+17) = pal[((tile & 0x000f0000)>>14)  ]; }
      if(tile & 0xf0000000) { *(where+18) = pal[((tile & 0xf0000000)>>26)+1];
      			      *(where+19) = pal[((tile & 0xf0000000)>>26)+2];
      			      *(where+20) = pal[((tile & 0xf0000000)>>26)  ]; }
      if(tile & 0x0f000000) { *(where+21) = pal[((tile & 0x0f000000)>>22)+1];
      			      *(where+22) = pal[((tile & 0x0f000000)>>22)+2];
      			      *(where+23) = pal[((tile & 0x0f000000)>>22)  ]; }
    }
}

// Blit tile solidly, for 4 byte-per-pixel
inline void md_vdp::draw_tile4_solid(int which, int line, unsigned char *where)
{
  unsigned tile, temp, *pal;
  unsigned *wwhere = (unsigned*)where;

  pal = highpal + (which >> 9 & 0x30); // Determine which 16-color palette
  temp = *pal; *pal = highpal[reg[7]&0x3f]; // Get background color

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      *(wwhere  ) = pal[((tile & PIXEL7)>>SHIFT7)];
      *(wwhere+1) = pal[((tile & PIXEL6)>>SHIFT6)];
      *(wwhere+2) = pal[((tile & PIXEL5)>>SHIFT5)];
      *(wwhere+3) = pal[((tile & PIXEL4)>>SHIFT4)];
      *(wwhere+4) = pal[((tile & PIXEL3)>>SHIFT3)];
      *(wwhere+5) = pal[((tile & PIXEL2)>>SHIFT2)];
      *(wwhere+6) = pal[((tile & PIXEL1)>>SHIFT1)];
      *(wwhere+7) = pal[((tile & PIXEL0)>>SHIFT0)];
    } else {
      *(wwhere  ) = pal[((tile & PIXEL0)>>SHIFT0)];
      *(wwhere+1) = pal[((tile & PIXEL1)>>SHIFT1)];
      *(wwhere+2) = pal[((tile & PIXEL2)>>SHIFT2)];
      *(wwhere+3) = pal[((tile & PIXEL3)>>SHIFT3)];
      *(wwhere+4) = pal[((tile & PIXEL4)>>SHIFT4)];
      *(wwhere+5) = pal[((tile & PIXEL5)>>SHIFT5)];
      *(wwhere+6) = pal[((tile & PIXEL6)>>SHIFT6)];
      *(wwhere+7) = pal[((tile & PIXEL7)>>SHIFT7)];
    }
  // Restore the original color
  *pal = temp;
} 

// Blit tile, leaving color zero transparent, for 4 byte per pixel
inline void md_vdp::draw_tile4(int which, int line, unsigned char *where)
{
  unsigned tile, *pal;
  unsigned *wwhere = (unsigned*)where;

  pal = highpal + (which >> 9 & 0x30); // Determine which 16-color palette

  if(which & 0x1000) // y flipped
    line ^= 7; // take from the bottom, instead of the top

  if(reg[12] & 2) // interlace
    tile = *(unsigned*)(vram + ((which&0x7ff) << 6) + (line << 3));
  else
    tile = *(unsigned*)(vram + ((which&0x7ff) << 5) + (line << 2));
  // If the tile is all 0's, why waste the time?
  if(!tile) return;

  // Blit the tile!
  if(which & 0x800) // x flipped
    {
      if(tile & PIXEL7) *(wwhere  ) = pal[((tile & PIXEL7)>>SHIFT7)];
      if(tile & PIXEL6) *(wwhere+1) = pal[((tile & PIXEL6)>>SHIFT6)];
      if(tile & PIXEL5) *(wwhere+2) = pal[((tile & PIXEL5)>>SHIFT5)];
      if(tile & PIXEL4) *(wwhere+3) = pal[((tile & PIXEL4)>>SHIFT4)];
      if(tile & PIXEL3) *(wwhere+4) = pal[((tile & PIXEL3)>>SHIFT3)];
      if(tile & PIXEL2) *(wwhere+5) = pal[((tile & PIXEL2)>>SHIFT2)];
      if(tile & PIXEL1) *(wwhere+6) = pal[((tile & PIXEL1)>>SHIFT1)];
      if(tile & PIXEL0) *(wwhere+7) = pal[((tile & PIXEL0)>>SHIFT0)];
    } else {
      if(tile & PIXEL0) *(wwhere  ) = pal[((tile & PIXEL0)>>SHIFT0)];
      if(tile & PIXEL1) *(wwhere+1) = pal[((tile & PIXEL1)>>SHIFT1)];
      if(tile & PIXEL2) *(wwhere+2) = pal[((tile & PIXEL2)>>SHIFT2)];
      if(tile & PIXEL3) *(wwhere+3) = pal[((tile & PIXEL3)>>SHIFT3)];
      if(tile & PIXEL4) *(wwhere+4) = pal[((tile & PIXEL4)>>SHIFT4)];
      if(tile & PIXEL5) *(wwhere+5) = pal[((tile & PIXEL5)>>SHIFT5)];
      if(tile & PIXEL6) *(wwhere+6) = pal[((tile & PIXEL6)>>SHIFT6)];
      if(tile & PIXEL7) *(wwhere+7) = pal[((tile & PIXEL7)>>SHIFT7)];
    }
} 
#endif // ASM_TILES

// Draw the window (front or back)
void md_vdp::draw_window(int line, int front)
{
  int size;
  int x, y, w, start;
  int pl, add;
  int total_window;
  unsigned char *where;
  int which;
  // Set everything up
  y = line >> 3;
  total_window = (y < (reg[18]&0x1f)) ^ (reg[18] >> 7);

  // Wide or narrow
  size = (reg[12] & 1)? 64 : 32;

  pl = (reg[3] << 10) + ((y&0x3f)*size*2);

  // Wide(320) or narrow(256)?
  if(reg[12] & 1)
    {
      w = 40;
      start = -8;
    } else {
      w = 32;
      start = 24;
    }
  add = -2;
  where = dest + (start * Bpp);
  for(x=-1; x<w; ++x)
    {
      if(!total_window)
	if(reg[17] & 0x80) 
	  {
	    if(x < ((reg[17]&0x1f) << 1)) goto skip;
	  } else {
	    if(x >= ((reg[17]&0x1f) << 1)) goto skip;
	  }
      which = get_word(((unsigned char*)vram) + (pl+(add&((size-1)<<1))));
      if((which>>15) == front)
	draw_tile(which, line&7, where);
skip:
      add += 2; where += Bpp_times8;
    }
}

void md_vdp::draw_sprites(int line, int front)
{
  unsigned which;
  int tx, ty, x, y, xend, ysize, yoff, i;
  unsigned char *where, *sprite;
  // Sprites have to be in reverse order :P
  for(i = sprite_count - 1; i >= 0; --i)
    {
      sprite = sprite_base + (sprite_order[i] << 3);
      // Get the first tile
      which = get_word(sprite + 4);
      // Only do it if it's on the right priority
      if((which >> 15) == front)
	{
	  // Get the sprite's location
	  y = get_word(sprite);
	  x = get_word(sprite + 6) & 0x1ff;

	  // Interlace?
	  if(reg[12] & 2)
	    y = (y & 0x3fe) >> 1;
	  else
	    y &= 0x1ff;

	  x -= 0x80;
	  y -= 0x80;
	  yoff = (line - y);

	  // Narrow mode?
	  if(!(reg[12] & 1)) x += 32;
	  
	  xend = ((sprite[2] << 1) & 0x18) + x;
	  ysize = sprite[2] & 0x3;

	  // Render if this sprite's on this line
	  if(xend > -8 && x < 320 && yoff >= 0 && yoff <= (ysize<<3)+7)
	    {
	      ty = yoff & 7;
	      // y flipped?
	      if(which & 0x1000)
		which += ysize - (yoff >> 3);
	      else
		which += (yoff >> 3);
	      ++ysize;
	      // x flipped?
	      if(which & 0x800)
		{
		  where = dest + (xend * Bpp);
		  for(tx = xend; tx >= x; tx -= 8)
		    {
		      if(tx > -8 && tx < 320)
			draw_tile(which, ty, where);
		      which += ysize;
		      where -= Bpp_times8;
		    }
	        } else {
		  where = dest + (x * Bpp);
		  for(tx = x; tx <= xend; tx += 8)
		    {
		      if(tx > -8 && tx < 320)
			draw_tile(which, ty, where);
		      which += ysize;
		      where += Bpp_times8;
		    }
		} 
	    }
	}
    }
}

// The body for the next few functions is in an extraneous header file.
// Phil, I hope I left enough in this file for GLOBAL to hack it right. ;)
// Thanks to John Stiles for this trick :)

void md_vdp::draw_plane_back0(int line)
{
#define FRONT 0
#define PLANE 0
#include "ras-drawplane.h"
#undef PLANE
#undef FRONT
}

void md_vdp::draw_plane_back1(int line)
{
#define FRONT 0
#define PLANE 1
#include "ras-drawplane.h"
#undef PLANE
#undef FRONT
}

void md_vdp::draw_plane_front0(int line)
{
#define FRONT 1
#define PLANE 0
#include "ras-drawplane.h"
#undef PLANE
#undef FRONT
}

void md_vdp::draw_plane_front1(int line)
{
#define FRONT 1
#define PLANE 1
#include "ras-drawplane.h"
#undef PLANE
#undef FRONT
}

// The main interface function, to generate a scanline
void md_vdp::draw_scanline(struct bmap *bits, int line)
{
  unsigned *ptr, i;
  // Set the destination in the bmap
  dest = bits->data + (bits->pitch * (line + 8) + 16);
  // If bytes per pixel hasn't yet been set, do it
  if(!Bpp)
    {
           if(bits->bpp <= 8)  Bpp = 1;
      else if(bits->bpp <= 16) Bpp = 2;
      else if(bits->bpp <= 24) Bpp = 3;
      else		       Bpp = 4;
      Bpp_times8 = Bpp << 3; // used for tile blitting
#ifdef ASM_TILES
      _asm_tiles_init(vram, reg, highpal); // pass these values to the asm tiles
#endif
    }

  // If the palette's been changed, update it
  if(dirt[0x34] & 2)
    {
      ptr = highpal;
      // What color depth are we?
      switch(bits->bpp)
        {
	case 32:
	case 24:
	  for(i = 0; i < 128; i += 2)
	    *ptr++ = ((cram[i+1]&0x0e) << 20) |
	    	     ((cram[i+1]&0xe0) << 8 ) |
		     ((cram[i]  &0x0e) << 4 );
	  break;
	case 16:
	  for(i = 0; i < 128; i += 2)
	    *ptr++ = ((cram[i+1]&0x0e) << 12) |
	    	     ((cram[i+1]&0xe0) << 3 ) |
		     ((cram[i]  &0x0e) << 1 );
	  break;
	case 15:
	  for(i = 0; i < 128; i += 2)
	    *ptr++ = ((cram[i+1]&0x0e) << 11) |
	    	     ((cram[i+1]&0xe0) << 2 ) |
		     ((cram[i]  &0x0e) << 1 );
	  break;
	case 8:
	default:
	  // Let the hardware palette sort it out :P
	  for(i = 0; i < 64; ++i) *ptr++ = i;
	}
      // Clean up the dirt
      dirt[0x34] &= ~2;
      pal_dirty = 1;
    }
  // Render the screen if it's turned on
  if(reg[1] & 0x40)
    {
      // Recalculate the sprite order, if it's dirty
      if((dirt[0x30] & 0x20) || (dirt[0x34] & 1))
	{
	  unsigned next = 0;
	  // Find the sprite base in VRAM
	  sprite_base = vram + (reg[5]<<9);
	  // Order the sprites
	  sprite_count = sprite_order[0] = 0;
	  do {
	    next = sprite_base[(next << 3) + 3];
	    sprite_order[++sprite_count] = next;
	  // No more than 256 sprites/line, a reasonable limit ;)
	  } while (next && sprite_count < 0x100);
	  // Clean up the dirt
	  dirt[0x30] &= ~0x20; dirt[0x34] &= ~1;
	}
      // Draw, from the bottom up
      // Low priority
      draw_plane_back1(line);
      draw_plane_back0(line);
      draw_window(line, 0);
      draw_sprites(line, 0);
      // High priority
      draw_plane_front1(line);
      draw_plane_front0(line);
      draw_window(line, 1);
      draw_sprites(line, 1);
    } else {
      // The display is off, paint it black
      // Do it a dword at a time
      unsigned *destl = (unsigned*)dest;
      for(i = 0; i < (80 * Bpp); ++i) destl[i] = 0;
    }

  // If we're in narrow (256) mode, cut off the messy edges
  if(!(reg[12] & 1))
    {
      unsigned *destl = (unsigned*)dest;
      for(i = 0; i < Bpp_times8; ++i)
        destl[i] = destl[i + (72 * Bpp)] = 0;
    }
}
