// leader.c
#include "leader.h"
#include "quantum.h"

// Global state
static leader_state_t leader_state = {
    .step = 0,
    .first_key = 0,
    .second_key = 0,
    .third_key = 0,
    .last_rgb_mode = 0
};

// Lookup table for leader sequences
static const leader_sequence_t leader_sequences[] = {
    // Three-key sequences
    {KC_G, KC_C, KC_F, ACT_GIT_COMMIT_FULL,     "git commit\n"},
    {KC_G, KC_C, KC_S, ACT_GIT_COMMIT_SHORT,    "git commit -m \"\""},
    {KC_G, KC_P, KC_D, ACT_GIT_PULL,            "git pull\n"},
	{KC_G, KC_P, KC_R, ACT_GIT_PULL_REBASE,     "git pull --rebase\n"},
    {KC_G, KC_P, KC_U, ACT_GIT_PUSH,            "git push\n"},
    {KC_G, KC_S, KC_T, ACT_GIT_STATUS,          "git status -sb\n"},
    {KC_G, KC_S, KC_I, ACT_GIT_STASH_PUSH,      "git stash push -m \"wip\"\n"},
    {KC_G, KC_S, KC_O, ACT_GIT_STASH_POP,       "git stash pop\n"},
    {KC_G, KC_B, KC_L, ACT_GIT_BRANCH,          "git branch\n"},
    {KC_G, KC_B, KC_N, ACT_GIT_BRANCH_NEW,      "git checkout -b "},
    {KC_G, KC_B, KC_C, ACT_GIT_BRANCH_CHECKOUT, "git checkout "},
	{KC_G, KC_D, KC_U, ACT_GIT_DIFF,            "git diff --color\n"},
	{KC_G, KC_D, KC_S, ACT_GIT_DIFF_STAGED,     "git diff --staged\n"},
	{KC_G, KC_R, KC_R, ACT_GIT_REBASE,          "git rebase\n"},
	{KC_G, KC_R, KC_A, ACT_GIT_REBASE_ABORT,    "git rebase --abort\n"},
	{KC_G, KC_R, KC_C, ACT_GIT_REBASE_CONTINUE, "git rebase --continue\n"},
	{KC_G, KC_R, KC_S, ACT_GIT_RESTORE,         "git restore --staged\n"},
	{KC_G, KC_R, KC_H, ACT_GIT_RESET,           "git reset --hard\n"},

    {KC_C, KC_P, KC_H, ACT_CODE_HASHTABLE,      "[hashtable] $  = @{}"},
    {KC_C, KC_P, KC_U, ACT_CODE_CUSTOM_OBJ,     "[PSCustomObject]@{}"},

    // Two-key sequences
    {KC_Q, KC_Q, 0,    ACT_QUICK_KILL_CLEAR,    NULL},  // Special handling
    {KC_G, KC_I, 0,    ACT_GIT_INIT,            "git init\n"},
    {KC_G, KC_A, 0,    ACT_GIT_ADD,             "git add .\n"},
    {KC_G, KC_L, 0,    ACT_GIT_LOG,             "git log --graph --oneline --decorate --all\n"},
    {KC_G, KC_F, 0,    ACT_GIT_FETCH,           "git fetch\n"},
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
    leader_state.third_key = 0;

    // Change to listen mode (dim white background)
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_set_color_all(10, 10, 10);
}

void leader_end_logic(void) {
    bool did_match = false;

    // Check all sequences in the lookup table using our custom tracked state
    for (uint8_t i = 0; i < NUM_SEQUENCES; i++) {
        const leader_sequence_t* seq = &leader_sequences[i];

        // Check if this sequence matches
        if (seq->key3 != 0) {
            // Three-key sequence: compare with our tracked keys
            if (leader_state.step >= 3 &&
                leader_state.first_key == seq->key1 &&
                leader_state.second_key == seq->key2 &&
                leader_state.third_key == seq->key3) {
                // Special handling for sequences that need cursor positioning
                if (seq->action == ACT_GIT_COMMIT_SHORT) {
                    SEND_STRING(seq->output);
                    SEND_STRING(SS_TAP(X_LEFT));
                } else if (seq->action == ACT_CODE_CUSTOM_OBJ) {
					SEND_STRING(seq->output);
					SEND_STRING(SS_TAP(X_LEFT));
				} else {
                    SEND_STRING(seq->output);
                }
                did_match = true;
                break;
            }
        } else if (seq->key2 != 0) {
            // Two-key sequence: compare with our tracked keys (exact match: step must be exactly 2)
            if (leader_state.step == 2 &&
                leader_state.first_key == seq->key1 &&
                leader_state.second_key == seq->key2) {
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
            // Blue for first level category selection
            rgb_matrix_set_color(7, 0, 23, 255);  // Q quick actions
            rgb_matrix_set_color(17, 0, 23, 255); // G git actions
            rgb_matrix_set_color(21, 0, 23, 255); // C code actions
            break;

        case 1:
            if (leader_state.first_key == KC_Q) { // Quick actions
                rgb_matrix_set_color(7, 255, 0, 0);  // Q ctrl + c && clear
            }
            else if (leader_state.first_key == KC_G) { // Git actions
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                // Green for second level git options
                rgb_matrix_set_color(15, 15, 230, 44); // S status / stash
                rgb_matrix_set_color(11, 15, 230, 44); // B branch
                rgb_matrix_set_color(10, 15, 230, 44); // P pull
                rgb_matrix_set_color(21, 15, 230, 44); // C commit
                rgb_matrix_set_color(13, 15, 230, 44); // A add
                rgb_matrix_set_color(41, 15, 230, 44); // I init
                rgb_matrix_set_color(33, 15, 230, 44); // L log
				rgb_matrix_set_color(14, 15, 230, 44); // R rebase / restore / reset
                rgb_matrix_set_color(9, 15, 230, 44);  // F fetch
                rgb_matrix_set_color(22, 15, 230, 44); // D diff
            }
            else if (leader_state.first_key == KC_C) { // Code actions
                rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                rgb_matrix_set_color(10, 15, 230, 44); // P powershell
            	rgb_matrix_set_color(17, 15, 230, 44); // G golang
            }
            break;

        case 2:
            if (leader_state.first_key == KC_G) {
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                if (leader_state.second_key == KC_P) { // Pull
                    rgb_matrix_set_color(10, 0, 23, 255);  // P Blue (Selected)

                    rgb_matrix_set_color(34, 15, 230, 44); // U push
                    rgb_matrix_set_color(22, 15, 230, 44); // D pull
                    rgb_matrix_set_color(14, 15, 230, 44); // R pull rebase
                }
                else if (leader_state.second_key == KC_C) { // Commit
                    rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                    rgb_matrix_set_color(9, 15, 230, 44);  // F full
                    rgb_matrix_set_color(15, 15, 230, 44); // S short
                }
                else if (leader_state.second_key == KC_B) { // Branch
                    rgb_matrix_set_color(11, 0, 23, 255); // B Blue (Selected)

                    rgb_matrix_set_color(33, 15, 230, 44); // L list
                    rgb_matrix_set_color(39, 15, 230, 44); // N new
                    rgb_matrix_set_color(21, 15, 230, 44); // C checkout
                }
                else if (leader_state.second_key == KC_S) { // Status / Stash
                    rgb_matrix_set_color(15, 0, 23, 255); // S Blue (Selected)

                    rgb_matrix_set_color(16, 15, 230, 44); // T status
                    rgb_matrix_set_color(41, 15, 230, 44); // I stash push
                    rgb_matrix_set_color(42, 15, 230, 44); // O stash pop
                }
                else if (leader_state.second_key == KC_D) { // Diff
                    rgb_matrix_set_color(22, 0, 23, 255); // D Blue (Selected)

                    rgb_matrix_set_color(15, 15, 230, 44); // S staged
					rgb_matrix_set_color(34, 15, 230, 44); // U unstaged
                }
                else if (leader_state.second_key == KC_R) { // Rebase / Restore / Reset
                    rgb_matrix_set_color(14, 0, 23, 255); // R Blue (Selected)

                    rgb_matrix_set_color(15, 15, 230, 44); // S restore staged
					rgb_matrix_set_color(14, 15, 230, 44); // R rebase
         	        rgb_matrix_set_color(21, 15, 230, 44); // C continue rebase
         	        rgb_matrix_set_color(13, 15, 230, 44); // A abort rebase
					rgb_matrix_set_color(45, 15, 230, 44); // H reset hard
                }
            }
            if (leader_state.first_key == KC_C) { // Code actions
                rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                if (leader_state.second_key == KC_P) {
                    rgb_matrix_set_color(10, 0, 23, 255);  // P Blue (Selected)

                    rgb_matrix_set_color(45, 15, 230, 44); // H
                    rgb_matrix_set_color(34, 15, 230, 44); // U
                }
            }
            break;
    }
}
