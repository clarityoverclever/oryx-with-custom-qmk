// leader.c
#include "leader.h"
#include "quantum.h"

// Global state
static leader_state_t leader_state = {
    .step = 0,
    .first_key = 0,
    .second_key = 0,
    .last_rgb_mode = 0
};

// Lookup table for leader sequences
static const leader_sequence_t leader_sequences[] = {
    // Three-key sequences
    {KC_G, KC_C, KC_F, ACT_GIT_COMMIT_FULL,     "git commit\n"},
    {KC_G, KC_C, KC_S, ACT_GIT_COMMIT_SHORT,    "git commit -m \"\""},
    {KC_G, KC_P, KC_D, ACT_GIT_PULL,            "git pull\n"},
    {KC_G, KC_P, KC_U, ACT_GIT_PUSH,            "git push\n"},
    {KC_G, KC_S, KC_T, ACT_GIT_STATUS,          "git status -sb\n"},
    {KC_G, KC_S, KC_I, ACT_GIT_STASH_PUSH,      "git stash push -m \"wip\"\n"},
    {KC_G, KC_S, KC_O, ACT_GIT_STASH_POP,       "git stash pop\n"},
    {KC_G, KC_B, KC_R, ACT_GIT_BRANCH,          "git branch\n"},
    {KC_G, KC_B, KC_N, ACT_GIT_BRANCH_NEW,      "git checkout -b "},
    {KC_G, KC_B, KC_C, ACT_GIT_BRANCH_CHECKOUT, "git checkout "},

    // Two-key sequences
    {KC_Q, KC_Q, 0,    ACT_QUICK_KILL_CLEAR,    NULL},  // Special handling
    {KC_G, KC_I, 0,    ACT_GIT_INIT,            "git init\n"},
    {KC_G, KC_A, 0,    ACT_GIT_ADD,             "git add .\n"},
    {KC_G, KC_L, 0,    ACT_GIT_LOG,             "git log --graph --oneline --decorate --all\n"},
};

#define NUM_SEQUENCES (sizeof(leader_sequences) / sizeof(leader_sequences[0]))

// Accessor for state
leader_state_t* get_leader_state(void) {
    return &leader_state;
}

void leader_start_logic(void) {
    // Save the current RGB mode
    leader_state.last_rgb_mode = rgb_matrix_get_mode();

    // Reset state
    leader_state.step = 0;
    leader_state.first_key = 0;
    leader_state.second_key = 0;

    // Change to listen mode (dim white background)
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_set_color_all(10, 10, 10);
}

void leader_end_logic(void) {
    bool did_match = false;

    // Check all sequences in the lookup table
    for (uint8_t i = 0; i < NUM_SEQUENCES; i++) {
        const leader_sequence_t* seq = &leader_sequences[i];

        // Check if this sequence matches
        if (seq->key3 != 0) {
            // Three-key sequence
            if (leader_sequence_three_keys(seq->key1, seq->key2, seq->key3)) {
                // Special handling for sequences that need cursor positioning
                if (seq->action == ACT_GIT_COMMIT_SHORT) {
                    SEND_STRING(seq->output);
                    SEND_STRING(SS_TAP(X_LEFT));
                } else {
                    SEND_STRING(seq->output);
                }
                did_match = true;
                break;
            }
        } else if (seq->key2 != 0) {
            // Two-key sequence
            if (leader_sequence_two_keys(seq->key1, seq->key2)) {
                // Special handling for kill+clear
                if (seq->action == ACT_QUICK_KILL_CLEAR) {
                    SEND_STRING(SS_LCTL("c") "clear\n");
                } else {
                    SEND_STRING(seq->output);
                }
                did_match = true;
                break;
            }
        }
    }

    // Reset step counter
    leader_state.step = 0;

    // Visual feedback
    if (did_match) {
        // Flash green for success
        rgb_matrix_set_color_all(15, 230, 44);
        rgb_matrix_update_pwm_buffers();
        wait_ms(150);
    } else {
        // Flash red for failure
        rgb_matrix_set_color_all(255, 0, 0);
        rgb_matrix_update_pwm_buffers();
        wait_ms(150);
    }

    // Restore original RGB mode
    rgb_matrix_mode(leader_state.last_rgb_mode);
}

void leader_visual_logic(void) {
    if (!leader_sequence_active()) {
        return;
    }

    // Base background: static and dim
    rgb_matrix_set_color_all(5, 5, 5);

    switch (leader_state.step) {
        case 0:
            // Blue for first level options
            rgb_matrix_set_color(17, 0, 23, 255); // G
            rgb_matrix_set_color(7, 0, 23, 255);  // Q
            break;

        case 1:
            if (leader_state.first_key == KC_G) {
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                // Green for second level git options
                rgb_matrix_set_color(15, 15, 230, 44); // S
                rgb_matrix_set_color(11, 15, 230, 44); // B
                rgb_matrix_set_color(10, 15, 230, 44); // P
                rgb_matrix_set_color(21, 15, 230, 44); // C
                rgb_matrix_set_color(13, 15, 230, 44); // A
                rgb_matrix_set_color(41, 15, 230, 44); // I
                rgb_matrix_set_color(33, 15, 230, 44); // L
            }
            else if (leader_state.first_key == KC_Q) {
                rgb_matrix_set_color(7, 255, 0, 0);  // Q Red (Selected)
                rgb_matrix_set_color(7, 255, 0, 0);  // Q again for double-tap
            }
            break;

        case 2:
            if (leader_state.first_key == KC_G) {
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                if (leader_state.second_key == KC_P) {
                    rgb_matrix_set_color(10, 0, 23, 255);  // P Blue (Selected)

                    rgb_matrix_set_color(34, 15, 230, 44); // U
                    rgb_matrix_set_color(22, 15, 230, 44); // D
                }
                else if (leader_state.second_key == KC_C) {
                    rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                    rgb_matrix_set_color(9, 15, 230, 44);  // F
                    rgb_matrix_set_color(15, 15, 230, 44); // S
                }
                else if (leader_state.second_key == KC_B) {
                    rgb_matrix_set_color(11, 0, 23, 255); // B Blue (Selected)

                    rgb_matrix_set_color(14, 15, 230, 44); // R
                    rgb_matrix_set_color(39, 15, 230, 44); // N
                    rgb_matrix_set_color(21, 15, 230, 44); // C
                }
                else if (leader_state.second_key == KC_S) {
                    rgb_matrix_set_color(15, 0, 23, 255); // S Blue (Selected)

                    rgb_matrix_set_color(16, 15, 230, 44); // T
                    rgb_matrix_set_color(41, 15, 230, 44); // I
                    rgb_matrix_set_color(42, 15, 230, 44); // O
                }
            }
            break;
    }
}
