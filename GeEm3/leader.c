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
    {KC_G, KC_P, KC_D, ACT_GIT_PULL,            "git pull --rebase\n"},
    {KC_G, KC_P, KC_U, ACT_GIT_PUSH,            "git push\n"},
    {KC_G, KC_S, KC_T, ACT_GIT_STATUS,          "git status -sb\n"},
    {KC_G, KC_S, KC_I, ACT_GIT_STASH_PUSH,      "git stash push -m \"wip\"\n"},
    {KC_G, KC_S, KC_O, ACT_GIT_STASH_POP,       "git stash pop\n"},
    {KC_G, KC_B, KC_L, ACT_GIT_BRANCH,          "git branch\n"},
    {KC_G, KC_B, KC_N, ACT_GIT_BRANCH_NEW,      "git switch -c "},
    {KC_G, KC_B, KC_C, ACT_GIT_BRANCH_CHECKOUT, "git switch "},
	{KC_G, KC_D, KC_U, ACT_GIT_DIFF,            "git diff --color\n"},
	{KC_G, KC_D, KC_S, ACT_GIT_DIFF_STAGED,     "git diff --staged\n"},
	{KC_G, KC_R, KC_R, ACT_GIT_REBASE,          "git rebase\n"},
	{KC_G, KC_R, KC_A, ACT_GIT_REBASE_ABORT,    "git rebase --abort\n"},
	{KC_G, KC_R, KC_C, ACT_GIT_REBASE_CONTINUE, "git rebase --continue\n"},
	{KC_G, KC_R, KC_F, ACT_GIT_RESTORE,         "git restore "},
	{KC_G, KC_R, KC_H, ACT_GIT_RESET,           "git reset --hard\n"},
	{KC_G, KC_F, KC_C, ACT_GIT_FIND_CONFLICT,   "<<<<<<<"},

	{KC_C, KC_P, KC_S, ACT_CODE_PS_SET_PATH,    "$path = \"\""},
    {KC_C, KC_G, KC_E, ACT_CODE_GO_ERROR,       "if err != nil {\n}"},

    {KC_S, KC_W, KC_T, ACT_SYSTEM_WIN_TAKEOWN,            "takeown /F $path /R /D Y"},
    {KC_S, KC_W, KC_R, ACT_SYSTEM_WIN_ICACLS_RESET,       "icacls $path /reset /T\n"},
    {KC_S, KC_W, KC_A, ACT_SYSTEM_WIN_ICACLS_GRANT,       "icacls $path /grant Administrators:F /T\n"},
    {KC_S, KC_W, KC_O, ACT_SYSTEM_WIN_ICACLS_SET_SYSTEM,  "icacls $path /setowner SYSTEM /T\n"},
    {KC_S, KC_W, KC_I, ACT_SYSTEM_WIN_ICACLS_INH_REMOVE,  "icacls $path /inheritance:r\n"},
    {KC_S, KC_W, KC_D, ACT_SYSTEM_WIN_ICACLS_REMOVE_DENY, "icacls $path /remove:d *\n"},


    // Two-key sequences
    {KC_Q, KC_Q, 0,    ACT_QUICK_KILL_CLEAR,    NULL},  // Special handling
	{KC_Q, KC_S, 0,    ACT_QUICK_SUDO_RERUN,    "sudo !!\n"},
	{KC_Q, KC_E, 0,    ACT_QUICK_EMAIL,         "@.com"},
    {KC_G, KC_C, 0     ACT_GIT_COMMIT,          "git commit"},
    {KC_G, KC_A, 0,    ACT_GIT_ADD,             "git add .\n"},
    {KC_G, KC_L, 0,    ACT_GIT_LOG,             "git log --graph --oneline --decorate --all\n"},
    {KC_G, KC_F, 0,    ACT_GIT_FETCH,           "git fetch\n"},
	{KC_G, KC_U, 0,    ACT_GIT_UNSTAGE,         "git reset\n"},
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
                if (seq->action == ACT_CODE_PS_SET_PATH) {
                    SEND_STRING(seq->output);
                    tap_code(KC_LEFT);
				} else if (seq->action == ACT_CODE_GO_ERROR) {
					SEND_STRING(seq->output);
					//for (int i = 0; i < 13; i++) { tap_code(KC_LEFT); }
				} else if (seq->action == ACT_GIT_FIND_CONFLICT) {
    				// Open Search (Ctrl+F)
    				tap_code16(LCTL(KC_F));
  					wait_ms(150); // Small buffer for the search UI to pop up
   					SEND_STRING(seq->output);
					tap_code(KC_ENT);
					tap_code(KC_ESC);
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
                } else if (seq->action == ACT_QUICK_EMAIL) {
                    SEND_STRING(seq->output);
					for (int i = 0; i < 4; i++) { tap_code(KC_LEFT); }
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
			rgb_matrix_set_color(15, 0, 23, 255); // S system actions
            break;

        case 1:
            if (leader_state.first_key == KC_Q) { // Quick actions
                rgb_matrix_set_color(7, 255, 0, 0);  // Q ctrl + c && clear
				rgb_matrix_set_color(15, 15, 230, 44); // S sudo rerun
				rgb_matrix_set_color(40, 15, 230, 44); // E @.com

            }
            else if (leader_state.first_key == KC_G) { // Git actions
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                rgb_matrix_set_color(15, 15, 230, 44); // S status / stash
                rgb_matrix_set_color(11, 15, 230, 44); // B branch
                rgb_matrix_set_color(10, 15, 230, 44); // P pull
                rgb_matrix_set_color(21, 15, 230, 44); // C commit
                rgb_matrix_set_color(13, 15, 230, 44); // A add
                rgb_matrix_set_color(33, 15, 230, 44); // L log
				rgb_matrix_set_color(14, 15, 230, 44); // R rebase / restore / reset
                rgb_matrix_set_color(9, 15, 230, 44);  // F fetch / find conflict
                rgb_matrix_set_color(22, 15, 230, 44); // D diff
				rgb_matrix_set_color(34, 15, 230, 44); // U unstage
            }
            else if (leader_state.first_key == KC_C) { // Code actions
                rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                rgb_matrix_set_color(10, 15, 230, 44); // P powershell
            	rgb_matrix_set_color(17, 15, 230, 44); // G golang
            }
            else if (leader_state.first_key == KC_S) { // System actions
                rgb_matrix_set_color(15, 0, 23, 255); // S Blue (Selected)

                rgb_matrix_set_color(8, 15, 230, 44); // W Windows
				//rgb_matrix_set_color(33, 15, 230, 44); // L Linux
            }
            break;

        case 2:
            if (leader_state.first_key == KC_G) {
                rgb_matrix_set_color(17, 0, 23, 255); // G Blue (Selected)

                if (leader_state.second_key == KC_P) { // Pull
                    rgb_matrix_set_color(10, 0, 23, 255);  // P Blue (Selected)

                    rgb_matrix_set_color(34, 15, 230, 44); // U push
                    rgb_matrix_set_color(22, 15, 230, 44); // D pull rebase
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

                    rgb_matrix_set_color(9, 255, 0, 0); // F restore
					rgb_matrix_set_color(14, 15, 230, 44); // R rebase
         	        rgb_matrix_set_color(21, 15, 230, 44); // C continue rebase
         	        rgb_matrix_set_color(13, 15, 230, 44); // A abort rebase
					rgb_matrix_set_color(45, 255, 0, 0); // H reset hard
                }
                else if (leader_state.second_key == KC_F) { // Find
                    rgb_matrix_set_color(9, 0, 23, 255); // F Blue (Selected)

                    rgb_matrix_set_color(21, 15, 230, 44); // C conflict
                }
            }
            if (leader_state.first_key == KC_C) { // Code actions
                rgb_matrix_set_color(21, 0, 23, 255); // C Blue (Selected)

                if (leader_state.second_key == KC_P) { // Powershell
                    rgb_matrix_set_color(10, 0, 23, 255);  // P Blue (Selected)

					rgb_matrix_set_color(15, 15, 230, 44); // S set $path
                }
                else if (leader_state.second_key == KC_G) { // Golang
                    rgb_matrix_set_color(17, 0, 23, 255);  // G Blue (Selected)

                    rgb_matrix_set_color(40, 15, 230, 44); // E error
                }
            }
            if (leader_state.first_key == KC_S) { // System actions
                rgb_matrix_set_color(15, 0, 23, 255); // S Blue (Selected)

                if (leader_state.second_key == KC_W) { // Windows
                    rgb_matrix_set_color(8, 0, 23, 255);  // W Blue (Selected)

                    rgb_matrix_set_color(13, 15, 230, 44); // A icacls grant
                    rgb_matrix_set_color(14, 15, 230, 44); // R icacls reset
                    rgb_matrix_set_color(16, 15, 230, 44); // T takeown
                    rgb_matrix_set_color(42, 15, 230, 44); // O icacls system owner
                    rgb_matrix_set_color(41, 15, 230, 44); // I icacls inheritance remove
                    rgb_matrix_set_color(22, 15, 230, 44); // D icacls remove deny
                }
            }
            break;
    }
}
