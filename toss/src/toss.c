#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "mt_prng.h"


#define MY_UUID { 0x4C, 0x3F, 0x63, 0x27, 0x00, 0x5B, 0x4A, 0x4E, 0xA0, 0xD7, 0x5F, 0x1E, 0xFD, 0x86, 0xDA, 0xA7 }
PBL_APP_INFO(MY_UUID,
             "Coin Toss", "@_jiga",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

#define HEAD 0
#define TAIL 1

static char* coinFace[] = {"HEAD", "TAIL"};

Window window;

TextLayer inputLayer;

BmpContainer head_container;
BmpContainer tail_container;

void toss_init()
{
  PblTm t;
  get_time(&t);
    
  unsigned int __seed = t.tm_sec	// start seconds
    + t.tm_min*60 // add minutes
    + t.tm_hour*3600 // add hours
    + t.tm_yday*86400 // add days
    + (t.tm_year-70)*31536000 // add years since 1970
    + ((t.tm_year-69)/4)*86400 // add a day after leap years, starting in 1973
    - ((t.tm_year-1)/100)*86400 // remove a leap day every 100 years, starting in 2001
    + ((t.tm_year+299)/400)*86400; // add a leap day back every 400 years, starting in 2001

  mt_random_init(__seed);
}

unsigned int toss()
{
  return mt_random(2);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  layer_set_hidden(&tail_container.layer.layer, true);
  layer_set_hidden(&head_container.layer.layer, false);
  text_layer_set_text(&inputLayer, coinFace[HEAD]);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  layer_set_hidden(&head_container.layer.layer, true);
  layer_set_hidden(&tail_container.layer.layer, false);
  text_layer_set_text(&inputLayer, coinFace[TAIL]);
}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(toss())
  {
    layer_set_hidden(&head_container.layer.layer, true);
    layer_set_hidden(&tail_container.layer.layer, false);
    text_layer_set_text(&inputLayer, coinFace[TAIL]);
  }
  else
  {
    layer_set_hidden(&head_container.layer.layer, false);
    layer_set_hidden(&tail_container.layer.layer, true);
    text_layer_set_text(&inputLayer, coinFace[HEAD]);
  }
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}

// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Coin Toss");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_COIN_HEAD, &head_container);
  bmp_init_container(RESOURCE_ID_COIN_TAIL, &tail_container);

  layer_add_child(&window.layer, &head_container.layer.layer);
  layer_add_child(&window.layer, &tail_container.layer.layer);

  text_layer_init(&inputLayer, GRect(40, 120, 144-40 /* width */, 38 /* height */));
  text_layer_set_text_color(&inputLayer, GColorWhite);
  text_layer_set_background_color(&inputLayer, GColorClear);
  text_layer_set_font(&inputLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(&window.layer, &inputLayer.layer);
  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
  
  toss_init();
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&head_container);
  bmp_deinit_container(&tail_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
