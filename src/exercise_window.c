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

// ---------------- Constant definitions
#define THRESHOLD -1000

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
static Window *window;
static TextLayer *exercise_name_text_layer;
static TextLayer *sets_and_rep_count_text_layer;
static TextLayer *rep_count_text_layer;
static TextLayer *weight_text_layer;
static TextLayer *next_set_text_layer;
static TextLayer *set_count_text_layer;
static bool previous_reading_below_threshold = false;
static int number_reps = 0;
static char *exercise_name_string;
static char *weight_string;
static int current_set;
static int num_of_sets;
static int reps_per_set;
static bool user_notified_set_completed = false;
static char reps[20];
static char sets[20];
static char sets_and_rep_text[20];

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);
static void data_handler(AccelData *data, uint32_t num_samples);
static void show_continue_prompt();
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void click_config_provider(void *context);

/* ========================================================================== */

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (user_notified_set_completed) {
        go_to_next_exercise();
    }
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

Window *exercise_window_init(char* exercise_name, char *weight, int sets,
    int reps, int current_set_number) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = load,
    .unload = unload,
  });

  exercise_name_string = exercise_name;
  weight_string = weight;
  num_of_sets = sets;
  reps_per_set = reps;
  current_set = current_set_number;

  return window;
}

static void data_handler(AccelData *data, uint32_t num_samples) {
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d, %d", data[0].x, data[1].x, data[2].x);

  bool not_done = number_reps / 2 < reps_per_set;

  bool sample0_below = data[0].x < THRESHOLD;
  bool sample1_below = data[1].x < THRESHOLD;
  bool sample2_below = data[2].x < THRESHOLD;

  bool all_below = sample0_below && sample1_below && sample2_below;

  if (not_done && all_below && !previous_reading_below_threshold) {
    number_reps++;
    previous_reading_below_threshold = true;

    snprintf(reps, sizeof(reps), "%d", number_reps / 2);
    text_layer_set_text(rep_count_text_layer, reps);

    // APP_LOG(APP_LOG_LEVEL_DEBUG, "NEW REP COUNT: %s", reps);
    if (!user_notified_set_completed && number_reps / 2 == reps_per_set) {
      show_continue_prompt();
    }

  } else if (!(sample0_below || sample1_below || sample2_below)) {
    previous_reading_below_threshold = false;
  }
}

static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // set up exercise name text layer
  exercise_name_text_layer = text_layer_create((GRect) {
    .origin = { 0, 5 },
    .size = { bounds.size.w, 20 }
  });
  text_layer_set_text(exercise_name_text_layer, exercise_name_string);
  text_layer_set_font(exercise_name_text_layer,
    fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(exercise_name_text_layer, GTextAlignmentCenter);

  // set up weight text layer
  weight_text_layer = text_layer_create((GRect) {
    .origin = { 0, 27 },
    .size = { bounds.size.w, 20 }
  });
  text_layer_set_text(weight_text_layer, weight_string);
  text_layer_set_text_alignment(weight_text_layer, GTextAlignmentCenter);

  // set up set and rep count text layer
  sets_and_rep_count_text_layer = text_layer_create((GRect) {
    .origin = { 0, 130},
    .size = { bounds.size.w, 20 }
  });

  snprintf(sets_and_rep_text, sizeof(sets_and_rep_text), "%d sets of %d reps", num_of_sets, reps_per_set);
  text_layer_set_text(sets_and_rep_count_text_layer, sets_and_rep_text);
  text_layer_set_text_alignment(sets_and_rep_count_text_layer, GTextAlignmentCenter);

  // set up rep count text layer
  rep_count_text_layer = text_layer_create((GRect) {
    .origin = { 0, 50 },
    .size = { bounds.size.w, 50 }
  });
  text_layer_set_text(rep_count_text_layer, "0");
  text_layer_set_font(rep_count_text_layer,
    fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(rep_count_text_layer, GTextAlignmentCenter);

  next_set_text_layer = text_layer_create((GRect) {
    .origin = { 0, 60 },
    .size = { bounds.size.w, 50 }
  });
  text_layer_set_text(next_set_text_layer, "Start next set ->");
  text_layer_set_text_alignment(next_set_text_layer, GTextAlignmentCenter);

  set_count_text_layer = text_layer_create((GRect) {
    .origin = {0, 110},
    .size = { bounds.size.w, 20}
  });
  snprintf(sets, sizeof(sets), "Set #%d", current_set);
  text_layer_set_text(set_count_text_layer, sets);
  text_layer_set_text_alignment(set_count_text_layer, GTextAlignmentCenter);

  // add text layers to window
  layer_add_child(window_layer, text_layer_get_layer(exercise_name_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(sets_and_rep_count_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(weight_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(set_count_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(rep_count_text_layer));

  accel_data_service_subscribe(3, data_handler);
}

static void show_continue_prompt() {
  layer_remove_from_parent(text_layer_get_layer(rep_count_text_layer));
  layer_add_child(window_get_root_layer(window),
  text_layer_get_layer(next_set_text_layer));
  vibes_short_pulse();
  user_notified_set_completed = true;
}

static void unload(Window *window) {
  text_layer_destroy(exercise_name_text_layer);
  // TODO: free up resources
}
