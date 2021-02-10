#include "md.h"
int sound_is_okay = 0;


static void ram_load(md& megad)
{

}

int port_main(void)
{
  int c = 0, pal_mode = 0, running = 1, usec = 0,
      wp = 0, rp = 0, start_slot = -1;
  unsigned long long f = 0;
  char *patches = nullptr, *rom = nullptr;
  int demo_record = 0, demo_play = 0, foo;

  // Create the megadrive object
  md megad;
  if(!megad.okay())
    {
      return 1;
    }
  // Load the requested ROM
  if(megad.load(rom))
    {
      return 1;
    }
  // Set untouched pads
  megad.pad[0] = megad.pad[1] = 0xF303F;

  // Load patches, if given
  //if(patches)
  //  {
      //megad.patch(patches);
   // }
  // Fix checksum
  //megad.fix_rom_checksum();
  // Reset
  megad.reset();
  // Set PAL mode
  megad.pal = pal_mode;

  // Cleanup
  megad.unplug();
  YM2612Shutdown();

  // Come back anytime :)
  return 0;
}
