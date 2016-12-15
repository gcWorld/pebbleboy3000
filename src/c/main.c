#include <pebble.h>
#include <time.h>
  
//#define SECONDS_PER_DAY ( 24 * 60 * 60 ) 
  
Window *my_window;
TextLayer *s_time_layer, *s_day_layer, *s_date_layer;
Layer *some_layer;
static BitmapLayer *s_background_layer, *s_battery_layer, *s_bt_layer;
static GBitmap *s_background_bitmap, *s_battery_charging,*s_battery_full,*s_battery_empty,*s_battery_2,*s_battery_3,*s_battery_4,*s_battery_5,*s_battery_6,*s_bt;

static void handle_battery(BatteryChargeState charge_state) {

  if (charge_state.is_charging) {
    bitmap_layer_set_bitmap(s_battery_layer, s_battery_charging);
  } else {
    if(charge_state.charge_percent>94) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_full); }
    else if(charge_state.charge_percent<11) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_empty); }
    else if(charge_state.charge_percent<26) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_2); }
    else if(charge_state.charge_percent<46) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_3); }
    else if(charge_state.charge_percent<66) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_4); }
    else if(charge_state.charge_percent<76) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_5); }
    else if(charge_state.charge_percent<95) { bitmap_layer_set_bitmap(s_battery_layer, s_battery_6); }
  }
}

void bt_handler(bool connected) {
  if (connected) {
    bitmap_layer_set_bitmap(s_bt_layer, NULL);
  } else {
    bitmap_layer_set_bitmap(s_bt_layer, s_bt);
  }
}

  time_t timeFromDate( int year, int month, int day ) { 
     time_t rawtime; 
     struct tm * my_time; 
  
     // Create a filled in time structure 
     time( &rawtime ); 
     my_time = localtime( &rawtime ); 
      
     // Reassign our date 
     my_time->tm_year     = year - 1900; // Different sources say 1900 and 1970? 
     my_time->tm_mon        = month - 1;    // tm uses uses january + months [0..11]  
     my_time->tm_mday     = day; 
  
     // Return it as seconds since epoch 
     return( mktime( my_time ) ); 
 } 

static void calcFallout() {
  time_t start_date, end_date; 
     int days; 
  
     start_date     = timeFromDate( 2015, 06, 20 ); 
     end_date     = timeFromDate( 2015, 11, 10 ); 
  
     days = difftime( end_date, start_date) / SECONDS_PER_DAY; 
     char * str = malloc(3);
     snprintf(str, 3, "%d", days);
     APP_LOG(APP_LOG_LEVEL_INFO, "Days:");
  APP_LOG(APP_LOG_LEVEL_INFO, str);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  static char s_day_buffer[] = "Wednesday12";
                                //Donnerstag
  //Write current day
  strftime(s_day_buffer, sizeof(s_day_buffer), "%A", tick_time);
  text_layer_set_text(s_day_layer, s_day_buffer);
  
  
  static char s_date_buffer[] = "01.02.1234";
  
  //Write current date
  strftime(s_date_buffer, sizeof(s_date_buffer), "%x", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  handle_battery(battery_state_service_peek());
  bt_handler(bluetooth_connection_service_peek());
}

static void some_update_proc(Layer *this_layer, GContext *ctx) {
  // Draw things here using ctx
  GPoint p0 = GPoint(0, 0);
  GPoint p1 = GPoint(144, 144);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, p0, p1);
}

void handle_init(void) {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(handle_battery);
  bluetooth_connection_service_subscribe(bt_handler);
  
  my_window = window_create();
  
  s_battery_charging = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
  s_battery_empty = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_EMPTY);
  s_battery_2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_2);
  s_battery_3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_3);
  s_battery_4 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_4);
  s_battery_5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_5);
  s_battery_6 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_6);
  s_battery_full = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_FULL);
  s_bt = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_BT);
  
  
  
  
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEBBLE_BOY_BG);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(s_background_layer));
  
   s_battery_layer = bitmap_layer_create(GRect(25, 0, 20, 20));
  bitmap_layer_set_bitmap(s_battery_layer, s_battery_full);
  layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(s_battery_layer));
  
  s_bt_layer = bitmap_layer_create(GRect(66,99,20,37));
  bitmap_layer_set_bitmap(s_bt_layer, NULL);
  layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(s_bt_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 10, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  #ifdef PBL_COLOR 
    text_layer_set_text_color(s_time_layer, GColorIslamicGreen);
  #else
    text_layer_set_text_color(s_time_layer, GColorWhite);
  #endif
  text_layer_set_text(s_time_layer, "20:43");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_time_layer));
  
  s_day_layer = text_layer_create(GRect(20, 50, 144, 30));
  text_layer_set_background_color(s_day_layer, GColorClear);
  #ifdef PBL_COLOR 
    text_layer_set_text_color(s_day_layer, GColorIslamicGreen);
  #else
    text_layer_set_text_color(s_day_layer, GColorWhite);
  #endif
  text_layer_set_text(s_day_layer, "Sunday");
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentLeft);
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_day_layer));
  
  s_date_layer = text_layer_create(GRect(20, 75, 144, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  #ifdef PBL_COLOR 
    text_layer_set_text_color(s_date_layer, GColorIslamicGreen);
  #else
    text_layer_set_text_color(s_date_layer, GColorWhite);
  #endif
  text_layer_set_text(s_date_layer, "07.06.2015");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  
  layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(s_date_layer));
  
  window_stack_push(my_window, true);
  
  some_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(some_layer, some_update_proc);
  
  calcFallout();
  
  update_time();
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_date_layer);
  window_destroy(my_window);
}

int main(void) {
  setlocale(LC_TIME, "");
  handle_init();
  app_event_loop();
  handle_deinit();
}
