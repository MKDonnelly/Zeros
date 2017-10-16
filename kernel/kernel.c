asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.


void kmain(){
   char* video_memory = (char*)0xb8000;
   *video_memory = 'X';
}
