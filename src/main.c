#include "pebble.h"

#include "string.h"
#include "stdlib.h"

Layer *date_layer;
TextLayer *time_layer;

static BitmapLayer *meri_image_layer;
static GBitmap *meri_image;

Window *window;

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00";

  strftime(time_text, sizeof(time_text), "%I:%M", tick_time);
  text_layer_set_text(time_layer, time_text);

  layer_mark_dirty(window_get_root_layer(window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  meri_image_layer = bitmap_layer_create(bounds);
  meri_image = gbitmap_create_with_resource(RESOURCE_ID_MERI);
	
  #if defined(PBL_BW)
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpAssign);
  #elif PBL_ROUND
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpSet);
  #elif defined(PBL_COLOR)
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpSet);
  #endif
	
  bitmap_layer_set_bitmap(meri_image_layer, meri_image);
  bitmap_layer_set_alignment(meri_image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(meri_image_layer));

  // Digital clock
  time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(135, 120), PBL_IF_ROUND_ELSE(bounds.size.w, 138), PBL_IF_ROUND_ELSE(100, 100)));
  text_layer_set_text_alignment(time_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
  text_layer_set_text_color(time_layer, PBL_IF_ROUND_ELSE(GColorWhite, GColorBlack));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS, FONT_KEY_BITHAM_42_MEDIUM_NUMBERS)));

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(meri_image);
  bitmap_layer_destroy(meri_image_layer);
  layer_destroy(date_layer);
  text_layer_destroy(time_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}