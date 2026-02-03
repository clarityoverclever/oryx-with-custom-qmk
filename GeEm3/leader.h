// leader.h
#pragma once
#include "quantum.h"

typedef enum {
    CAT_NONE,
    CAT_GIT,
    CAT_CODE,
    CAT_SYSTEM,
    CAT_QUICK,
} leader_category_t;

typedef enum {
    ACT_NONE,

    // Quick actions
    ACT_QUICK_KILL_CLEAR,
	ACT_QUICK_SUDO_RERUN,
    ACT_QUICK_EMAIL,

    // Git actions
    ACT_GIT_COMMIT,
	ACT_GIT_FETCH,
    ACT_GIT_PULL,
    ACT_GIT_PUSH,
    ACT_GIT_STATUS,
    ACT_GIT_STASH_PUSH,
    ACT_GIT_STASH_POP,
    ACT_GIT_BRANCH,
    ACT_GIT_BRANCH_NEW,
    ACT_GIT_BRANCH_CHECKOUT,
    ACT_GIT_ADD,
    ACT_GIT_LOG,
    ACT_GIT_DIFF,
    ACT_GIT_DIFF_STAGED,
    ACT_GIT_REBASE,
    ACT_GIT_REBASE_CONTINUE,
    ACT_GIT_REBASE_ABORT,
    ACT_GIT_RESTORE,
    ACT_GIT_RESET,
    ACT_GIT_UNSTAGE,
    ACT_GIT_FIND_CONFLICT,

    // Code actions
	ACT_CODE_GO_ERROR,
    ACT_CODE_PS_SET_PATH,

    // System actions
    ACT_SYSTEM_WIN_TAKEOWN,
    ACT_SYSTEM_WIN_ICACLS_GRANT,
    ACT_SYSTEM_WIN_ICACLS_RESET,
    ACT_SYSTEM_WIN_ICACLS_SET_SYSTEM,
    ACT_SYSTEM_WIN_ICACLS_INH_REMOVE,
    ACT_SYSTEM_WIN_ICACLS_REMOVE_DENY,

} leader_action_t;

// Leader sequence structure
typedef struct {
    uint16_t key1;          // First key (0 for unused)
    uint16_t key2;          // Second key (0 for unused)
    uint16_t key3;          // Third key (0 for unused)
    leader_action_t action;
    const char* output;     // String to send
} leader_sequence_t;

// Global state tracking
typedef struct {
    uint8_t step;           // Current step in sequence (0, 1, 2, 3)
    uint16_t first_key;     // First leader key pressed
    uint16_t second_key;    // Second leader key pressed
    uint16_t third_key;     // Third leader key pressed
    uint8_t last_rgb_mode;  // Saved RGB mode
} leader_state_t;

// Function declarations
void leader_start_logic(void);
void leader_end_logic(void);
void leader_visual_logic(void);

// State accessor (defined in leader.c)
leader_state_t* get_leader_state(void);
