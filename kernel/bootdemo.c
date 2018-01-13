#include <bootdemo.h>

void delay_print(char *string, int x, int y){
   int i = 0;
   while( string[i] != 0 ){
      k_delayms(80);
      k_putchar_at( string[i], x++, y);
      i++;
   }
}

void demo(){

  k_printf("Initilizing VGA .......... [ OK ]\n");
  k_delays(1);
  k_printf("Setting up GDT  .......... [ OK ]\n");
  k_delays(1);
  k_printf("Installing Interrupts .... [ OK ]\n");
  k_delays(1);
  k_printf("Remapping PIC   .......... [ OK ]\n");
  k_delays(1);
  k_printf("Initilizing Timer ........ [ OK ]\n");
  k_delays(1);
  k_printf("Setting up Keyboard ...... [ OK ]\n");
  k_delays(1);
  k_printf("Setting up heap .......... [ OK ]\n");
  k_delays(1);
  k_printf("Starting Interrupts ...... [ OK ]\n");
  k_delays(1);
  k_printf("All Systems successfully started!\n");
  k_delays(3);



  k_clear_screen();
  delay_print("In the beginning was the hacker", 23, 8);
  k_delays(1);
  delay_print("and the hacker was all there was.", 23, 9);
  k_delays(2);

  k_clear_screen();

  delay_print("And so the hacker brought into existance", 23, 8);
  k_delays(1);
  delay_print("a system with which to exert his will.", 22, 9);
  k_delays(2);

  k_printf_at("============", 10, 15);
  k_printf_at("+16-bit CPU+", 10, 16);
  k_printf_at("============", 10, 17); 
  k_printf_at("==============================", 24, 15);
  k_printf_at("+        1M Memory           +", 24, 16);
  k_printf_at("==============================", 24, 17);
  k_printf_at("========", 56, 15);
  k_printf_at("+ BIOS +", 56, 16);
  k_printf_at("========", 56, 17);
  k_delays(2);


  delay_print("And so the hacker began to abstract the system", 18, 8);
  k_delays(1);
  delay_print("upon the baren landscape. Many difficulties", 18, 9);
  k_delays(1);
  delay_print("slowed his progress, all conquered one by one", 25, 10);
  k_delays(2);

  k_clear_screen();
  
  k_printf_at("============", 10, 15);
  k_printf_at("+16-bit CPU+", 10, 16);
  k_printf_at("============", 10, 17); 
  k_printf_at("==============================", 24, 15);
  k_printf_at("+        1M Memory           +", 24, 16);
  k_printf_at("==============================", 24, 17);
  k_printf_at("========", 56, 15);
  k_printf_at("+ BIOS +", 56, 16);
  k_printf_at("========", 56, 17);
  k_delays(2);

  delay_print("Having extended the base system, the hacker", 18, 8);
  delay_print("lifted himself to 32-bit mode", 20, 9);

  k_delays(3);

  for(int i = 0; i < 20; i++){
     k_scroll_screen();
     k_delays(1);
  }


  k_printf_at("============", 10, 14);
  k_printf_at("+32-bit CPU+", 10, 15);
  k_printf_at("============", 10, 16); 
  k_printf_at("==============================", 24, 14);
  k_printf_at("+        4G Memory           +", 24, 15);
  k_printf_at("==============================", 24, 16);
 
  k_delays(2); 
  
  delay_print("Upon this base would be placed paging,", 25, 4);
  k_printf_at("          <Paging>            ", 24, 8);
  k_printf_at("==============================", 24, 9);
  k_printf_at("+      Virtual Memory        +", 24, 10);
  k_printf_at("==============================", 24, 11);
  k_printf_at("\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/", 24, 12);
  k_printf_at("/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\", 24, 13);
  k_delays(1);

  delay_print("interrupts, ", 25, 5);  
  k_printf_at("============", 55, 14);
  k_printf_at("+Interrupts+", 55, 15);
  k_printf_at("============", 55, 16); 

  delay_print("a timer.", 38, 5);
  k_printf_at("=======", 12, 11);  
  k_printf_at("+Timer+", 12, 12);
  k_printf_at("=======", 12, 13);
  k_delays(1);

  delay_print("With this base,", 48, 5);
  delay_print("processes and threads could exist", 25, 6);
  k_delays(1);

  k_clear_screen();  

  k_printf_at("============", 10, 14);
  k_printf_at("+32-bit CPU+", 10, 15);
  k_printf_at("============", 10, 16); 
  k_printf_at("==============================", 24, 14);
  k_printf_at("+        4G Memory           +", 24, 15);
  k_printf_at("==============================", 24, 16);
  k_printf_at("          <Paging>            ", 24, 8);
  k_printf_at("==============================", 24, 9);
  k_printf_at("+      Virtual Memory        +", 24, 10);
  k_printf_at("==============================", 24, 11);
  k_printf_at("\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/\\|/", 24, 12);
  k_printf_at("/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\/|\\", 24, 13);
  k_printf_at("============", 55, 14);
  k_printf_at("+Interrupts+", 55, 15);
  k_printf_at("============", 55, 16); 
  k_printf_at("=======", 12, 11);  
  k_printf_at("+Timer+", 12, 12);
  k_printf_at("=======", 12, 13);

  k_delays(2);

  delay_print("And so the hacker rested, for his creation", 25, 4);
  delay_print("had come far.", 35, 5);
  k_delays(2);

  k_clear_screen();

  delay_print("Intelligence is not the accrual of knowledge;", 25, 1);
  k_delays(1);
  delay_print("Intelligence is the most difficult challenge conquered.", 20, 2);

  k_delays(2);

/*
  for(int i = 0; i < 18; i++){
     k_printf_at("                                                                               ", 0, i);
     k_delayms(500);
  }
*/
  
  char bigbuf[40][80] = { \
"   L                                                                            ",
"   k                                           $                                ",
"   .                                           j                                ",
"   -                                           l                                ",
"   /                                           @                                ",
"   2                                           z                                ",
"   5           #                               5                                ",
"   s           %                               1                                ",
"               i                               h                                ",
"               :                               i                                ",
"               |                                                                ",
"               a              <                                                 ",
"               e              G                                -                ",
"               ~              S                                {                ",
"                              f                                @                ",
"                              ^                                A                ",
"  o                           )                                V                ",
"  l                   U       -                                n                ",
"  3                   %                                        B                ",
"  !                   #                                        /                ",
"  =                   P                   &                                     ",
"  ?                   *                   (                                     ",
"  p                   l                   '                                     ",
"  7                                       n                                     ",
"  &                                       *                                     ",
"                                          +                                     ",
"                                          m                                     ",
"                                          a                                     ",
"                                          f                                     ",
"                                          /                                     ",
"               a                                        l                       ",
"               j                                        %                       ",
"               T                                        #                       ",
"               ?                                        Q                       ",
"               #                                        e                       ",
"               Z               &                        o                       ",
"               }               #                        +                       ",
"               >               F                                                ",
"                               Z                                                ",
"                               n                                                "};

  int start_line = 40;
  int iterations_done = 0;  
  k_set_fg_color( VGA3H_GREEN );

  while(iterations_done <= 70){
     for(int scr_idx = 0, bufidx = start_line, count = 0; count < 25; count++, scr_idx++, bufidx++){
        if( bufidx >= 40 )
           bufidx = 0;
        k_printf_at( bigbuf[bufidx], 0, scr_idx );
     }
     start_line--;
     if(start_line < 0)
        start_line = 40;
     iterations_done++;     
     k_delayms(150);
  }


  //Prints 
  // "This is ring 0
  //  This is kernel mode"
  //By scatter-printing each char
  k_printf_at("T", 30, 12);
  k_delayms(500);
  k_printf_at("s", 34, 13);
  k_delayms(500);
  k_printf_at("i", 39, 12);
  k_delayms(500);
  k_printf_at("e", 37, 13);
  k_delayms(500);
  k_printf_at("i", 32, 12);
  k_delayms(500);
  k_printf_at("d", 45, 13);
  k_delayms(500);
  k_printf_at("r", 38, 13);
  k_delayms(500);
  k_printf_at("s", 36, 12);
  k_delayms(500);
  k_printf_at("h", 29, 13);
  k_delayms(500);
  k_printf_at(" ", 42, 13);
  k_printf_at("m", 43, 13);
  k_delayms(500);
  k_printf_at("k", 36, 13);
  k_delayms(500);
  k_printf_at("s", 33, 12);
  k_delayms(500);
  k_printf_at("s", 31, 13);
  k_delayms(500);
  k_printf_at("o", 44, 13);
  k_delayms(500);
  k_printf_at("r", 38, 12);
  k_delayms(500);
  k_printf_at("n", 39, 13);
  k_delayms(500);
  k_printf_at("i", 33, 13);
  k_delayms(500);
  k_printf_at("n", 40, 12);
  k_delayms(500);
  k_printf_at("i", 30, 13);
  k_delayms(500);
  k_printf_at("i", 35, 12);
  k_delayms(500);
  k_printf_at("l", 41, 13);
  k_delayms(500);
  k_printf_at(" ", 42, 12);
  k_printf_at("0", 43, 12);
  k_delayms(500);
  k_printf_at("T", 28, 13);
  k_delayms(500);
  k_printf_at("h", 31, 12);
  k_delayms(500);
  k_printf_at("e", 40, 13);
  k_delayms(500);
  k_printf_at("g", 41, 12);
  k_delayms(500);
  k_printf_at("e", 46, 13);

  k_delays(3);
  k_clear_screen();
}
