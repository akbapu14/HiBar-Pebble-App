/* ========================================================================== */
/*
 * exercise_window.c
 *
 * Project Name: hibar
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>
#include <stdio.h>

// ---------------- Local includes  e.g., "file.h"
#include "next_exercise_window.h"
#include "message_utils.h"
#include "next_exercise_window.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
static Window *window;
static TextLayer *waiting_text_layer;

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);

/* ========================================================================== */

void go_to_next_exercise() {
  window_stack_pop(true);
  window_stack_push(next_exercise_window_init(), true);
}

Window *next_exercise_window_init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = load,
    .unload = unload,
  });

  return window;
}

static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // set up exercise name text layer
  waiting_text_layer = text_layer_create((GRect) {
    .origin = { 0, 50 },
    .size = { bounds.size.w, 50 }
  });
  text_layer_set_text(waiting_text_layer, "Retrieving next \nexercise...");
  text_layer_set_font(waiting_text_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(waiting_text_layer, GTextAlignmentCenter);

  // add text layers to window
  layer_add_child(window_layer, text_layer_get_layer(waiting_text_layer));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "sending start message");

  // TODO: send next message to mobile app
  send_msg(1, "Next");
}

static void unload(Window *window) {
  text_layer_destroy(waiting_text_layer);
  // TODO: free up resources
}
