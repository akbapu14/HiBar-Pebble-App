/* ========================================================================== */
/*
 * waiting_window.c
 *
 * Project Name: hibar
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "exercise_window.h"
#include "message_utils.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
static Window *window;
static TextLayer *text_layer;

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);

/* ========================================================================== */

Window *waiting_window_init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = load,
    .unload = unload,
  });
  return window;
}

// static void show_exercise_window(void *data) {
//   // TODO: if received message from ios, open window if the data
//   Window *exercise_window = exercise_window_init("Bench Press", "150lbs", 5, 5, 1);
//   window_stack_push(exercise_window, true);
// }

static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) {
    .origin = { 0, 50 },
    .size = { bounds.size.w, 50 }
  });

  text_layer_set_text(text_layer, "Select a workout \nin the mobile app");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  // app_timer_register(100, show_exercise_window, NULL);

}

static void unload(Window *window) {
  text_layer_destroy(text_layer);
}
