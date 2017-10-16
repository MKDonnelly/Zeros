asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

void k_printfull(char*, int, int);

void kmain(){
   //char* vidmem = (char*)0xb8000;
   //*vidmem = 'X';
   k_printfull("Testing...", 0, 0);
}

unsigned int next_vidmem_location = 0;

//prints text (msg) with the given color (color),
//indent from the left edge of the screen (indent).
//Newline dictates if text after this should start 
//on the next line
void k_printfull(char *msg, int indent, int newline){

   //Get the start of the video memory
   char *vidmem = (char*)0xb8000;

   //Calculate the offset from the left edge of the screen 
   //(takes care of indent) 
   unsigned int offset = next_vidmem_location + indent * 2;

   //Print each character one at a time
   //setting the given color attribute
   int i;
   for(i = 0; msg[i] != 0; i++ ){
      vidmem[offset + i * 2] = msg[i];
      vidmem[offset + i * 2 + 1] = 15;
   }

   //Update the next available memory
   //location by adding the number
   //of characters in this string to it
   next_vidmem_location += i;
}

