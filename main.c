#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"

#include "libs/cities.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 600

// Global display object for theme switching
static lv_display_t* g_display;

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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);

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
    lv_obj_align(settings_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(settings_btn, (lv_event_cb_t)go_to_city_management_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* settings_label = lv_label_create(settings_btn);
    lv_label_set_text(settings_label, "City Management");
    lv_obj_center(settings_label);

    // Weather Overview button
    lv_obj_t* weather_btn = lv_btn_create(btn_container);
    lv_obj_set_size(weather_btn, 200, 50);
    lv_obj_align(weather_btn, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(weather_btn, (lv_event_cb_t)go_to_weather_overview_screen_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* weather_label = lv_label_create(weather_btn);
    lv_label_set_text(weather_label, "Weather Overview");
    lv_obj_center(weather_label);

    // Settings button
    lv_obj_t* config_btn = lv_btn_create(btn_container);
    lv_obj_set_size(config_btn, 200, 50);
    lv_obj_align(config_btn, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_add_event_cb(config_btn, (lv_event_cb_t)go_to_settings_screen_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* config_label = lv_label_create(config_btn);
    lv_label_set_text(config_label, "Settings");
    lv_obj_center(config_label);
}

void create_city_management_screen()
{
    city_management_screen = lv_obj_create(NULL);

    // Set flex layout for the main container
    lv_obj_set_flex_flow(city_management_screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(city_management_screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(city_management_screen, 20, 0);
    lv_obj_set_style_pad_gap(city_management_screen, 20, 0);
    
    // Title
    lv_obj_t* title = lv_label_create(city_management_screen);
    lv_label_set_text(title, "City Management");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);  // Use larger built-in font
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);

    // Button container with consistent sizing
    lv_obj_t* btn_container = lv_obj_create(city_management_screen);
    lv_obj_set_size(btn_container, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, 15, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, 0);

    // Load Default Cities button
    lv_obj_t* load_default_cities_btn = lv_btn_create(btn_container);
    lv_obj_set_size(load_default_cities_btn, 200, 50);
    lv_obj_align(load_default_cities_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(load_default_cities_btn, (lv_event_cb_t)load_default_cities_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* load_default_cities_label = lv_label_create(load_default_cities_btn);
    lv_label_set_text(load_default_cities_label, "Load Default Cities");
    lv_obj_center(load_default_cities_label);

    // Back button
    lv_obj_t* back_btn = lv_btn_create(btn_container);
    lv_obj_set_size(back_btn, 200, 50);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(back_btn, (lv_event_cb_t)go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
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
    lv_label_set_text(title, "City Management");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);  // Use larger built-in font
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);

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
    lv_obj_align(city_name_input, LV_ALIGN_CENTER, 0, -100);
    lv_textarea_set_placeholder_text(city_name_input, "Click here to type city name");
    lv_textarea_set_one_line(city_name_input, true);

    // Add event callbacks for both click and value change
    lv_obj_add_event_cb(city_name_input, show_virtual_keyboard_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(city_name_input, textarea_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Search button
    lv_obj_t* search_btn = lv_btn_create(btn_container);
    lv_obj_set_size(search_btn, 200, 50);
    lv_obj_align(search_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(search_btn, process_weather_search_cb, LV_EVENT_CLICKED, city_name_input);

    lv_obj_t* search_label = lv_label_create(search_btn);
    lv_label_set_text(search_label, "Search");
    lv_obj_center(search_label);

    // Back button
    lv_obj_t* back_btn = lv_btn_create(btn_container);
    lv_obj_set_size(back_btn, 200, 50);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(back_btn, (lv_event_cb_t)go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    // Create virtual keyboard (initially hidden)
    virtual_keyboard = lv_keyboard_create(weather_overview_screen);
    lv_obj_set_size(virtual_keyboard, LV_PCT(95), LV_PCT(40));
    lv_obj_align(virtual_keyboard, LV_ALIGN_BOTTOM_MID, 0, -5);
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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);

    // Button container with consistent sizing
    lv_obj_t* btn_container = lv_obj_create(settings_screen);
    lv_obj_set_size(btn_container, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, 15, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_container, LV_OPA_TRANSP, 0);

    // Back button
    lv_obj_t* back_btn = lv_btn_create(btn_container);
    lv_obj_set_size(back_btn, 200, 50);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(back_btn, (lv_event_cb_t)go_to_main_screen_cb, LV_EVENT_CLICKED, NULL);
    
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
        lv_obj_clear_flag(virtual_keyboard, LV_OBJ_FLAG_HIDDEN);
        
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
        
        // Force the textarea to redraw/refresh
        lv_obj_invalidate(textarea);
        lv_obj_refresh_style(textarea, LV_PART_MAIN, LV_STYLE_PROP_ANY);
        
        printf("Textarea value changed: %s\n", lv_textarea_get_text(textarea));
    }
}

//======== Main Function ========//
int main()
{
    // Initialize LVGL
    lv_init();

    // Create display
    g_display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!g_display) {
        printf("ERROR: Failed to create LVGL SDL display!\n");
        return -1;
    }

    // Create input device
    lv_indev_t * mouse_indev = lv_sdl_mouse_create();
    if (!mouse_indev) {
        printf("ERROR: Failed to create LVGL SDL mouse!\n");
        return -1;
    }

    lv_indev_t* keyboard_indev = lv_sdl_keyboard_create();
    if (!keyboard_indev) {
        printf("ERROR: Failed to create LVGL SDL keyboard!\n");
        return -1;
    }
    
    // Create all screens
    create_main_screen();
    create_city_management_screen();
    create_weather_overview_screen();
    create_settings_screen();
    
    // Load the main screen
    lv_screen_load(main_screen);
    
    // Main loop
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next > 0 ? time_till_next : 5);
    }

    return 0;
}