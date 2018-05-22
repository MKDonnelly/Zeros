#include <arch/x86/drivers/timer.h>

#include <arch/x86/portio.h>
#include <arch/x86/pmode/isr.h>
#include <arch/x86/drivers/pic.h>

void arch_timer_init( void (*timer_callback)(context_t) ){

   //Set the timer to operate at 100HZ
   set_timer_count( I8253_10MS_COUNTER );

   //Enable the timer interrupt and 
   //set the interrupt handler
   enable_irq( TIMER_IRQ );
   arch_register_interrupt( TIMER_INTERRUPT, timer_callback );
}

void set_timer_count(uint16_t count){

   //Select Channel 0, set the access mode to
   //both low/high byte, set it to operate in
   //mode 3, and use binary instead of BCD 
   portb_write( I8253_CTRL_P, I8253_CTRL_CH0_M |
                              I8253_CTRL_LHBYTE_M |
                              I8253_CTRL_MODE3_M |
                              I8253_CTRL_BIN_M );

   //Write the frequency to the timer
   int8_t lowerByte = (uint8_t)(count & 0xFF );
   int8_t higherByte = (uint8_t)( (count >> 8) & 0xFF );
   portb_write( I8253_CH0_P, lowerByte );
   portb_write( I8253_CH0_P, higherByte );
}
