#include "pebble.h"

static BitmapLayer *meri_image_layer;
static GBitmap *meri_image;

Window *window;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  meri_image_layer = bitmap_layer_create(bounds);

  #if defined(PBL_BW)
  meri_image = gbitmap_create_with_resource(RESOURCE_ID_MERI);
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpAssign);
  #elif PBL_ROUND
  meri_image = gbitmap_create_with_resource(RESOURCE_ID_MERI_ROUND);
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpSet);
  #elif defined(PBL_COLOR)
  meri_image = gbitmap_create_with_resource(RESOURCE_ID_MERI_COLOR);
  bitmap_layer_set_compositing_mode(meri_image_layer, GCompOpSet);
  #endif
	
  bitmap_layer_set_bitmap(meri_image_layer, meri_image);
  bitmap_layer_set_alignment(meri_image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(meri_image_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(meri_image);
  bitmap_layer_destroy(meri_image_layer);
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
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}