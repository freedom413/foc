#include "User.h"
#include "fsmc.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "lcd_fsmc.h"


tftlcd_t htftlcd1;

static inline void lcd_w_cmd(tftlcd_t *p_tftlcd, uint16_t cmd) {
    p_tftlcd->p_fsmc_bus->cmd = cmd;
}

static inline void lcd_w_data(tftlcd_t *p_tftlcd, uint16_t data) {
    p_tftlcd->p_fsmc_bus->data = data;
}

static inline void lcd_w_cmd_then_data(tftlcd_t *p_tftlcd, uint16_t cmd, uint16_t data) {
    lcd_w_cmd(p_tftlcd, cmd);
    lcd_w_data(p_tftlcd, data);
}

/* Forward declare helper used by fill functions */
static void lcd_windows_set(tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);

static void lcd_windows_set(tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    /* Column address (X) */
    lcd_w_cmd(p_tftlcd, 0x2A);
    lcd_w_data(p_tftlcd, sx >> 8);
    lcd_w_data(p_tftlcd, sx & 0xFF);
    lcd_w_data(p_tftlcd, width >> 8);
    lcd_w_data(p_tftlcd, width & 0xFF);

    /* Page address (Y) */
    lcd_w_cmd(p_tftlcd, 0x2B);
    lcd_w_data(p_tftlcd, sy >> 8);
    lcd_w_data(p_tftlcd, sy & 0xFF);
    lcd_w_data(p_tftlcd, height >> 8);
    lcd_w_data(p_tftlcd, height & 0xFF);

    /* Prepare to write memory - caller should follow with pixel writes */
    lcd_w_cmd(p_tftlcd, 0x2C);
}

static inline void lcd_dir_set(tftlcd_t *p_tftlcd, uint8_t dir)
{
    if (dir == 0) {
        lcd_w_cmd_then_data(p_tftlcd, 0x36, 0x08);
        p_tftlcd->window.width = p_tftlcd->size.width;
        p_tftlcd->window.height = p_tftlcd->size.height;
    } else {
        lcd_w_cmd_then_data(p_tftlcd, 0x36, 0x28);
        lcd_w_cmd(p_tftlcd, 0xB6);
        lcd_w_data(p_tftlcd, 0x00);
        lcd_w_data(p_tftlcd, 0x02);
        lcd_w_data(p_tftlcd, 0x3B);
        p_tftlcd->window.width = p_tftlcd->size.height;
        p_tftlcd->window.height = p_tftlcd->size.width;
    }
    p_tftlcd->dir = dir;
}

void tftlcd_init(tftlcd_t *p_tftlcd)
{
    if (p_tftlcd == NULL || p_tftlcd->p_fsmc_bus == NULL) return;

    /* Software reset */
    lcd_w_cmd(p_tftlcd, 0x01);
    HAL_Delay(10);

    /* Power control settings */
    lcd_w_cmd(p_tftlcd, 0xC0); /* Power Control 1 */
    lcd_w_data(p_tftlcd, 0x18);
    lcd_w_data(p_tftlcd, 0x17);

    lcd_w_cmd(p_tftlcd, 0xC1); /* Power Control 2 */
    lcd_w_data(p_tftlcd, 0x41);

    lcd_w_cmd(p_tftlcd, 0xC5); /* VCOM Control */
    lcd_w_data(p_tftlcd, 0x00);
    lcd_w_data(p_tftlcd, 0x1A);
    lcd_w_data(p_tftlcd, 0x80);

    /* Memory access control (orientation) - default portrait */
    lcd_w_cmd(p_tftlcd, 0x36);
    lcd_w_data(p_tftlcd, 0x08);

    /* Pixel format: 16-bit/pixel (RGB565) */
    lcd_w_cmd(p_tftlcd, 0x3A);
    lcd_w_data(p_tftlcd, 0x55);

    /* Frame rate / Interface mode */
    lcd_w_cmd(p_tftlcd, 0xB1);
    lcd_w_data(p_tftlcd, 0xA0);

    lcd_w_cmd(p_tftlcd, 0xB4);
    lcd_w_data(p_tftlcd, 0x02);

    lcd_w_cmd(p_tftlcd, 0xB6);
    lcd_w_data(p_tftlcd, 0x00);
    lcd_w_data(p_tftlcd, 0x22);
    lcd_w_data(p_tftlcd, 0x3B);

    /* Set Image Function / Adjustments */
    lcd_w_cmd(p_tftlcd, 0xE9);
    lcd_w_data(p_tftlcd, 0x00);

    lcd_w_cmd(p_tftlcd, 0xF7);
    lcd_w_data(p_tftlcd, 0xA9);
    lcd_w_data(p_tftlcd, 0x51);
    lcd_w_data(p_tftlcd, 0x2C);
    lcd_w_data(p_tftlcd, 0x82);

    /* Gamma set (positive/negative) */
    lcd_w_cmd(p_tftlcd, 0xE0);
    lcd_w_data(p_tftlcd, 0x00);
    lcd_w_data(p_tftlcd, 0x13);
    lcd_w_data(p_tftlcd, 0x18);
    lcd_w_data(p_tftlcd, 0x04);
    lcd_w_data(p_tftlcd, 0x0F);
    lcd_w_data(p_tftlcd, 0x06);
    lcd_w_data(p_tftlcd, 0x3A);
    lcd_w_data(p_tftlcd, 0x56);
    lcd_w_data(p_tftlcd, 0x4D);
    lcd_w_data(p_tftlcd, 0x03);
    lcd_w_data(p_tftlcd, 0x0A);
    lcd_w_data(p_tftlcd, 0x06);
    lcd_w_data(p_tftlcd, 0x30);
    lcd_w_data(p_tftlcd, 0x3E);
    lcd_w_data(p_tftlcd, 0x0F);

    lcd_w_cmd(p_tftlcd, 0xE1);
    lcd_w_data(p_tftlcd, 0x00);
    lcd_w_data(p_tftlcd, 0x13);
    lcd_w_data(p_tftlcd, 0x18);
    lcd_w_data(p_tftlcd, 0x01);
    lcd_w_data(p_tftlcd, 0x11);
    lcd_w_data(p_tftlcd, 0x06);
    lcd_w_data(p_tftlcd, 0x38);
    lcd_w_data(p_tftlcd, 0x34);
    lcd_w_data(p_tftlcd, 0x4D);
    lcd_w_data(p_tftlcd, 0x06);
    lcd_w_data(p_tftlcd, 0x0D);
    lcd_w_data(p_tftlcd, 0x0B);
    lcd_w_data(p_tftlcd, 0x31);
    lcd_w_data(p_tftlcd, 0x37);
    lcd_w_data(p_tftlcd, 0x0F);

    /* Sleep Out */
    lcd_w_cmd(p_tftlcd, 0x11);
    HAL_Delay(120);

    /* Do NOT issue 0x2C (Memory Write) or 0x29 (Display ON) here. */
    lcd_dir_set(p_tftlcd, 0);
}

typedef enum {
    step1,
    step2,
    step3,
    step4,
} state_t;

// 转数阈值检测函数
void Check_Revs_Threshold() {

    float revolutions = (float)encoder_total_count / (PULSES_PER_REV * GEAR_RATIO);
    
    static state_t current_state = step1;

    switch (current_state) {
        case step1:
                GPIO_SetBits(GPIOA, GPIO_Pin_6);
                GPIO_SetBits(GPIOA, GPIO_Pin_7);
                current_state = step2;
            break;

        case step2:
            if (revolutions >= -60.0f) {
                // 转数达到条件，设置PA7为低电平
                GPIO_ResetBits(GPIOA, GPIO_Pin_7);
                delay(500);
                GPIO_SetBits(GPIOA, GPIO_Pin_6);
                current_state = step3;
            }
            break;

        case step3:
            // 检测转数是否大于等于-50，回到step1
            if (revolutions >= 0.0f) {
                current_state = step4;
            }
            break;
        case step4:
            //推出
            break;
        default:
            current_state = step1;
            break;
    
    }

}

/* ------------------------------------------------------------------
 * Public helper APIs
 * ------------------------------------------------------------------ */

void tftlcd_attach_bus(tftlcd_t *p, uint32_t base, uint16_t width, uint16_t height)
{
    if (p == NULL) return;
    p->p_fsmc_bus = (lcd_fsmc_t *)base;
    /* default to portrait 320x480 if caller passes 0 */
    if (width == 0) width = 320;
    if (height == 0) height = 480;
    p->size.width = width;
    p->size.height = height;
    p->window = p->size;
    p->dir = 0;
}

int tftlcd_init_driver(tftlcd_t *p_tftlcd)
{
    if (p_tftlcd == NULL || p_tftlcd->p_fsmc_bus == NULL) return -1;
    tftlcd_init(p_tftlcd);
    /* After initialization, default the screen to white to indicate ready state */
    lcd_fill_color_fast(p_tftlcd, 0, 0, p_tftlcd->size.width, p_tftlcd->size.height, 0xFFFF);
    /* Now that the framebuffer has been written while the display was off,
     * enable the display so the image appears without leaving the controller
     * in a half-written state that can produce a color block. */
    lcd_w_cmd(p_tftlcd, 0x29);
    return 0;
}

/* Bulk pixel write: write `count` RGB565 pixels from `pixels` into RAM data port.
 * Use direct volatile 16-bit writes to the data register for performance. */
static inline void lcd_write_pixels_bulk(tftlcd_t *p_tftlcd, const uint16_t *pixels, uint32_t count)
{
    volatile uint16_t *data_port = &p_tftlcd->p_fsmc_bus->data;
    uint32_t i;
    for (i = 0; i < count; ++i) {
        *data_port = pixels[i];
    }
}

/* Fast fill with a single color using 32/64-bit buffer stores where possible.
 * For simplicity and portability we write 16-bit values in a loop which is
 * usually fast enough via FSMC bus. Optimizations can write 32-bit by
 * duplicating the 16-bit color into a 32-bit word if alignment is OK. */
void lcd_fill_color_fast(tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, uint16_t color)
{
    if (p_tftlcd == NULL || p_tftlcd->p_fsmc_bus == NULL) return;
    /* set window */
    lcd_windows_set(p_tftlcd, sx, sy, sx + w - 1, sy + h - 1);
    /* write pixel data */
    volatile uint16_t *data_port = &p_tftlcd->p_fsmc_bus->data;
    uint32_t total = (uint32_t)w * (uint32_t)h;
    uint32_t i;
    for (i = 0; i < total; ++i) {
        *data_port = color;
    }
}

/* Fill rectangle from a provided buffer (row-major, RGB565) */
void lcd_fill_buffer(tftlcd_t *p_tftlcd, uint16_t sx, uint16_t sy, uint16_t w, uint16_t h, const uint16_t *buf)
{
    if (p_tftlcd == NULL || p_tftlcd->p_fsmc_bus == NULL || buf == NULL) return;
    lcd_windows_set(p_tftlcd, sx, sy, sx + w - 1, sy + h - 1);
    lcd_write_pixels_bulk(p_tftlcd, buf, (uint32_t)w * (uint32_t)h);
}

/* Simple LVGL flush callback (concept): adapt arguments to LVGL9 API
 * This stub shows how to call lcd_fill_buffer for the requested area.
 * You must adapt types and call lv_disp_flush_ready when integrated. */
void ili9488_lvgl_flush_cb(ili_lv_disp_t *disp, const ili_lv_area_t *area, const ili_lv_color_t *color_map)
{
    if (area == NULL || color_map == NULL) return;

    uint16_t w = (uint16_t)(area->x2 - area->x1 + 1);
    uint16_t h = (uint16_t)(area->y2 - area->y1 + 1);

    /* Assume LVGL color is RGB565 (uint16_t). If LVGL uses a different
     * color depth, the caller must convert the buffer before calling.
     */
    lcd_fill_buffer(&htftlcd1, (uint16_t)area->x1, (uint16_t)area->y1, w, h, (const uint16_t *)color_map);

#ifdef LVGL_VERSION_MAJOR
    lv_disp_flush_ready((lv_disp_t *)disp);
#else
    (void)disp;
#endif
}

