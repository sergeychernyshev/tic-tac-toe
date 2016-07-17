#include <Adafruit_NeoPixel.h>

#define NEOPIXELSPIN 6

int buttons_number = 9;

#define MIN_BRIGHTNESS 127
#define MAX_BRIGHTNESS 255
// brightness to go down to when fading in and out for visual effects
#define FADE_LOW_BRIGHTNESS 50

int brightness = MAX_BRIGHTNESS;

#define END_OF_GAME_FADE_NUMBER 3
#define END_OF_GAME_FADE_STEPS 20
#define END_OF_GAME_TIME_MS 300

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(buttons_number, NEOPIXELSPIN, NEO_GRB + NEO_KHZ800);

// Pins that get triggered when buttons are pressed
int button_pins[] = {
  9, 10, 11, 12, 13, A2, A3, A4, A5
};

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  uint16_t i;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  for(i=0; i < buttons_number; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }
}

int board[] = {
  0, 0, 0,
  0, 0, 0,
  0, 0, 0
};

// starting with player 1, should be either 1 or 2
int player = 1;

int total_moves = 0;

void loop() {
  int i;
  int any_button_pressed = 0;
  int winner = 0;
  
  for(i = 0; i < buttons_number; i++) {
    // only allow pressing unpressed buttons
    if (board[i] == 0 && digitalRead(button_pins[i]) == LOW) {
      board[i] = player;
      any_button_pressed = 1;
      total_moves += 1;

      if (board[i] == 1) {
        strip.setPixelColor(i, strip.Color(0, 0, brightness)); // blue
      }
      
      if (board[i] == 2) {
        strip.setPixelColor(i, strip.Color(0, brightness, 0)); // green
      }

      if (player == 1) {
        player = 2;
      } else {
        player = 1;
      }
    }
  }
  
  // only update strip once per loop and only if at least one button was updated
  if (any_button_pressed == 1) {
    strip.show();
  }

  winner = got_winner();
  if (winner != 0) {
    flash_finish(winner);
  }

  // all buttons were pressed, nobody won
  if (total_moves == buttons_number) {
    flash_finish(0);
  }

  //delay(25);
}

int got_winner() {
  int test;
  int total_tests = 8;
  int starts[] = {0, 1, 2, 0, 3, 6, 0, 2};
  int increments[] = {3, 3, 3, 1, 1, 1, 4, 2};
  
  int s, i;
  
  for (test = 0; test < total_tests; test++) {
    if (board[starts[test]] > 0 &&
        board[starts[test]] == board[starts[test] + increments[test]] && 
        board[starts[test]] == board[starts[test] + increments[test] * 2]) {
      return board[starts[test]];
    }
  }
  
  return 0;
}

void flash_finish(int winner) {
  int i;
  int fade;
  int fade_brightness;
  int fade_brightness_step = (brightness - FADE_LOW_BRIGHTNESS) / END_OF_GAME_FADE_STEPS;
  int fade_brightness_step_time = END_OF_GAME_TIME_MS / END_OF_GAME_FADE_STEPS / 2;

  for (fade = 0; fade < END_OF_GAME_FADE_NUMBER; fade++) {
    // run fade twice - up, then down
    for (fade_brightness = brightness;
      fade_brightness > FADE_LOW_BRIGHTNESS;
      fade_brightness -= fade_brightness_step
    ) {
      for(i = 0; i < buttons_number; i++) {
        if (board[i] == 1) {
          if (winner == 2) {
            strip.setPixelColor(i, strip.Color(0, 0, FADE_LOW_BRIGHTNESS)); // faded blue
          } else {
            strip.setPixelColor(i, strip.Color(0, 0, fade_brightness)); // blue
          }
        }
        
        if (board[i] == 2) {
          if (winner == 1) {
            strip.setPixelColor(i, strip.Color(0, FADE_LOW_BRIGHTNESS, 0)); // faded green
          } else {
            strip.setPixelColor(i, strip.Color(0, fade_brightness, 0)); // green
          }
        }
      }
      
      strip.show();
      delay(fade_brightness_step_time);
    }
 
    for (fade_brightness = FADE_LOW_BRIGHTNESS;
      fade_brightness < brightness;
      fade_brightness += fade_brightness_step
    ) {
      for(i = 0; i < buttons_number; i++) {
        if (board[i] == 1) {
          if (winner == 2) {
            strip.setPixelColor(i, strip.Color(0, 0, FADE_LOW_BRIGHTNESS)); // faded blue
          } else {
            strip.setPixelColor(i, strip.Color(0, 0, fade_brightness)); // blue
          }
        }
        
        if (board[i] == 2) {
          if (winner == 1) {
            strip.setPixelColor(i, strip.Color(0, FADE_LOW_BRIGHTNESS, 0)); // faded green
          } else {
            strip.setPixelColor(i, strip.Color(0, fade_brightness, 0)); // green
          }
        }
      }
      
      strip.show();
      delay(fade_brightness_step_time);
    }
  }

  for(i = 0; i < buttons_number; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // off
    board[i] = 0;
  }
  
  strip.show();
  
  total_moves = 0;
  player = 1;
}
