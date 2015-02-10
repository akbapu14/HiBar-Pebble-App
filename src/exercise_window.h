/* ========================================================================== */
/*
 * exercise_window.h
 *
 * Project Name: hibar
 */
/* ========================================================================== */
#ifndef EXERCISE_H
#define EXERCISE_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros
Window *exercise_window_init(char* exercise_name, char *weight, int sets,
    int reps, int current_set_number);

#endif // EXERCISE_H
