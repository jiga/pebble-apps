#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "umax4_8.h"

#define MY_UUID { 0xD0, 0x91, 0x45, 0xA4, 0x1B, 0xB4, 0x49, 0x73, 0x82, 0x3B, 0x23, 0x35, 0x8F, 0x77, 0xB4, 0x97 }
PBL_APP_INFO(MY_UUID,
             ":: Chess ::", "@_jiga",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

BmpContainer img_board_container;
BmpContainer img_select;
BmpContainer img_b_pawn;
BmpContainer img_b_bishop;
BmpContainer img_b_knight;
BmpContainer img_b_queen;
BmpContainer img_b_king;
BmpContainer img_b_rook;
BmpContainer img_w_pawn;
BmpContainer img_w_bishop;
BmpContainer img_w_knight;
BmpContainer img_w_queen;
BmpContainer img_w_king;
BmpContainer img_w_rook;

TextLayer strBoardLayer;
Layer board_layer;
Layer input_layer;

int input[9];

char *board;

static char strBoard[122];

static int xoffset=0,yoffset=6;

#define BLOCK GRect(7+17*col,18+17*row,10,10)
#define BOX GRect(4+17*xoffset,15+17*yoffset,16,16)
#define X_VALUE(x) (97+x)
#define Y_VALUE(y) (56-y)

void print_board(void)
{
 int N=-1,P=-1;
 while(++N<121)
   strBoard[++P] = N&8&&(N+=7)?10:PIECES[board[N]&15]; /* print board */
 strBoard[121] = '\0';
 //text_layer_set_text(&strBoardLayer, strBoard);
 layer_mark_dirty(&input_layer);
 layer_mark_dirty(&board_layer);
}

static void board_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

 int i=0,row=0,col=0;

 for(i=0; i<82; i++){
   switch(strBoard[i]) {
     case 10:
       col=0;
       row++;
       break;
     case '.':
       col++; 
       break;
     case '+':
       graphics_draw_bitmap_in_rect(ctx, &img_b_pawn.bmp, BLOCK);
       col++;
       break;
     case 'n':
       graphics_draw_bitmap_in_rect(ctx, &img_b_knight.bmp, BLOCK);
       col++;
       break;
     case 'k':
       graphics_draw_bitmap_in_rect(ctx, &img_b_king.bmp, BLOCK);
       col++;
       break;
     case 'b':
       graphics_draw_bitmap_in_rect(ctx, &img_b_bishop.bmp, BLOCK);
       col++;
       break;
     case 'r':
       graphics_draw_bitmap_in_rect(ctx, &img_b_rook.bmp, BLOCK);
       col++;
       break;
     case 'q':
       graphics_draw_bitmap_in_rect(ctx, &img_b_queen.bmp, BLOCK);
       col++;
       break;
     case '*':
       graphics_draw_bitmap_in_rect(ctx, &img_w_pawn.bmp, BLOCK);
       col++;
       break;
     case 'N':
       graphics_draw_bitmap_in_rect(ctx, &img_w_knight.bmp, BLOCK);
       col++;
       break;
     case 'K':
       graphics_draw_bitmap_in_rect(ctx, &img_w_king.bmp, BLOCK);
       col++;
       break;
     case 'B':
       graphics_draw_bitmap_in_rect(ctx, &img_w_bishop.bmp, BLOCK);
       col++;
       break;
     case 'R':
       graphics_draw_bitmap_in_rect(ctx, &img_w_rook.bmp, BLOCK);
       col++;
       break;
     case 'Q':
       graphics_draw_bitmap_in_rect(ctx, &img_w_queen.bmp, BLOCK);
       col++;
       break;
     default:
       break;
   }
 }
}

static void input_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

  graphics_draw_bitmap_in_rect(ctx, &img_select.bmp, BOX);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(yoffset==0) yoffset=7; 
  else yoffset--;
  print_board();
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(xoffset==7) xoffset=0; 
  else xoffset++;
  print_board();
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  static short idx = 0;
  static short userTurn = 1;
  
  if(userTurn == 1) {
    if(idx < 4) {
      input[idx++] = X_VALUE(xoffset);
      input[idx++] = Y_VALUE(yoffset);
    }
  
    if(idx == 4) {
      input[idx] = 10;
      userTurn = 0;
      idx = 0;
      chess_move(input);
    }
  }
  else {
    input[0] = 10;
    userTurn = 1;
    idx = 0;
    text_layer_set_text(&strBoardLayer, "thinking...");
    chess_move(input);
  }
  print_board();
}

static void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Chess");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BOARD, &img_board_container);
  bmp_init_container(RESOURCE_ID_IMAGE_SELECT, &img_select);
  bmp_init_container(RESOURCE_ID_IMAGE_B_PAWN, &img_b_pawn);
  bmp_init_container(RESOURCE_ID_IMAGE_B_BISHOP, &img_b_bishop);
  bmp_init_container(RESOURCE_ID_IMAGE_B_KNIGHT, &img_b_knight);
  bmp_init_container(RESOURCE_ID_IMAGE_B_QUEEN, &img_b_queen);
  bmp_init_container(RESOURCE_ID_IMAGE_B_KING, &img_b_king);
  bmp_init_container(RESOURCE_ID_IMAGE_B_ROOK, &img_b_rook);
  bmp_init_container(RESOURCE_ID_IMAGE_W_PAWN, &img_w_pawn);
  bmp_init_container(RESOURCE_ID_IMAGE_W_BISHOP, &img_w_bishop);
  bmp_init_container(RESOURCE_ID_IMAGE_W_KNIGHT, &img_w_knight);
  bmp_init_container(RESOURCE_ID_IMAGE_W_QUEEN, &img_w_queen);
  bmp_init_container(RESOURCE_ID_IMAGE_W_KING, &img_w_king);
  bmp_init_container(RESOURCE_ID_IMAGE_W_ROOK, &img_w_rook);
  
  layer_add_child(&window.layer, &img_board_container.layer.layer);

  layer_init(&input_layer, GRect(0,0, 144, 168));
  input_layer.update_proc = input_layer_update_callback;
  layer_add_child(&img_board_container.layer.layer, &input_layer);

  layer_init(&board_layer, GRect(0,0, 144, 168));
  board_layer.update_proc = board_layer_update_callback;
  layer_add_child(&input_layer, &board_layer);

  text_layer_init(&strBoardLayer, GRect(0, 0, 144/* width */, 11/* height */));
  text_layer_set_text_color(&strBoardLayer, GColorWhite);
  text_layer_set_background_color(&strBoardLayer, GColorBlack);
  //text_layer_set_font(&strBoardLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(&img_board_container.layer.layer, &strBoardLayer.layer);
  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

  board = chess_init();
  print_board();
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&img_board_container);
  bmp_deinit_container(&img_select);
  bmp_deinit_container(&img_b_pawn);
  bmp_deinit_container(&img_b_bishop);
  bmp_deinit_container(&img_b_knight);
  bmp_deinit_container(&img_b_queen);
  bmp_deinit_container(&img_b_king);
  bmp_deinit_container(&img_b_rook);
  bmp_deinit_container(&img_w_pawn);
  bmp_deinit_container(&img_w_bishop);
  bmp_deinit_container(&img_w_knight);
  bmp_deinit_container(&img_w_queen);
  bmp_deinit_container(&img_w_king);
  bmp_deinit_container(&img_w_rook);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
