/*
 * sys_init.c
 *
 *  Created on: Oct 15, 2018
 *      Author: ASUS
 */
#include <msp430.h>
#include "flappy.h"

unsigned int t1=0, t2=0, distance=0, first_pulse=0;

uint8_t display_map[8];
int Car_P;
int star,i,n,Location,min,max,k;
uint16_t adc_result;
uint16_t volt;

void sys_init(void)
{
    Config_stop_WDT();

    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;          // set range
    DCOCTL = CALDCO_16MHZ;           // set DCO step + moudulation

    Config_SPI_A();

    P1SEL &= ~BIT3;
    P1SEL2 &= ~BIT3;
    P1DIR |= BIT3;                  //LAT pin
    P1OUT &= ~BIT3;
    P2SEL &=~(BTN2_1 + BTN2_2 +BTN2_3);
    P2SEL2 &= ~(BTN2_1 + BTN2_2 +BTN2_3);
    P2DIR &= ~(BTN2_1 + BTN2_2 +BTN2_3);
    P2IE  |= (BTN2_1 + BTN2_2 +BTN2_3);  // khai bao interrupt
    P2IES |= (BTN2_1 + BTN2_2 +BTN2_3);  // bat canh xuong
    P2IFG &= ~(BTN2_1 + BTN2_2 +BTN2_3); // keo co ve lai bang

    P2DIR |= trigger;      // trigger ouput
    P2SEL |= echo;         //connect P2.0 (echo) to CCI0A (capture/compare input )

    _BIS_SR(GIE);          //ngat toan cuc
}

void begin(volatile sys_params_t *params)
{
    for(i=0;i<8;i++)
    {
        Send_byte_A(params->led_control[i],1);
        Send_byte_A(params->begin_img[i],1);
        Send_byte_A(params->off_img[i],1);
        P1OUT |= BIT3;
        __delay_cycles(1000);
        P1OUT &= ~BIT3;
    }
}

void end(volatile sys_params_t *params)
{
    for(i=0;i<8;i++)
    {
        Send_byte_A(params->led_control[i],1);
        Send_byte_A(params->end_img[i],1);
        Send_byte_A(params->off_img[i],1);
        P1OUT |= BIT3;
        __delay_cycles(100);
        P1OUT &= ~BIT3;
    }
}

void play(volatile sys_params_t *params)
{
    for(star=0;star<32;star++)
    {
//        P2OUT |= trigger;       // Trigger measure time
//        __delay_cycles(20);
//        P2OUT &= ~trigger;
//        __delay_cycles(10000);
        car_position(distance);
        for(k=star;k<(star+8);k++)
        {
            display_map[k-star] = params->game_map[k];
        }
//        if((display_map[star+1] | Car_P) == Car_P) // Conlission check
//           break;
        display_map[1] = display_map[1] & Car_P;
        for (n=0;n<500;n++)
        {
            for(i=0;i<8;i++)
            {
                Send_byte_A(params->led_control[i],1);
                Send_byte_A(sys_params.off_img[i],1);
                Send_byte_A(display_map[i],1);
                P1OUT |= BIT3;
                __delay_cycles(100);
                P1OUT &= ~BIT3;
            }
            __delay_cycles(1);
        }
    }
    sys_params.iState=2;
}

void car_position(int distance)
{
    if(distance < 10)
    {
        Car_P = 0b11011111;
    }
    else if(distance>=10 & distance <15)
        {
            Car_P = 0b11101111;
        }
    else if(distance>=15 & distance < 20)
        {
            Car_P = 0b11110111;
        }
    else if(distance >= 20)
        {
            Car_P = 0b11111011;
        }
}

void init_timer(void){
   TA1CTL = TASSEL_2 + MC_2 ;                       // continuos mode, 0 --> FFFF
   TA1CCTL0 = CM_3 + CAP + CCIS_0 + SCS+ CCIE;      // falling edge & raising edge, capture mode, capture/compare interrupt enable
   TA1CCTL0 &= ~ CCIFG;
}

void image_copy(uint8_t *a, uint8_t *b)
{
    int i;
    for(i=0; i < 8;i++)
        a[i] = b[i];
}
void map_copy(uint8_t *a, uint8_t *b)
{
    int i;
    for(i=0; i < 32;i++)
        a[i] = b[i];
}

#pragma vector = PORT2_VECTOR// nho
__interrupt void BUTTON_Interrupt_Handle(void)// nho
{
    if ((P2IFG & BTN_1) == BTN_1)
    {
        sys_params.iState++;
        P2IFG &= ~BTN_1;
    }
    P2IFG = 0;
}
#pragma vector = TIMER1_A0_VECTOR
__interrupt void timer0(void){

   if (P2IN & BIT0 ==1 )      //  neu co xung canh len tai chan echo (P2.0)
      t1 = TA1CCR0;

   else {                  // neu co canh xuong tai chan echo (P2.0)
      t2 = TA1CCR0;
      if (t2 > t1){
         distance = (t2-t1)/58;

      }

      }

   TA1CCTL0 &= ~ CCIFG;
}
