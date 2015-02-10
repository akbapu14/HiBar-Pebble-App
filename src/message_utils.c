/* ========================================================================== */
/*
 * message_utils.c
 *
 * Project Name: Event Menu
 * Author: cheniel
 *
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>
#include <string.h>

// ---------------- Local includes  e.g., "file.h"
#include "exercise_window.h"
#include "next_exercise_window.h"
#include "message_utils.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define MSG_KEY_GENERIC_STRING 0

// outgoing messages
#define MSG_KEY_NEXT 1

// incoming messages
#define MSG_KEY_START_EXERCISE 10
#define MSG_KEY_EXERCISE_NAME 11
#define MSG_KEY_EXERCISE_WEIGHT 12
#define MSG_KEY_EXERCISE_REPS 13
#define MSG_KEY_EXERCISE_SETS 14
#define MSG_KEY_EXERCISE_CURRENT_SET 15

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer *response_text_layer;
static char* response_text;

// ---------------- Private prototypes
static void outbox_fail_handler(DictionaryIterator *iterator,
                         AppMessageResult reason, void *context);
static void outbox_sent_handler(DictionaryIterator *iterator,
                         void *context);
static void response_load(Window *window);
static void response_unload(Window *window);


/* ========================================================================== */

static void response_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  response_text_layer = text_layer_create((GRect) {
      .origin = { 0, 72 },
      .size = { bounds.size.w, 20 }
  });

  text_layer_set_text(response_text_layer, response_text);
  text_layer_set_text_alignment(response_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(response_text_layer));
}

static void response_unload(Window *window) {
  text_layer_destroy(response_text_layer);
}

static void load_response_window(char *response) {
  response_text = response;
  Window *response_window = window_create();
  window_set_window_handlers(response_window, (WindowHandlers) {
    .load = response_load,
    .unload = response_unload,
  });

  window_stack_push(response_window, true);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Receiving some message");

  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  static char exercise_name[64];
  static char exercise_weight[64];
  int exercise_reps = -1;
  int exercise_sets = -1;
  int current_set = -1;

  // Process all pairs present
  while (t != NULL) {
    // Long lived buffer
    static char s_buffer[64];

    // Process this pair's key
    switch (t->key) {
      case MSG_KEY_GENERIC_STRING:
        // Copy value and display
        snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", t->value->cstring);
        APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
        break;

      case MSG_KEY_START_EXERCISE:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "found start exercise");

        window_stack_pop_all(false);
        window_stack_push(next_exercise_window_init(), true);
        break;

      case MSG_KEY_EXERCISE_NAME: // receive workout data
        snprintf(exercise_name, sizeof(exercise_name), "%s", t->value->cstring);
        break;

      case MSG_KEY_EXERCISE_WEIGHT:
        snprintf(exercise_weight, sizeof(exercise_weight), "%s", t->value->cstring);
        break;

      case MSG_KEY_EXERCISE_REPS:
        exercise_reps = (int)t->value->int32;
        break;

      case MSG_KEY_EXERCISE_SETS:
        exercise_sets = (int)t->value->int32;
        break;

      case MSG_KEY_EXERCISE_CURRENT_SET:
        current_set = (int)t->value->int32;
        break;

    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }

  if (exercise_reps != -1 && exercise_sets != -1 && current_set != -1) {
    window_stack_pop(true);
    window_stack_push(exercise_window_init(
      exercise_name, exercise_weight, exercise_reps, exercise_sets, current_set), true);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "pushing new exercise %s, %s, %d, %d, %d", exercise_name, exercise_weight, exercise_reps, exercise_sets, current_set);
  }
}

char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %s", translate_error(reason));
}

static void outbox_fail_handler(DictionaryIterator *iterator,
                         AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message failed.");
}

static void outbox_sent_handler(DictionaryIterator *iterator,
                         void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message sent!");
}

void init_app_message() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_fail_handler);
  app_message_register_outbox_sent(outbox_sent_handler);
  app_message_open(app_message_inbox_size_maximum(),
                   app_message_outbox_size_maximum());
}

void send_msg(int key, char *msg) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, key, msg);
  app_message_outbox_send();
}
