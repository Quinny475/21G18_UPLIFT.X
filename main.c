#include "samd21g18a.h"


int main(){
    
    GCLK_REGS->GCLK_GENCTRL |= 0x00010603;      //enable clk, SRC = 8Mhz OSC, GCLK_GEN3
    GCLK_REGS->GCLK_CLKCTRL |= 0x4305;          //clk enable, generator 3 selected, pointed to the GCLK_EIC peripheral
    
    
    PORT_REGS->GROUP[0].PORT_DIR = (1 << 11) | (1 << 20) | (1 << 21);   // OUTPUT LEDs 
    PORT_REGS->GROUP[0].PORT_OUT = (1 << 11) | (1 << 20) | (1 << 21);   // LEDs are active low, set high to turn off
    PORT_REGS->GROUP[0].PORT_PMUX[0] |= 0x00;   //PA0 multiplex to function A (EIC)
    PORT_REGS->GROUP[0].PORT_PINCFG[0] = 0x07;  //PA0 = input, pull up, multiplex enabled. (Button pulls low)
    PORT_REGS->GROUP[0].PORT_PINCFG[1] = 0x06;  //PA1 = input, pull up
    PORT_REGS->GROUP[0].PORT_OUT |= (1 << 0);   //this is required for some reason
    PORT_REGS->GROUP[0].PORT_OUT |= (1 << 1);   // connects inputs to internal pull up
    
    
    
    // External Interupt Controller (EIC)
    
    
      __enable_irq();
    
    PM_REGS->PM_APBCMASK |= PM_APBAMASK_EIC(1);                 //power to the EIC  
    NVIC_EnableIRQ(EIC_IRQn);                                   //enable EIC interrupts
    NVIC_SetPriority(EIC_IRQn, 5);                              //set a priority, (arbitrary)
    //EIC_REGS->EIC_EVCTRL |= EIC_EVCTRL_EXTINTEO0(1);            //event enabled on EXTINT channel 0 (PA0)
    //EIC_REGS->EIC_INTENCLR |= EIC_INTENCLR_EXTINT0(1);
    EIC_REGS->EIC_INTENSET |= EIC_INTENSET_EXTINT0(1);          //Interrupt enabled EXTINT[0]
    EIC_REGS->EIC_CONFIG[0] |= (EIC_CONFIG_SENSE0_FALL_Val);    //event on a falling edge (PA0)
    
    EIC_REGS->EIC_CTRL |= EIC_CTRL_ENABLE(1);                   //enable EIC
       
    while(1){
        if((PORT_REGS->GROUP[0].PORT_IN & 0x02) == 0){          //read input register port a, mask PA1 bit
            PORT_REGS->GROUP[0].PORT_OUT |= (1 << 20);          //set port A pin 20 high
           
        }else{
            PORT_REGS->GROUP[0].PORT_OUT &= ~(1 << 20);         //clear port A pin 20
        }
    }

    return 0;
}


void EIC_Handler(void){                                 //External Interrupt handlerA
        
    static uint8_t toggle = 0;                          //variable that keeps it's state each time the handler is called
    
    EIC_REGS->EIC_INTFLAG |= EIC_INTFLAG_EXTINT0(1);    //clear the event flag, (yes set it to 1 to clear)
    
    if(toggle == 1){
        PORT_REGS->GROUP[0].PORT_OUT |= (1 << 21);      
    }else{
        PORT_REGS->GROUP[0].PORT_OUT &= ~(1 << 21);
    }
    
    
    toggle = !toggle;
}