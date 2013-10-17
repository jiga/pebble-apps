#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "scorer.h"
#include "mini-printf.h"


#define MY_UUID { 0x73, 0x6D, 0xD5, 0x28, 0x48, 0x76, 0x41, 0xC2, 0xA2, 0xF0, 0x2E, 0x60, 0x3D, 0xC9, 0x01, 0xAC }
PBL_APP_INFO(MY_UUID,
             "Cricket Scorer", "@_jiga",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

BmpContainer image_container;
BmpContainer buttons_container;

Layer scoreBoard;
Layer buttonsLayer;
TextLayer inputLayer;
TextLayer topLayer;
TextLayer headerLayer;
TextLayer scoreLayer;
TextLayer statsLayer;
TextLayer historyLayer;

static ScoreBoard* current; 

static unsigned short team;

static char *teamLabel[] = {"Team A", "Team B"};
static unsigned short keyIndex = 8;

static char strHeader[100];
static char strScore[100];
static char strStats[100];
static char strHistory[100];

static short flashImage = 1;

void displayScore(void) {
    mini_snprintf(strHeader,99, "::: %d%c%c inning :::\n"
        , team+1
        , (team)?'n':'s'
        , (team)?'d':'t'
    );

    mini_snprintf(strScore,99, "%d/%d\n"
	, current->runs
	, current->wickets
    );
        
    mini_snprintf(strStats,99, "%d.%d overs\n RR: %d.%d\n"
	, current->balls/6
	, (current->balls - (6*(current->balls/6)))
        , (current->runs*6/current->balls)
        , (current->runs*600/current->balls)-(current->runs*6/current->balls)*100
    );

    mini_snprintf(strHistory,99, "%c %c %c %c %c %c %c %c %c %c %c %c\n"
        , keyPad[keyHistory[(keyPos+1)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+2)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+3)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+4)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+5)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+6)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+7)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+8)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+9)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+10)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+11)%MAX_HISTORY]]
        , keyPad[keyHistory[(keyPos+0)%MAX_HISTORY]]
        );

    text_layer_set_text(&headerLayer, strHeader);
    text_layer_set_text(&scoreLayer, strScore);
    text_layer_set_text(&statsLayer, strStats);
    text_layer_set_text(&historyLayer, strHistory);
}

int flashImageIsDisplayed(void) {
  if(flashImage) {
    flashImage = 0;
    current = getScore(team);
    layer_set_hidden(&image_container.layer.layer, true);
    layer_set_hidden(&scoreBoard, false);
    layer_set_hidden(&buttonsLayer, false);
    displayScore();
    return 1;
  }
  return 0;
}
// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  if(flashImageIsDisplayed()) return;

  keyIndex = sizeof(keyPad) - 1;

  layer_set_hidden(&inputLayer.layer, false);
  text_layer_set_text(&inputLayer, keyLabel[keyIndex]);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(flashImageIsDisplayed()) return;

  if (keyIndex == 8) keyIndex=1;
  else if (keyIndex == 7) keyIndex+=2;
  else if (keyIndex < sizeof(keyPad)-3) keyIndex++;
  else keyIndex = 0;

  if(keyIndex) {
    layer_set_hidden(&inputLayer.layer, false);
  }
  else {
    layer_set_hidden(&inputLayer.layer, true);
  }
  text_layer_set_text(&inputLayer, keyLabel[keyIndex]);
}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(flashImageIsDisplayed()) return;

  layer_set_hidden(&inputLayer.layer, true);

  if(keyIndex == 0) keyIndex = 8;
  score(keyIndex);
  displayScore();
  keyIndex = 8;
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  team ^= 1;
  current = getScore(team);
  score(12);
  displayScore();
  keyIndex = 8;
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

  window_init(&window, "MainMenu");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  // Note: This needs to be "de-inited" in the application's
  //       deinit handler.
  bmp_init_container(RESOURCE_ID_IMAGE_CRICSCORER, &image_container);
  bmp_init_container(RESOURCE_ID_IMAGE_BUTTONS, &buttons_container);

  layer_add_child(&window.layer, &image_container.layer.layer);

  layer_init(&scoreBoard, window.layer.frame);
  layer_add_child(&window.layer, &scoreBoard);

  text_layer_init(&topLayer, GRect(5,0,144-24,20));
  text_layer_set_text_color(&topLayer, GColorBlack);
  text_layer_set_background_color(&topLayer, GColorWhite);
  text_layer_set_text_alignment(&topLayer, GTextAlignmentCenter);
  text_layer_set_font(&topLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(&topLayer, ":::Cricket Scorer:::");

  text_layer_init(&headerLayer, GRect(5,18,144-24,22));
  text_layer_set_text_color(&headerLayer, GColorBlack);
  text_layer_set_background_color(&headerLayer, GColorWhite);
  text_layer_set_text_alignment(&headerLayer, GTextAlignmentCenter);
  text_layer_set_font(&headerLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_layer_init(&scoreLayer, GRect(5,40,144-19,50));
  text_layer_set_text_color(&scoreLayer, GColorWhite);
  text_layer_set_background_color(&scoreLayer, GColorBlack);
  text_layer_set_text_alignment(&scoreLayer, GTextAlignmentCenter);
  text_layer_set_font(&scoreLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));

  text_layer_init(&statsLayer, GRect(5,90,144-24,40));
  text_layer_set_text_color(&statsLayer, GColorWhite);
  text_layer_set_background_color(&statsLayer, GColorBlack);
  text_layer_set_text_alignment(&statsLayer, GTextAlignmentCenter);
  text_layer_set_font(&statsLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_layer_init(&historyLayer, GRect(5,128,144-24,34));
  text_layer_set_text_color(&historyLayer, GColorBlack);
  text_layer_set_background_color(&historyLayer, GColorWhite);
  text_layer_set_text_alignment(&historyLayer, GTextAlignmentCenter);
  text_layer_set_font(&historyLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  text_layer_init(&inputLayer, GRect(26, 60, 144-40 /* width */, 38 /* height */));
  text_layer_set_text_color(&inputLayer, GColorBlack);
  text_layer_set_background_color(&inputLayer, GColorWhite);
  text_layer_set_text_alignment(&inputLayer, GTextAlignmentCenter);
  text_layer_set_font(&inputLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  layer_init(&buttonsLayer, GRect(144-14, 10, 14 /* width */, 136 /* height */));
  layer_add_child(&buttonsLayer, &buttons_container.layer.layer);

  layer_set_hidden(&scoreBoard, true);
  layer_set_hidden(&buttonsLayer, true);
  layer_set_hidden(&inputLayer.layer, true);
  layer_add_child(&scoreBoard, &buttonsLayer);
  layer_add_child(&scoreBoard, &topLayer.layer);
  layer_add_child(&scoreBoard, &headerLayer.layer);
  layer_add_child(&scoreBoard, &scoreLayer.layer);
  layer_add_child(&scoreBoard, &statsLayer.layer);
  layer_add_child(&scoreBoard, &historyLayer.layer);
  layer_add_child(&scoreBoard, &inputLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&image_container);
  bmp_deinit_container(&buttons_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
