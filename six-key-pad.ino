#include <HID-Project.h>

/*
1 - Vol down 
2 - mute
3 - Vol up
4 - previous track
5 - Play/pause
6 - next track
*/

int button_pins[6] = { 2, 3, 4, 5, 6, 7 } ;
unsigned long pin_state[6] = { 0, 0, 0, 0, 0, 0 };

#define KEY_UP_ARROW 0xDA
#define KEY_DOWN_ARROW 0xD9
#define KEY_LEFT_ARROW 0xD8
#define KEY_RIGHT_ARROW 0xD7
#define KEY_RETURN 0xB0

int key_code[6] = { 
  MEDIA_VOL_DOWN, 
  MEDIA_VOL_MUTE, 
  MEDIA_VOL_UP, 
  MEDIA_PREVIOUS, 
  MEDIA_PLAY_PAUSE, 
  MEDIA_NEXT };

int led_pin = 21;
int shift_status = 0;

#define LED_OFF 0
#define LED_ON  128

void setup() {
  int cnt;

  for (cnt = 0; cnt < 6; cnt++) {
    pinMode(button_pins[cnt], INPUT_PULLUP);
    digitalWrite(button_pins[cnt], HIGH);
  }

  pinMode(led_pin, OUTPUT);
  analogWrite(led_pin, (shift_status == 0 ? LED_OFF : LED_ON));
  Consumer.begin();
}

bool debounce(unsigned long t_now, unsigned long t_prev) {
  unsigned long diff;

  diff = t_now - t_prev; // need to check for underflow?

  if (diff <= 20) return true;
  else return false;
}

void loop() {
  unsigned long tick_now = millis();
  int cnt;

  // since we use non zero to indicate pressed state, we need
  // to handle the edge case where millis() returns 0
  
  if (tick_now == 0) tick_now = 1;

  for (cnt = 0; cnt < 6; cnt++) {
    int key_state = 0;

    // ignore state change for pin if in debounce period
    if (pin_state[cnt] != 0)
      if (debounce(tick_now, pin_state[cnt]) == true)
        continue;
      
    if (digitalRead(button_pins[cnt]) == LOW) {
      if (pin_state[cnt] != 0) {
        pin_state[cnt] = 0;
      }
    } 
    else {
      if (pin_state[cnt] == 0) {
        if (key_code[cnt] != 0) {
          Consumer.write(key_code[cnt]);
        } 
        else {
          if (cnt == 0) {
            analogWrite(led_pin, (shift_status == 0 ? LED_OFF : LED_ON));
          }
        }
        pin_state[cnt] = tick_now;
      }
    }
  }
  delay(1);
}
