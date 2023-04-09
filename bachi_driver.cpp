

#include "bachi_driver.h"
#include "pin_config.h"

#include "Arduino.h"

#include "lvgl.h"      /* https://github.com/lvgl/lvgl.git */

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions
static lv_color_t *lv_disp_buf;
static bool is_initialized_lvgl = false;


/* Styles */
static lv_style_t   style_label_button;
static lv_style_t   style_button_press;
static lv_style_t   style_button_toggle;
static lv_style_t   style_label;
static lv_style_t   style_label_request;
static lv_style_t   style_label_subscript;
static lv_style_t   style_label_superscript;
static lv_style_t   style_label_superscript_request;

/*--- SCREENS ---*/
static lv_obj_t    *screen_splash;
static lv_obj_t    *screen_app;

/*--- SPLASH ---*/
static lv_obj_t    *label_button_splash;
static lv_obj_t    *label_splash;

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

void my_log_cb(const char * buf) {
  //serial_send(buf, strlen(buf));
  Serial.print(buf);
}

void bachi_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

/*
void
style_create()
{
    lv_style_copy(&style_label_button, &lv_style_transp);
    style_label_button.text.color = lv_color_hex(0xffffff);
    style_label_button.text.sel_color = lv_color_hex(0x5596d8);
    style_label_button.text.font = &lv_font_roboto_28;
    style_label_button.text.letter_space = 0;
    style_label_button.text.line_space = 2;
    style_label_button.text.opa = 255;

    lv_style_copy(&style_button_press, &lv_style_btn_pr);
    style_button_press.body.main_color = lv_color_hex(0xff0000);
    style_button_press.body.grad_color = lv_color_hex(0x770000);

    lv_style_copy(&style_button_toggle, &lv_style_btn_tgl_pr);
    style_button_toggle.body.main_color = lv_color_hex(0xee0000);
    style_button_toggle.body.grad_color = lv_color_hex(0x660000);

    lv_style_copy(&style_label, &lv_style_transp);
    style_label.text.color = lv_color_hex(0x303030);
    style_label.text.sel_color = lv_color_hex(0x5596d8);
    style_label.text.font = &lv_font_roboto_28;
    style_label.text.letter_space = 0;
    style_label.text.line_space = 2;
    style_label.text.opa = 255;

    lv_style_copy(&style_label_request, &style_label);
    style_label_request.text.color = lv_color_hex(0xff0000);

    lv_style_copy(&style_label_subscript, &lv_style_transp);
    style_label_subscript.text.font = &lv_font_roboto_22;

    lv_style_copy(&style_label_superscript, &lv_style_transp);
    style_label_superscript.text.font = &lv_font_roboto_12;

    lv_style_copy(&style_label_superscript_request, &style_label_superscript);
    style_label_superscript_request.text.color = lv_color_hex(0xff0000);
}
*/

lv_obj_t *
splash_screen_create()
{
    screen_splash = lv_obj_create(NULL);
    return screen_splash;
}

void
splash_create(lv_obj_t *parent)
{

    /* Label (Button) */
    label_button_splash = lv_label_create(parent);
    lv_obj_align(label_splash, LV_ALIGN_CENTER, 50, 50);
    lv_obj_set_style_text_align(label_splash, LV_TEXT_ALIGN_CENTER, 0); // LV_PART_MAIN
    lv_label_set_text(label_button_splash, "Start");

    /* Label */
    //label_splash = lv_label_create(parent);
    //lv_obj_align(label_splash, NULL, LV_ALIGN_CENTER, 0, 0);
    //lv_obj_set_style_text_align(label_splash, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    //lv_label_set_text(label_splash, "");
    
}

void
splash_show()
{
    lv_scr_load(screen_splash);
}

void
bachi_setup_display()
{
    Serial.print("2");
    pinMode(PIN_LCD_RD, OUTPUT);
    digitalWrite(PIN_LCD_RD, HIGH);
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = PIN_LCD_DC,
        .wr_gpio_num = PIN_LCD_WR,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums =
        {
            PIN_LCD_D0,
            PIN_LCD_D1,
            PIN_LCD_D2,
            PIN_LCD_D3,
            PIN_LCD_D4,
            PIN_LCD_D5,
            PIN_LCD_D6,
            PIN_LCD_D7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
    };
    esp_lcd_new_i80_bus(&bus_config, &i80_bus);

    Serial.print("3");
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = PIN_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 20,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_levels =
        {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_LCD_RES,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);

    esp_lcd_panel_invert_color(panel_handle, true);

    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can
    // have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 35);
#if defined(LCD_MODULE_CMD_1)
    Serial.print("4");
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].cmd, lcd_st7789v[i].data, lcd_st7789v[i].len & 0x7f);
        if (lcd_st7789v[i].len & 0x80)
            delay(120);
    }
#endif
    Serial.print("5");
    /* Lighten the screen with gradient */
    ledcSetup(0, 10000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    for (uint8_t i = 0; i < 0xFF; i++) {
        ledcWrite(0, i);
        delay(2);
    }

    Serial.print("6");
    lv_init();
    lv_disp_buf = (lv_color_t *)heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    lv_log_register_print_cb(my_log_cb);

    Serial.print("7");
    lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
    /*Initialize the display*/
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle; // from esp_lcd_panel_handle_t panel_handle = NULL;
    disp_drv.rotated = LV_DISP_ROT_NONE;
    lv_disp_drv_register(&disp_drv);

    lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_180);


    Serial.print("8");

    //lv_obj_t *screen_splash = splash_screen_create();
    //lv_obj_t *screen_app = app_screen_create();


    //style_create();
    //splash_create(screen_splash);
    //app_create(screen_app);
    
    //splash_show();

    bachi_example_get_started_1();

    Serial.print("9");
}

void
bachi_loop_display()
{
    lv_timer_handler();
    delay(3);
    static uint32_t last_tick;
    if (millis() - last_tick > 100) {
        last_tick = millis();
    }
}

static bool
example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    if (is_initialized_lvgl) {
        lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
        lv_disp_flush_ready(disp_driver);
    }
    return false;
}

static void
example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}


