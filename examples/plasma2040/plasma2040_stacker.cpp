#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
****LED STRIP STACKER GAME****
Button a Begins game

push button when the red and yellow leds cross each other.
see how far you get...
by Gee 'Rabid Inventor' Bartlett

*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 40;
const uint REFRESH_DELAY = 100;

// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
// by default the WS2812 LED strip will be 400KHz, RGB with no white element
//WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);

//Uncomment for WS2812 with RGBW running at 800KHz
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, 800000, true);

Button button_a(plasma2040::BUTTON_A, Polarity::ACTIVE_LOW, 50);
Button button_b(plasma2040::BUTTON_B, Polarity::ACTIVE_LOW, 50);
RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);


class playfield_object{
    public:
    uint8_t position = 0;
    uint8_t length = 0;
    int move_direction = 1;
    uint8_t speed = 0;
    uint8_t colour_R = 0;
    uint8_t colour_G = 0;
    uint8_t colour_B = 0;
    int is_present_at(uint8_t test_position){
        int ret = 0;
        if (test_position >= position){
            if (test_position <= position + length){
                ret = 1;
            }
        }
        return ret;
    }
};

class levels{
    public:
    uint8_t player_speed = 0;
    uint8_t player_speed_increase_rate = 0;
    bool moving_target = false;
    uint8_t target_speed = 0;
    uint8_t target_speed_increase_rate = 0;
    bool moving_playfield = false;
    uint8_t playfield_speed = 0;
    uint8_t playfield_speed_increase_rate = 0;
};

class StackerGame{
    public:
    enum states{
        ATTRACT_MODE,
        GAME_STARTED
    };
    levels begining_level;
    uint8_t playfield_width;
    int current_step = 0;
    states game_state = ATTRACT_MODE;
    int score = 0;
    uint8_t game_speed = 0;
    playfield_object player;
    playfield_object target;
    playfield_object left_wall;
    playfield_object right_wall;

    void start(levels game_level);
    void reset(levels game_level);
    void game_step();
    void advance_round();
    void stop();
};

void StackerGame::start(levels game_level){
    game_state = ATTRACT_MODE;
    begining_level = game_level;
    playfield_width = N_LEDS;
    player.position = 5;
    player.length = 10;
    target.position = 15;
    target.length = 10;
    left_wall.position = 0;
    left_wall.length = 3;
    right_wall.position = playfield_width - left_wall.length;
    right_wall.length = left_wall.length;
    player.speed = begining_level.player_speed;
    target.speed = begining_level.target_speed;

    //object colours
    player.colour_R = 0;
    player.colour_B = 0;
    player.colour_G = 50;

    target.colour_R = 50;
    target.colour_G = 50;
    target.colour_B = 0;

    left_wall.colour_R = 0;
    left_wall.colour_G = 0;
    left_wall.colour_B = 50;

    right_wall.colour_R = 0;
    right_wall.colour_G = 0;
    right_wall.colour_B = 50;

}

void StackerGame::reset(levels game_level){
    begining_level = game_level;
    playfield_width = N_LEDS;
    player.position = 5;
    player.length = 10;
    target.position = 15;
    target.length = 10;
    left_wall.position = 0;
    left_wall.length = 3;
    right_wall.position = playfield_width - left_wall.length;
    right_wall.length = left_wall.length;
    player.speed = begining_level.player_speed;
    target.speed = begining_level.target_speed;
    begining_level.moving_target = false;

    //object colours
    player.colour_R = 0;
    player.colour_B = 0;
    player.colour_G = 50;

    target.colour_R = 50;
    target.colour_G = 50;
    target.colour_B = 0;

    left_wall.colour_R = 0;
    left_wall.colour_G = 0;
    left_wall.colour_B = 50;

    right_wall.colour_R = 0;
    right_wall.colour_G = 0;
    right_wall.colour_B = 50;


}

void StackerGame::stop(){
    sleep_ms(500);
    game_state = ATTRACT_MODE;
    reset(begining_level);
    
    
    
}

void StackerGame::advance_round(){

    player.speed = player.speed + begining_level.player_speed_increase_rate;
    
    if (player.speed > 240){ //higher level stuff
        begining_level.moving_target = true;
        player.speed = begining_level.player_speed;
        target.speed = target.speed + begining_level.target_speed_increase_rate;
    }
    
    

}

void StackerGame::game_step(){
    current_step++;

    bool a = button_a.read();

    if (game_state == GAME_STARTED){
        if (a){
            if (player.position == target.position)
            {
            advance_round();
            }
            else if (player.position < target.position ){
                if (player.position + player.length >= target.position){
                    target.length = target.length - (target.position - player.position);
                    player.length = target.length;
                    sleep_ms(200);
                    advance_round();

                }
                else{
                    stop();
                }
            }
            
            else if (player.position > target.position){
                if(player.position < target.position + target.length){
                    
                    target.length = target.length - (player.position - target.position);
                    target.position = player.position;
                    player.length = target.length;
                    sleep_ms(200);
                    advance_round();

                }
                else{
                    stop();
                }
            }
        }
            
    
        
            
            // aminate player and target ingame
            if (player.position + player.length >= right_wall.position){
                player.move_direction = -1;
            }
            if (player.position <= left_wall.position + left_wall.length){
                player.move_direction = 1;
            }
            if (current_step % (254 - player.speed) == 0){
                player.position += player.move_direction;
            }


            if (begining_level.moving_target){
                if (target.position + target.length >= right_wall.position){
                    target.move_direction = -1;
                }
                if (target.position <= left_wall.position + left_wall.length){
                    target.move_direction = 1;
                }
                if (current_step % (254 - target.speed) == 0){
                    target.position += target.move_direction;
                }
            }
        
    }

    else if (game_state == ATTRACT_MODE){

        if (a){
            game_state = GAME_STARTED;
            reset(begining_level);
            sleep_ms(2000);
        }
        
        // aminate player and target attract mode
        if (player.position + player.length >= right_wall.position){
            player.move_direction = -1;
        }
        if (player.position <= left_wall.position + left_wall.length){
            player.move_direction = 1;
        }
        if (current_step % (254 - 150) == 0){
            player.position += player.move_direction;
        }


        
        if (target.position + target.length >= right_wall.position){
            target.move_direction = -1;
        }
        if (target.position <= left_wall.position + left_wall.length){
            target.move_direction = 1;
        }
        if (current_step % (254 - 50) == 0){
            target.position += target.move_direction;
        }


    

    }
    

}

int main() {
    stdio_init_all();

    led_strip.start(60);

    
    StackerGame stacker; // create instance of stacker game

    //create level instance this is where you can adjust beginning dificulty
    levels level_1;
    level_1.player_speed = 70;
    level_1.player_speed_increase_rate = 10;
    level_1.moving_target = false;
    level_1.target_speed = 20;
    level_1.target_speed_increase_rate = 10;

    stacker.start(level_1);

    


    while (true) {
        //do game loop
        stacker.game_step();

        if (stacker.game_state == stacker.ATTRACT_MODE){
            bool a = button_a.read();
            if (a){
                
                stacker.game_state = stacker.GAME_STARTED;
                stacker.start(level_1);
                sleep_ms(2000);
            }
        }
        
        
        // leds update routine 
        for(auto i = 0u; i < led_strip.num_leds; ++i) {

            if (stacker.target.is_present_at(i)){
                led_strip.set_rgb(i, stacker.target.colour_R, stacker.target.colour_G, stacker.target.colour_B);
                
                if (stacker.player.is_present_at(i)){
                    led_strip.set_rgb(i, stacker.target.colour_R - stacker.player.colour_R, 
                                         stacker.target.colour_G - stacker.player.colour_G,
                                         stacker.target.colour_B - stacker.player.colour_B);
                }
            }
            else if (stacker.player.is_present_at(i)){
                led_strip.set_rgb(i, stacker.player.colour_R, stacker.player.colour_G, stacker.player.colour_B);
            }
            else if (stacker.left_wall.is_present_at(i)){
                led_strip.set_rgb(i, stacker.left_wall.colour_R, stacker.left_wall.colour_G, stacker.left_wall.colour_B);
            }

            else if (stacker.right_wall.is_present_at(i)){
                led_strip.set_rgb(i, stacker.right_wall.colour_R, stacker.right_wall.colour_G, stacker.right_wall.colour_B);
            }
            else{
                led_strip.set_rgb(i, 0, 0, 0);
            }
            
            
        }
            
        // Sleep time controls the rate at which the LED buffer is updated
        // but *not* the actual framerate at which the buffer is sent to the LEDs
        sleep_ms(1);
    }
}
