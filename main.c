#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include "lib/flappy.h"
#include "lib/data.h"

uint8_t game_map[32]=
{
 0b10111101,
 0b10111101,
 0b10111101,
 0b10001101,
 0b10111101,
 0b10111101,
 0b10110001,
 0b10111101,
 0b10111101,
 0b10001101,
 0b10111101,
 0b10111101,
 0b10100001,
 0b10111101,
 0b10111101,
 0b10000101,
 0b10111101,
 0b10111101,
 0b10001101,
 0b10001101,
 0b10001101,
 0b10111101,
 0b10111101,
 0b10110001,
 0b10111101,
 0b10000101,
 0b10111101,
 0b10111001,
 0b10110001,
 0b10100001,
 0b10100001,
 0b10111101
};
uint8_t smile[8]=   {0b10000001,0b01111110,0b01011010,0b01111110,0b01011010,0b01100110,0b01111110,0b10000001};
uint8_t sad[8]  =   {0b10000001,0b01111110,0b01011010,0b01111110,0b01100110,0b01011010,0b01111110,0b10000001};
uint8_t led_control[8]  =   {0b10000000,0b01000000,0b00100000,0b00010000,0b00001000,0b00000100,0b00000010,0b00000001};
uint8_t test_on[8] =    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000};
uint8_t test_off[8] =   {0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111};



void main(void)
{
    sys_init();

    image_copy(sys_params.begin_img,smile);
    image_copy(sys_params.end_img,sad);
    image_copy(sys_params.led_control,led_control);
    image_copy(sys_params.off_img,test_off);
    image_copy(sys_params.on_img,test_on);
    map_copy(sys_params.game_map, game_map);

    while(1)
    {
        switch(sys_params.iState%3)
        {
        case 0:
            begin(&sys_params);
            break;
        case 1:
            play(&sys_params);
            break;
        case 2:
            end(&sys_params);
            break;
        default:
            break;
        }
    }


}

