/* ========================================================================== */
/*
 * hibar-pebble.c
 *
 * Project Name: hibar
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "waiting_window.h"
#include "message_utils.h"

// ---------------- Constant definitions

// ---------------- Macro definitions


// ---------------- Structures/Types

// ---------------- Private variables
static Window *window;

// ---------------- Private prototypes
int main(void);
static void init(void);
static void deinit(void);

/* ========================================================================== */

static void init(void) {
  init_app_message();
  window = waiting_window_init();
  window_stack_push(window, true);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
