#include QMK_KEYBOARD_H
#include "version.h"
#include "process_leader.h"
#include "math.h"
#define MOON_LED_LEVEL LED_LEVEL
#ifndef ZSA_SAFE_RANGE
#define ZSA_SAFE_RANGE SAFE_RANGE
#endif

enum custom_keycodes {
  RGB_SLD = ZSA_SAFE_RANGE,
  HSV_168_255_255,
  ST_MACRO_0,
  ST_MACRO_1,
  ST_MACRO_2,
  ST_MACRO_3,
  ST_MACRO_4,
  ST_MACRO_5,
  ST_MACRO_6,
  ST_MACRO_7,
  ST_MACRO_8,
  ST_MACRO_9,
  ST_MACRO_10,
  ST_MACRO_11,
  ST_MACRO_12,
  DRAG_SCROLL,
  TOGGLE_SCROLL,
  NAVIGATOR_INC_CPI,
  NAVIGATOR_DEC_CPI,
  NAVIGATOR_TURBO,
  NAVIGATOR_AIM
};



enum tap_dance_codes {
  DANCE_0,
  DANCE_1,
  DANCE_2,
  DANCE_3,
  DANCE_4,
  DANCE_5,
  DANCE_6,
  DANCE_7,
  DANCE_8,
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_voyager(
    MT(MOD_LGUI, KC_ESCAPE),KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                                           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINUS,
    CW_TOGG,        KC_Q,           KC_W,           KC_F,           KC_P,           KC_B,                                           KC_J,           KC_L,           KC_U,           KC_Y,           KC_SCLN,        KC_BSLS,
    KC_LEFT_SHIFT,  KC_A,           KC_R,           KC_S,           KC_T,           KC_G,                                           KC_M,           KC_N,           KC_E,           KC_I,           KC_O,           MT(MOD_RSFT, KC_QUOTE),
    KC_LEFT_CTRL,   MT(MOD_LALT, KC_Z),KC_X,           KC_C,           KC_D,           KC_V,                                           KC_K,           KC_H,           KC_COMMA,       KC_DOT,         MT(MOD_RALT, KC_SLASH),KC_ENTER,
                                                    LT(1, KC_TAB),  QK_LEAD,                                       KC_SPACE,       TD(DANCE_0)
  ),
  [1] = LAYOUT_voyager(
    MT(MOD_LGUI, KC_ESCAPE),KC_F1,          KC_F2,          KC_F3,          KC_F4,          TD(DANCE_1),                                    KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         TD(DANCE_5),
    QK_LLCK,        KC_INSERT,      ST_MACRO_0,     ST_MACRO_1,     ST_MACRO_2,     TD(DANCE_2),                                    KC_RPRN,        KC_NO,          ST_MACRO_8,     KC_UP,          LSFT(KC_SCLN),  KC_NO,
    KC_LEFT_SHIFT,  KC_DELETE,      ST_MACRO_3,     ST_MACRO_4,     KC_HOME,        TD(DANCE_3),                                    KC_RCBR,        TD(DANCE_6),    KC_LEFT,        KC_DOWN,        KC_RIGHT,       TD(DANCE_7),
    KC_LEFT_CTRL,   KC_LEFT_ALT,    ST_MACRO_5,     ST_MACRO_6,     ST_MACRO_7,     TD(DANCE_4),                                    TD(DANCE_8),    ST_MACRO_9,     ST_MACRO_10,    ST_MACRO_11,    ST_MACRO_12,    KC_ENTER,
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [2] = LAYOUT_voyager(
    RGB_TOG,        TOGGLE_LAYER_COLOR,RGB_MODE_FORWARD,HSV_168_255_255,RGB_VAD,        RGB_VAI,                                        KC_MS_JIGGLER_TOGGLE,KC_NO,          KC_KP_SLASH,    KC_KP_ASTERISK, KC_KP_MINUS,    KC_KP_PLUS,
    QK_LLCK,        LALT(LCTL(LSFT(KC_0))),KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_AUDIO_MUTE,  LALT(LCTL(LSFT(KC_1))),                                KC_NO,          KC_NO,          KC_KP_7,        KC_KP_8,        KC_KP_9,        KC_KP_COMMA,
    KC_LEFT_SHIFT,  KC_MEDIA_PREV_TRACK,KC_MEDIA_NEXT_TRACK,KC_MEDIA_STOP,  KC_MEDIA_PLAY_PAUSE,LALT(LCTL(LSFT(KC_2))),                                KC_PSCR,        KC_NO,          KC_KP_4,        KC_KP_5,        KC_KP_6,        KC_EQUAL,
    KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_KP_0,        KC_KP_1,        KC_KP_2,        KC_KP_3,        KC_ENTER,
                                                    KC_DOT,         KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [3] = LAYOUT_voyager(
    MT(MOD_LGUI, KC_ESCAPE),KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          NAVIGATOR_DEC_CPI,NAVIGATOR_INC_CPI,
    QK_LLCK,        KC_NO,          KC_NO,          KC_MS_WH_UP,    KC_NO,          TOGGLE_SCROLL,                                  KC_NO,          KC_PGDN,        KC_MS_WH_UP,    KC_PAGE_UP,     KC_NO,          KC_NO,
    KC_LEFT_SHIFT,  KC_NO,          KC_MS_BTN2,     DRAG_SCROLL,    KC_MS_BTN1,     KC_MS_DBL_CLICK,                                KC_NO,          KC_MS_BTN1,     KC_MS_WH_DOWN,  KC_MS_BTN2,     KC_NO,          KC_NO,
    KC_LEFT_CTRL,   KC_LEFT_ALT,    KC_NO,          KC_MS_WH_DOWN,  KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
};

static uint8_t leader_step = 0;
static uint16_t first_leader_key = 0;
static uint16_t second_leader_key = 0;
static uint8_t last_rgb_mode;

extern rgb_config_t rgb_matrix_config;

RGB hsv_to_rgb_with_value(HSV hsv) {
  RGB rgb = hsv_to_rgb( hsv );
  float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
  return (RGB){ f * rgb.r, f * rgb.g, f * rgb.b };
}

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
}

const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [1] = { {91,238,230}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {20,249,221}, {41,170,230}, {196,138,221}, {196,138,221}, {196,138,221}, {41,170,230}, {91,238,230}, {41,170,230}, {196,138,221}, {196,138,221}, {142,255,255}, {41,170,230}, {91,238,230}, {91,238,230}, {196,138,221}, {196,138,221}, {196,138,221}, {41,170,230}, {0,0,0}, {0,0,0}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {41,170,230}, {0,0,0}, {196,138,221}, {139,225,240}, {41,170,230}, {0,0,0}, {41,170,230}, {142,255,255}, {139,225,240}, {139,225,240}, {139,225,240}, {41,170,230}, {41,170,230}, {196,138,221}, {196,138,221}, {196,138,221}, {196,138,221}, {91,238,230}, {0,0,0}, {0,0,0} },

    [2] = { {20,249,221}, {20,249,221}, {20,249,221}, {168,255,255}, {20,249,221}, {20,249,221}, {20,249,221}, {0,0,255}, {41,255,255}, {41,255,255}, {251,255,147}, {0,0,255}, {91,238,230}, {147,253,255}, {147,253,255}, {251,255,147}, {91,247,96}, {0,0,255}, {91,238,230}, {91,238,230}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {41,170,230}, {0,0,0}, {20,249,221}, {0,0,0}, {41,170,230}, {41,170,230}, {41,170,230}, {41,170,230}, {0,0,0}, {0,0,0}, {251,242,255}, {251,242,255}, {251,242,255}, {41,170,230}, {41,170,230}, {0,0,0}, {251,242,255}, {251,242,255}, {251,242,255}, {41,170,230}, {0,0,0}, {251,242,255}, {251,242,255}, {251,242,255}, {251,242,255}, {91,238,230}, {0,0,0}, {0,0,0} },

    [3] = { {91,238,230}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {20,249,221}, {0,0,0}, {0,0,0}, {142,255,255}, {0,0,0}, {20,249,221}, {91,238,230}, {0,0,0}, {142,255,255}, {91,238,230}, {142,255,255}, {142,255,255}, {91,238,230}, {91,238,230}, {0,0,0}, {142,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {20,249,221}, {20,249,221}, {0,0,0}, {142,255,255}, {142,255,255}, {142,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {142,255,255}, {142,255,255}, {142,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

};

void set_layer_color(int layer) {
  for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
    HSV hsv = {
      .h = pgm_read_byte(&ledmap[layer][i][0]),
      .s = pgm_read_byte(&ledmap[layer][i][1]),
      .v = pgm_read_byte(&ledmap[layer][i][2]),
    };
    if (!hsv.h && !hsv.s && !hsv.v) {
        rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
        RGB rgb = hsv_to_rgb_with_value(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
  }
}

bool rgb_matrix_indicators_user(void) {
  if (rawhid_state.rgb_control) {
      return false;
  }
  if (!keyboard_config.disable_layer_led) {
    switch (biton32(layer_state)) {
      case 1:
        set_layer_color(1);
        break;
      case 2:
        set_layer_color(2);
        break;
      case 3:
        set_layer_color(3);
        break;
     default:
        if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
          rgb_matrix_set_color_all(0, 0, 0);
        }
    }
  } else {
    if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
      rgb_matrix_set_color_all(0, 0, 0);
    }
  }

  return true;
}

extern bool set_scrolling;
extern bool navigator_turbo;
extern bool navigator_aim;
void pointing_device_init_user(void) {
  set_auto_mouse_enable(true);
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
  // Treat all keys as mouse keys when in the automouse layer so that any key set resets the timeout without leaving the layer.
  if (!layer_state_is(AUTO_MOUSE_TARGET_LAYER)){
    // When depressing a mouse key with a LT key at the same time, the mouse key tracker is not decremented.
    // This is a workaround to fix that
    if (IS_MOUSE_KEYCODE(keycode) && !record->event.pressed) {
      return true;
    }
    return false;
  }
  else {
    return false;
  }
}


typedef struct {
    bool is_press_action;
    uint8_t step;
} tap;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP,
    MORE_TAPS
};

static tap dance_state[9];

uint8_t dance_step(tap_dance_state_t *state);

uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        else return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return DOUBLE_SINGLE_TAP;
        else if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
    }
    return MORE_TAPS;
}


void on_dance_0(tap_dance_state_t *state, void *user_data);
void dance_0_finished(tap_dance_state_t *state, void *user_data);
void dance_0_reset(tap_dance_state_t *state, void *user_data);

void on_dance_0(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
        tap_code16(KC_BSPC);
    }
    if(state->count > 3) {
        tap_code16(KC_BSPC);
    }
}

void dance_0_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[0].step = dance_step(state);
    switch (dance_state[0].step) {
        case SINGLE_TAP: register_code16(KC_BSPC); break;
        case SINGLE_HOLD: layer_on(2); break;
        case DOUBLE_TAP: register_code16(KC_BSPC); register_code16(KC_BSPC); break;
        case DOUBLE_HOLD: register_code16(KC_BSPC); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_BSPC); register_code16(KC_BSPC);
    }
}

void dance_0_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[0].step) {
        case SINGLE_TAP: unregister_code16(KC_BSPC); break;
        case SINGLE_HOLD:
          if(!is_layer_locked(2)) {
            layer_off(2);
          }
        break;
        case DOUBLE_TAP: unregister_code16(KC_BSPC); break;
        case DOUBLE_HOLD: unregister_code16(KC_BSPC); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_BSPC); break;
    }
    dance_state[0].step = 0;
}
void on_dance_1(tap_dance_state_t *state, void *user_data);
void dance_1_finished(tap_dance_state_t *state, void *user_data);
void dance_1_reset(tap_dance_state_t *state, void *user_data);

void on_dance_1(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_F5);
        tap_code16(KC_F5);
        tap_code16(KC_F5);
    }
    if(state->count > 3) {
        tap_code16(KC_F5);
    }
}

void dance_1_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[1].step = dance_step(state);
    switch (dance_state[1].step) {
        case SINGLE_TAP: register_code16(KC_F5); break;
        case DOUBLE_TAP: register_code16(LCTL(LSFT(KC_F5))); break;
        case DOUBLE_HOLD: register_code16(LSFT(KC_F5)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_F5); register_code16(KC_F5);
    }
}

void dance_1_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[1].step) {
        case SINGLE_TAP: unregister_code16(KC_F5); break;
        case DOUBLE_TAP: unregister_code16(LCTL(LSFT(KC_F5))); break;
        case DOUBLE_HOLD: unregister_code16(LSFT(KC_F5)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_F5); break;
    }
    dance_state[1].step = 0;
}
void on_dance_2(tap_dance_state_t *state, void *user_data);
void dance_2_finished(tap_dance_state_t *state, void *user_data);
void dance_2_reset(tap_dance_state_t *state, void *user_data);

void on_dance_2(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
    }
    if(state->count > 3) {
        tap_code16(KC_LPRN);
    }
}

void dance_2_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[2].step = dance_step(state);
    switch (dance_state[2].step) {
        case SINGLE_TAP: register_code16(KC_LPRN); break;
        case DOUBLE_TAP: register_code16(KC_AT); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_LPRN); register_code16(KC_LPRN);
    }
}

void dance_2_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[2].step) {
        case SINGLE_TAP: unregister_code16(KC_LPRN); break;
        case DOUBLE_TAP: unregister_code16(KC_AT); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_LPRN); break;
    }
    dance_state[2].step = 0;
}
void on_dance_3(tap_dance_state_t *state, void *user_data);
void dance_3_finished(tap_dance_state_t *state, void *user_data);
void dance_3_reset(tap_dance_state_t *state, void *user_data);

void on_dance_3(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_LCBR);
        tap_code16(KC_LCBR);
        tap_code16(KC_LCBR);
    }
    if(state->count > 3) {
        tap_code16(KC_LCBR);
    }
}

void dance_3_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[3].step = dance_step(state);
    switch (dance_state[3].step) {
        case SINGLE_TAP: register_code16(KC_LCBR); break;
        case DOUBLE_TAP: register_code16(KC_AT); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_LCBR); register_code16(KC_LCBR);
    }
}

void dance_3_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[3].step) {
        case SINGLE_TAP: unregister_code16(KC_LCBR); break;
        case DOUBLE_TAP: unregister_code16(KC_AT); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_LCBR); break;
    }
    dance_state[3].step = 0;
}
void on_dance_4(tap_dance_state_t *state, void *user_data);
void dance_4_finished(tap_dance_state_t *state, void *user_data);
void dance_4_reset(tap_dance_state_t *state, void *user_data);

void on_dance_4(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_LBRC);
        tap_code16(KC_LBRC);
        tap_code16(KC_LBRC);
    }
    if(state->count > 3) {
        tap_code16(KC_LBRC);
    }
}

void dance_4_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[4].step = dance_step(state);
    switch (dance_state[4].step) {
        case SINGLE_TAP: register_code16(KC_LBRC); break;
        case DOUBLE_TAP: register_code16(KC_LABK); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_LBRC); register_code16(KC_LBRC);
    }
}

void dance_4_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[4].step) {
        case SINGLE_TAP: unregister_code16(KC_LBRC); break;
        case DOUBLE_TAP: unregister_code16(KC_LABK); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_LBRC); break;
    }
    dance_state[4].step = 0;
}
void on_dance_5(tap_dance_state_t *state, void *user_data);
void dance_5_finished(tap_dance_state_t *state, void *user_data);
void dance_5_reset(tap_dance_state_t *state, void *user_data);

void on_dance_5(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_F11);
        tap_code16(KC_F11);
        tap_code16(KC_F11);
    }
    if(state->count > 3) {
        tap_code16(KC_F11);
    }
}

void dance_5_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[5].step = dance_step(state);
    switch (dance_state[5].step) {
        case SINGLE_TAP: register_code16(KC_F11); break;
        case DOUBLE_TAP: register_code16(LSFT(KC_F11)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_F11); register_code16(KC_F11);
    }
}

void dance_5_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[5].step) {
        case SINGLE_TAP: unregister_code16(KC_F11); break;
        case DOUBLE_TAP: unregister_code16(LSFT(KC_F11)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_F11); break;
    }
    dance_state[5].step = 0;
}
void on_dance_6(tap_dance_state_t *state, void *user_data);
void dance_6_finished(tap_dance_state_t *state, void *user_data);
void dance_6_reset(tap_dance_state_t *state, void *user_data);

void on_dance_6(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_END);
        tap_code16(KC_END);
        tap_code16(KC_END);
    }
    if(state->count > 3) {
        tap_code16(KC_END);
    }
}

void dance_6_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[6].step = dance_step(state);
    switch (dance_state[6].step) {
        case SINGLE_TAP: register_code16(KC_END); break;
        case DOUBLE_TAP: register_code16(KC_SCLN); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_END); register_code16(KC_END);
    }
}

void dance_6_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[6].step) {
        case SINGLE_TAP: unregister_code16(KC_END); break;
        case DOUBLE_TAP: unregister_code16(KC_SCLN); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_END); break;
    }
    dance_state[6].step = 0;
}
void on_dance_7(tap_dance_state_t *state, void *user_data);
void dance_7_finished(tap_dance_state_t *state, void *user_data);
void dance_7_reset(tap_dance_state_t *state, void *user_data);

void on_dance_7(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_DQUO);
        tap_code16(KC_DQUO);
        tap_code16(KC_DQUO);
    }
    if(state->count > 3) {
        tap_code16(KC_DQUO);
    }
}

void dance_7_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[7].step = dance_step(state);
    switch (dance_state[7].step) {
        case SINGLE_TAP: register_code16(KC_DQUO); break;
        case DOUBLE_TAP: register_code16(KC_GRAVE); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_DQUO); register_code16(KC_DQUO);
    }
}

void dance_7_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[7].step) {
        case SINGLE_TAP: unregister_code16(KC_DQUO); break;
        case DOUBLE_TAP: unregister_code16(KC_GRAVE); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_DQUO); break;
    }
    dance_state[7].step = 0;
}
void on_dance_8(tap_dance_state_t *state, void *user_data);
void dance_8_finished(tap_dance_state_t *state, void *user_data);
void dance_8_reset(tap_dance_state_t *state, void *user_data);

void on_dance_8(tap_dance_state_t *state, void *user_data) {
    if(state->count == 3) {
        tap_code16(KC_RBRC);
        tap_code16(KC_RBRC);
        tap_code16(KC_RBRC);
    }
    if(state->count > 3) {
        tap_code16(KC_RBRC);
    }
}

void dance_8_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[8].step = dance_step(state);
    switch (dance_state[8].step) {
        case SINGLE_TAP: register_code16(KC_RBRC); break;
        case DOUBLE_TAP: register_code16(KC_RABK); break;
        case DOUBLE_SINGLE_TAP: tap_code16(KC_RBRC); register_code16(KC_RBRC);
    }
}

void dance_8_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[8].step) {
        case SINGLE_TAP: unregister_code16(KC_RBRC); break;
        case DOUBLE_TAP: unregister_code16(KC_RABK); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(KC_RBRC); break;
    }
    dance_state[8].step = 0;
}

tap_dance_action_t tap_dance_actions[] = {
        [DANCE_0] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_0, dance_0_finished, dance_0_reset),
        [DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_1, dance_1_finished, dance_1_reset),
        [DANCE_2] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_2, dance_2_finished, dance_2_reset),
        [DANCE_3] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_3, dance_3_finished, dance_3_reset),
        [DANCE_4] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_4, dance_4_finished, dance_4_reset),
        [DANCE_5] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_5, dance_5_finished, dance_5_reset),
        [DANCE_6] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_6, dance_6_finished, dance_6_reset),
        [DANCE_7] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_7, dance_7_finished, dance_7_reset),
        [DANCE_8] = ACTION_TAP_DANCE_FN_ADVANCED(on_dance_8, dance_8_finished, dance_8_reset),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (leader_sequence_active() && record->event.pressed) {
        if (leader_step == 0) {
            first_leader_key = keycode;
            leader_step = 1;
        } else if (leader_step == 1) {
            leader_step = 2;
        }else if (leader_step == 2) {
                leader_step = 3;
        }
    }

  switch (keycode) {
  case QK_MODS ... QK_MODS_MAX:
    // Mouse keys with modifiers work inconsistently across operating systems, this makes sure that modifiers are always
    // applied to the mouse key that was pressed.
    if (IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
    if (record->event.pressed) {
        add_mods(QK_MODS_GET_MODS(keycode));
        send_keyboard_report();
        wait_ms(2);
        register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
        return false;
      } else {
        wait_ms(2);
        del_mods(QK_MODS_GET_MODS(keycode));
      }
    }
    break;
    case ST_MACRO_0:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_MINUS)SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_1:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_EQUAL))SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_2:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_9))SS_DELAY(100)  SS_LSFT(SS_TAP(X_1))SS_DELAY(100)  SS_LSFT(SS_TAP(X_9))SS_DELAY(100)  SS_LSFT(SS_TAP(X_0))SS_DELAY(100)  SS_LSFT(SS_TAP(X_0))SS_DELAY(100)  SS_TAP(X_LEFT)SS_DELAY(100)  SS_TAP(X_LEFT));
    }
    break;
    case ST_MACRO_3:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_COMMA))SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_4:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_DOT))SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_5:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_SCLN))SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_6:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_1))SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_7:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_8:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_7))SS_DELAY(100)  SS_LSFT(SS_TAP(X_6))SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_9:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_COMMA))SS_DELAY(100)  SS_TAP(X_MINUS)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;
    case ST_MACRO_10:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_MINUS)SS_DELAY(100)  SS_TAP(X_MINUS));
    }
    break;
    case ST_MACRO_11:
    if (record->event.pressed) {
      SEND_STRING(SS_LSFT(SS_TAP(X_EQUAL))SS_DELAY(100)  SS_LSFT(SS_TAP(X_EQUAL)));
    }
    break;
    case ST_MACRO_12:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_EQUAL)SS_DELAY(100)  SS_TAP(X_SPACE));
    }
    break;

    case DRAG_SCROLL:
      if (record->event.pressed) {
        set_scrolling = true;
      } else {
        set_scrolling = false;
      }
      return false;
    case TOGGLE_SCROLL:
      if (record->event.pressed) {
        set_scrolling = !set_scrolling;
      }
      return false;
    break;
  case NAVIGATOR_TURBO:
    if (record->event.pressed) {
      navigator_turbo = true;
    } else {
      navigator_turbo = false;
    }
    return false;
  case NAVIGATOR_AIM:
    if (record->event.pressed) {
      navigator_aim = true;
    } else {
      navigator_aim = false;
    }
    return false;
  case NAVIGATOR_INC_CPI:
    if (record->event.pressed) {
        pointing_device_set_cpi(1);
    }
    return false;
  case NAVIGATOR_DEC_CPI:
    if (record->event.pressed) {
        pointing_device_set_cpi(0);
    }
    return false;
    case RGB_SLD:
      if (record->event.pressed) {
        rgblight_mode(1);
      }
      return false;
    case HSV_168_255_255:
      if (record->event.pressed) {
        rgblight_mode(1);
        rgblight_sethsv(168,255,255);
      }
      return false;
  }
  return true;
}

void leader_start_user(void) {
    // Save the current mode
    last_rgb_mode = rgb_matrix_get_mode();

    leader_step = 0;
    first_leader_key = 0;

    // Change to listen mode
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_set_color_all(10, 10, 10); // Low white/dim background
}

void leader_end_user(void) {
    bool did_match = false;
    leader_step = 0; // reset leader tracker

    // three key sequences
    if (leader_sequence_three_keys(KC_G, KC_C, KC_M)) {
        SEND_STRING("git commit -m \"\"" SS_TAP(X_LEFT));
        did_match = true;
    }
    else if (leader_sequence_three_keys(KC_G, KC_C, KC_C)) {
        SEND_STRING("git commit\n");
        did_match = true;
    }
    else if (leader_sequence_three_keys(KC_G, KC_P, KC_D)) {
        SEND_STRING("git pull\n");
        did_match = true;
    }
    else if (leader_sequence_three_keys(KC_G, KC_P, KC_U)) {
        SEND_STRING("git push\n");
        did_match = true;
    }

    // two key sequences
    else if (leader_sequence_two_keys(KC_Q, KC_Q)) {
        // Sends Ctrl+C to kill any hung process, then clears terminal
        SEND_STRING(SS_LCTL("c") "clear\n");
        did_match = true;
    }
    else if (leader_sequence_two_keys(KC_G, KC_I)) {
        SEND_STRING("git init\n");
        did_match = true;
    }
    else if (leader_sequence_two_keys(KC_G, KC_N)) {
        // Prepare for a new branch name
        SEND_STRING("git checkout -b ");
        did_match = true;
    }
    else if (leader_sequence_two_keys(KC_G, KC_B)) {
        // Prepare to switch branches
        SEND_STRING("git checkout ");
        did_match = true;
    }
    else if (leader_sequence_two_keys(KC_G, KC_S)) {
        SEND_STRING("git status\n");
        did_match = true;
    }
    else if (leader_sequence_two_keys(KC_G, KC_A)) {
        SEND_STRING("git add .\n");
        did_match = true;
    }

    // --- Visual Feedback Logic ---
    if (did_match) {
        // Flash Green for success
        rgb_matrix_set_color_all(15, 230, 44);
        rgb_matrix_update_pwm_buffers();

        wait_ms(150);
    } else {
        // Flash Red for failure
        rgb_matrix_set_color_all(255, 0, 0);
        rgb_matrix_update_pwm_buffers();

        wait_ms(150);
    }

    rgb_matrix_mode(last_rgb_mode);
}

void matrix_scan_user(void) {
    if (leader_sequence_active()) {
        // Calculate pulse: oscillates roughly every 1.5 seconds
        // scaled to a range of 0.4 to 1.0 so keys don't go fully dark
        float pulse = 0.7 + 0.3 * sin(g_rgb_timer * 0.005);

        // Base background remains static and dim
        rgb_matrix_set_color_all(5, 5, 5);

        switch (leader_step) {
            case 0:
                // Pulsing Blue for First Level
                rgb_matrix_set_color(17, 0, 23 * pulse, 255 * pulse); // G
                rgb_matrix_set_color(7, 0, 23 * pulse, 255 * pulse);  // Q
                break;

            case 1:
                if (first_leader_key == KC_G) {
                    rgb_matrix_set_color(17, 0, 23, 255); // Solid Blue (Selected)

                    // Pulsing Green for Git options
                    uint8_t g = 230 * pulse;
                    uint8_t b = 44 * pulse;
                    rgb_matrix_set_color(15, 15, g, b); // s
                    rgb_matrix_set_color(13, 15, g, b); // a
                    rgb_matrix_set_color(11, 15, g, b); // b
                    rgb_matrix_set_color(39, 15, g, b); // n
                    rgb_matrix_set_color(41, 15, g, b); // i
                    rgb_matrix_set_color(10, 15, g, b); // p
                    rgb_matrix_set_color(21, 15, g, b); // c
                }
                break;

            case 2:
                if (first_leader_key == KC_G) {
                    rgb_matrix_set_color(17, 0, 23, 255); // Solid G

                    uint8_t g = 230 * pulse;
                    uint8_t b = 44 * pulse;

                    if (second_leader_key == KC_P) {
                        rgb_matrix_set_color(10, 0, 23, 255); // Solid P
                        rgb_matrix_set_color(35, 15, g, b);   // Pulse U
                        rgb_matrix_set_color(22, 15, g, b);   // Pulse D
                    }
                    else if (second_leader_key == KC_C) {
                        rgb_matrix_set_color(21, 0, 23, 255); // Solid C
                        rgb_matrix_set_color(38, 15, g, b);   // Pulse M
                        rgb_matrix_set_color(21, 15, g, b);   // Pulse C (for CC)
                    }
                }
                break;
        }
    }
}
