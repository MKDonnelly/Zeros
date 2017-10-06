
char *VIDEO_MEMORY = (char*)0xb8000;
int CUR_SCREEN_OFFSET = 0;

void k_putchar( char input ){
   *(VIDEO_MEMORY + CUR_SCREEN_OFFSET) = input;
   CUR_SCREEN_OFFSET += 2; //Skip 2 bytes since 
                           //the second byte is for color
}

//This functions alters SCREEN_LOCATION to
//move it to the next line. There is a new
//line every 160 positions.
void k_newline(){
  int current_line = (int)( CUR_SCREEN_OFFSET / 160 );
  CUR_SCREEN_OFFSET = ( (current_line + 1) * 160); 
}




