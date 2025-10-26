#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"
#include "lvgl/src/drivers/lv_drivers.h"

#include "SDL2/SDL.h"

#include "libs/cities.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 600

// Global display object for theme switching
static lv_display_t* g_display;
static lv_display_t * hal_init(int32_t w, int32_t h);

lv_obj_t* main_screen;
lv_obj_t* city_management_screen;
lv_obj_t* weather_overview_screen;
lv_obj_t* settings_screen;

lv_obj_t* virtual_keyboard;
void show_virtual_keyboard_cb(lv_event_t * e);
void hide_virtual_keyboard_cb(lv_event_t * e);
void textarea_value_changed_cb(lv_event_t * e);

void create_main_screen();
void create_city_management_screen();
void create_weather_overview_screen();
void create_settings_screen();

//======== Navigation Callbacks ========//

void go_to_city_management_screen_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_screen_load(city_management_screen);
    }
}

void go_to_weather_overview_screen_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_screen_load(weather_overview_screen);
    }
}

void go_to_settings_screen_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_screen_load(settings_screen);
    }
}

void go_to_main_screen_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_screen_load(main_screen);
    }
}

//======== Functional Callbacks ========//

void load_default_cities_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        printf("Load Default Cities button clicked!\n");
        cities_t* cities;
        if (cities_init(&cities) != 0) {
            printf("Failed to initialize cities list.\n");
            return;
        }
        printf("\033[H\033[J");
        cities_print(cities);
        cities_dispose(&cities);
    }
}

void process_weather_search_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // Get the city name input from user data (passed when setting up the event)
        lv_obj_t* city_name_input = (lv_obj_t*)lv_event_get_user_data(e);
        
        // Check if we have a valid input object
        if (city_name_input == NULL) {
            printf("Error: No input field found!\n");
            return;
        }
        
        const char* city_name = lv_textarea_get_text(city_name_input);
        
        // Check if the text is empty or null
        if (city_name == NULL || strlen(city_name) == 0) {
            printf("Please enter a city name!\n");
            return;
        }
        
        printf("Searching weather for city: %s\n", city_name);
        // Here you would add code to fetch and display weather data
    }
}

void switch_theme_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        static bool is_dark_theme = true;
        if (is_dark_theme) {
            lv_theme_t* light_theme = lv_theme_default_init(g_display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                                            true, LV_FONT_DEFAULT);
            lv_display_set_theme(g_display, light_theme);
            is_dark_theme = false;
        } else {
            lv_theme_t* dark_theme = lv_theme_default_init(g_display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                                           false, LV_FONT_DEFAULT);
            lv_display_set_theme(g_display, dark_theme);
            is_dark_theme = true;
        }
    }
}

void power_off_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        printf("Power Off button clicked! Exiting application...\n");
        exit(0);
    }
}

//======== Screen Creation Functions ========//

void create_main_screen()
{
    main_screen = lv_obj_create(NULL);
    
    // Set flex layout for the main container
    lv_obj_set_flex_flow(main_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(main_screen, 20, 0);
    lv_obj_set_style_pad_gap(main_screen, 20, 0);
    
    // Title
    lv_obj_t* title = lv_label_create(main_screen);
    lv_label_set_text(title, "Garfields Weather Station");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);  // Use larger built-in font

    // Button container with consistent sizing
    lv_obj_t* btn_container = lv_obj_create(main_screen);
    lv_obj_set_size(btn_container, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, 15, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, 0);
    
    // City Management button
    lv_obj_t* settings_btn = lv_btn_create(btn_container);
    lv_obj_set_size(settings_btn, 200, 50);
    lv_obj_add_event_cb(settings_btn, go_to_city_management_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* settings_label = lv_label_create(settings_btn);
    lv_label_set_text(settings_label, "City Management");
    lv_obj_center(settings_label);

    // Weather Overview button
    lv_obj_t* weather_btn = lv_btn_create(btn_container);
    lv_obj_set_size(weather_btn, 200, 50);
    lv_obj_add_event_cb(weather_btn, go_to_weather_overview_screen_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* weather_label = lv_label_create(weather_btn);
    lv_label_set_text(weather_label, "Weather Overview");
    lv_obj_center(weather_label);

    // Settings button
    lv_obj_t* config_btn = lv_btn_create(btn_container);
    lv_obj_set_size(config_btn, 200, 50);
    lv_obj_add_event_cb(config_btn, go_to_settings_screen_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* config_label = lv_label_create(config_btn);
    lv_label_set_text(config_label, "Settings");
    lv_obj_center(config_label);

    // Turn off button
    lv_obj_t* power_btn = lv_btn_create(btn_container);
    lv_obj_set_size(power_btn, 200, 50);
    lv_obj_add_event_cb(power_btn, power_off_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* power_label = lv_label_create(power_btn);
    lv_label_set_text(power_label, "Power Off");
    lv_obj_center(power_label);
}

void create_city_management_screen()
{
    city_management_screen = lv_obj_create(NULL);

    // Set flex layout for the main container
    lv_obj_set_flex_flow(city_management_screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(city_management_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(city_management_screen, 20, 0);
    lv_obj_set_style_pad_gap(city_management_screen, 20, 0);

    // Sidebar panel
    lv_obj_t* panel_sidebar = lv_obj_create(city_management_screen);
    lv_obj_set_size(panel_sidebar, LV_PCT(24), LV_PCT(99));
    //lv_obj_set_style_bg_color(panel_sidebar, lv_color_hex(0x2E86C1), 0);
    lv_obj_set_style_border_opa(panel_sidebar, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(panel_sidebar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel_sidebar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(panel_sidebar, 10, 0);
    lv_obj_set_style_pad_gap(panel_sidebar, 15, 0);
    
    // Main panel
    lv_obj_t* panel_main = lv_obj_create(city_management_screen);
    lv_obj_set_size(panel_main, LV_PCT(74), LV_PCT(99));
    //lv_obj_set_style_bg_color(panel_main, lv_color_hex(0xD6EAF8), 0);
    lv_obj_set_style_border_opa(panel_main, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(panel_main, 0, 0);

    // Button load default cities
    lv_obj_t* btn_load_defaults = lv_btn_create(panel_sidebar);
    lv_obj_set_size(btn_load_defaults, LV_PCT(80), 50);
    lv_obj_add_event_cb(btn_load_defaults, load_default_cities_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* load_defaults_label = lv_label_create(btn_load_defaults);
    lv_label_set_text(load_defaults_label, "Load Default Cities");
    lv_obj_center(load_defaults_label);

    // Button add city
    lv_obj_t* btn_add_city = lv_btn_create(panel_sidebar);
    lv_obj_set_size(btn_add_city, LV_PCT(80), 50);
    // lv_obj_add_event_cb(btn_add_city, add_city_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* add_city_label = lv_label_create(btn_add_city);
    lv_label_set_text(add_city_label, "Add City");
    lv_obj_center(add_city_label);

    // Button remove city
    lv_obj_t* btn_remove_city = lv_btn_create(panel_sidebar);
    lv_obj_set_size(btn_remove_city, LV_PCT(80), 50);
    //lv_obj_add_event_cb(btn_remove_city, remove_city_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* remove_city_label = lv_label_create(btn_remove_city);
    lv_label_set_text(remove_city_label, "Remove City");
    lv_obj_center(remove_city_label);

    // Spacer that expands to fill available space and pushes the back button to bottom
    lv_obj_t* spacer = lv_obj_create(panel_sidebar);
    // lv_obj_set_size(spacer, LV_PCT(10), 0);
    lv_obj_set_style_border_opa(spacer, LV_OPA_TRANSP, 0);  // Add this line to remove border
    lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_grow(spacer, 1); // expand to take remaining space

    lv_obj_t* btn_back = lv_btn_create(panel_sidebar);
    lv_obj_set_size(btn_back, LV_PCT(80), 50);
    lv_obj_add_event_cb(btn_back, go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* back_label = lv_label_create(btn_back);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    lv_obj_t* list_cities = lv_list_create(panel_main);
    lv_obj_set_size(list_cities, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_opa(list_cities, LV_OPA_TRANSP, 0);  // Add this line to remove border
    lv_obj_set_style_bg_opa(list_cities, LV_OPA_TRANSP, 0);
}

void create_weather_overview_screen()
{
    weather_overview_screen = lv_obj_create(NULL);

    // Set flex layout for the main container
    lv_obj_set_flex_flow(weather_overview_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(weather_overview_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(weather_overview_screen, 20, 0);
    lv_obj_set_style_pad_gap(weather_overview_screen, 20, 0);
    
    // Title
    lv_obj_t* title = lv_label_create(weather_overview_screen);
    lv_label_set_text(title, "Weather Overview");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);  // Use larger built-in font

    // Button container with consistent sizing
    lv_obj_t* btn_container = lv_obj_create(weather_overview_screen);
    lv_obj_set_size(btn_container, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, 15, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, 0);

    // Create a simple textarea with better sizing and styling
    lv_obj_t* city_name_input = lv_textarea_create(btn_container);
    lv_obj_set_size(city_name_input, 400, 60);  // Make it wider and taller
    lv_textarea_set_placeholder_text(city_name_input, "Click here to type city name");
    lv_textarea_set_one_line(city_name_input, true);

    // Add event callbacks for both click and value change
    lv_obj_add_event_cb(city_name_input, show_virtual_keyboard_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(city_name_input, textarea_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Search button
    lv_obj_t* search_btn = lv_btn_create(btn_container);
    lv_obj_set_size(search_btn, 200, 50);
    lv_obj_add_event_cb(search_btn, process_weather_search_cb, LV_EVENT_CLICKED, city_name_input);

    lv_obj_t* search_label = lv_label_create(search_btn);
    lv_label_set_text(search_label, "Search");
    lv_obj_center(search_label);

    // Back button
    lv_obj_t* back_btn = lv_btn_create(
    btn_container);
    lv_obj_set_size(back_btn, 200, 50);
    lv_obj_add_event_cb(back_btn, go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    // Create virtual keyboard (initially hidden)
    virtual_keyboard = lv_keyboard_create(weather_overview_screen);
    lv_obj_set_size(virtual_keyboard, LV_PCT(95), LV_PCT(40));
    lv_obj_align(virtual_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);  // ADD THIS LINE
    lv_obj_add_flag(virtual_keyboard, LV_OBJ_FLAG_HIDDEN);
    
    // Add event to hide keyboard when OK is pressed
    lv_obj_add_event_cb(virtual_keyboard, hide_virtual_keyboard_cb, LV_EVENT_READY, NULL);
}

void create_settings_screen()
{
    settings_screen = lv_obj_create(NULL);
    
    // Set flex layout for the main container
    lv_obj_set_flex_flow(settings_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(settings_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(settings_screen, 20, 0);
    lv_obj_set_style_pad_gap(settings_screen, 20, 0);
    
    // Title
    lv_obj_t* title = lv_label_create(settings_screen);
    lv_label_set_text(title, "Settings");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);  // Use larger built-in font

    // Button container with consistent sizing
    lv_obj_t* btn_container = lv_obj_create(settings_screen);
    lv_obj_set_size(btn_container, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, 15, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, 0);

    lv_obj_t* theme_btn = lv_btn_create(btn_container);
    lv_obj_set_size(theme_btn, 200, 50);
    lv_obj_add_event_cb(theme_btn, switch_theme_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* theme_label = lv_label_create(theme_btn);
    lv_label_set_text(theme_label, "Theme");
    lv_obj_center(theme_label);

    // Back button
    lv_obj_t* back_btn = lv_btn_create(btn_container);
    lv_obj_set_size(back_btn, 200, 50);
    lv_obj_add_event_cb(back_btn, go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
}

//======== Callback Functions ========//
void show_virtual_keyboard_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t* textarea = lv_event_get_target(e);
        
        // Show the virtual keyboard
        lv_obj_remove_flag(virtual_keyboard, LV_OBJ_FLAG_HIDDEN);
        
        // Connect keyboard to textarea
        lv_keyboard_set_textarea(virtual_keyboard, textarea);
    }
}

void hide_virtual_keyboard_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_READY) {
        // Hide the virtual keyboard
        lv_obj_add_flag(virtual_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

void textarea_value_changed_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t* textarea = lv_event_get_target(e);
        
        // // Force the textarea to redraw/refresh
        // lv_obj_invalidate(textarea);
        // lv_obj_refresh_style(textarea, LV_PART_MAIN, LV_STYLE_PROP_ANY);
        // lv_obj_mark_layout_as_dirty(textarea);

        // lv_refr_now(lv_obj_get_display(textarea));

        printf("Textarea value changed: %s\n", lv_textarea_get_text(textarea));
    }
}

void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf)
{
    /* Show the rendered image on the display */
    //my_display_update(area, px_buf);
    lv_obj_invalidate(g_display);

    /* Indicate that the buffer is available.
     * If DMA were used, call in the DMA complete interrupt. */
    lv_display_flush_ready(g_display);
}

//======== Main Function ========//
int main()
{
    // Initialize LVGL
    lv_init();

    hal_init(1024, 600);

    //lv_tick_set_cb(SDL_GetTicks64());

    //lv_display_t * display = lv_display_create(320, 240);

    /* LVGL will render to this 1/10 screen sized buffer for 2 bytes/pixel */
    //static uint8_t buf[320 * 240 / 10 * 2];
    //lv_display_set_buffers(display, buf, NULL, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* This callback will display the rendered image */
    //lv_display_set_flush_cb(g_display, my_flush_cb);

    // Create display
    // g_display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    // if (!g_display) {
    //     printf("ERROR: Failed to create LVGL SDL display!\n");
    //     return -1;
    // }

    // // Create input device
    // lv_indev_t * mouse_indev = lv_sdl_mouse_create();
    // if (!mouse_indev) {
    //     printf("ERROR: Failed to create LVGL SDL mouse!\n");
    //     return -1;
    // }

    // lv_indev_t* keyboard_indev = lv_sdl_keyboard_create();
    // if (!keyboard_indev) {
    //     printf("ERROR: Failed to create LVGL SDL keyboard!\n");
    //     return -1;
    // }
    
    // Create all screens
    // create_main_screen();
    // create_city_management_screen();
    // create_weather_overview_screen();
    // create_settings_screen();
    
    // Load the main screen
    // lv_screen_load(main_screen);
    
    // Main loop
    while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        usleep(5 * 1000);
    }

    return 0;
}

static lv_display_t * hal_init(int32_t w, int32_t h)
{
    lv_group_set_default(lv_group_create());

    lv_display_t * disp = lv_sdl_window_create(w, h);

    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);
    lv_display_set_default(disp);

    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, disp);
    lv_indev_set_group(mousewheel, lv_group_get_default());

    lv_indev_t * kb = lv_sdl_keyboard_create();
    lv_indev_set_display(kb, disp);
    lv_indev_set_group(kb, lv_group_get_default());

    create_main_screen();
    create_city_management_screen();
    create_weather_overview_screen();
    create_settings_screen();
    lv_screen_load(main_screen);

  return disp;
}