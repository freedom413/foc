#ifndef __LCD_FSMC_H__
#define __LCD_FSMC_H__

#include <stdint.h>

/* Forward declaration of struct (actual definition is in lcd_fsmc.c) */
#define TFTLCD_BASE     ((uint32_t)(0x6C000000 | 0x0000007E))

typedef struct lcd_fsmc_t{
    volatile uint16_t cmd;
    volatile uint16_t data;
}lcd_fsmc_t;

typedef struct lcd_size_t{
    uint16_t    width;     
    uint16_t    height;
}lcd_size_t;

typedef struct tftlcd_t{                                         
    uint8_t         dir;       
    uint16_t        id;
    lcd_size_t      size;
    lcd_size_t      window;
    lcd_fsmc_t     *p_fsmc_bus;
}tftlcd_t;

extern tftlcd_t htftlcd1;

/* Attach FSMC base address and size to a tftlcd instance */
void tftlcd_attach_bus(struct tftlcd_t *p, uint32_t base, uint16_t width, uint16_t height);

/* Initialize the display controller (calls the low-level init sequence) */
int tftlcd_init_driver(struct tftlcd_t *p_tftlcd);

/* Fast rectangle fill with a single RGB565 color */
void lcd_fill_color_fast(struct tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t color);

/* Fill a rectangle with a supplied pixel buffer (RGB565, row-major) */
void lcd_fill_buffer(struct tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, const uint16_t *buf);

/* A small LVGL-compatible flush wrapper (adapt as needed to LVGL9) */
/* If LVGL is available, use real LVGL types. Otherwise provide minimal fallbacks
 * so this header can be included without LVGL installed. */
#ifdef LVGL_VERSION_MAJOR
#include "lvgl.h"
typedef lv_disp_t ili_lv_disp_t;
typedef lv_area_t ili_lv_area_t;
typedef lv_color_t ili_lv_color_t;
#else
typedef struct { int32_t x1; int32_t y1; int32_t x2; int32_t y2; } ili_lv_area_t;
typedef struct { uint16_t full; } ili_lv_color_t; /* RGB565 fallback */
typedef void ili_lv_disp_t;
#endif

/* LVGL9-compatible flush callback. If LVGL is present, this will call
 * lv_disp_flush_ready() when done. */
void ili9488_lvgl_flush_cb(ili_lv_disp_t *disp, const ili_lv_area_t *area, const ili_lv_color_t *color_map);

#endif