/*

  u8g_ll_api.c
  
  low level api

  Universal 8bit Graphics Library
  
  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  

*/

#include <stddef.h>
#include "u8g.h"

uint8_t u8g_call_dev_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{
  return dev->dev_fn(u8g, dev, msg, arg); 
}

/*====================================================================*/

uint8_t u8g_InitLL(u8g_t *u8g, u8g_dev_t *dev)
{
  uint8_t r;
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  r =  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_INIT, NULL);
  u8g->state_cb(U8G_STATE_MSG_BACKUP_U8G);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
  return r;
}

void u8g_FirstPageLL(u8g_t *u8g, u8g_dev_t *dev)
{  
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_U8G);
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_PAGE_FIRST, NULL);
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
}

uint8_t u8g_NextPageLL(u8g_t *u8g, u8g_dev_t *dev)
{  
  uint8_t r;
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_U8G);
  r = u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_PAGE_NEXT, NULL);
  if ( r != 0 )
  {
    u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
  }
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
  return r;
}

uint8_t u8g_SetContrastLL(u8g_t *u8g, u8g_dev_t *dev, uint8_t contrast)
{  
  return u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_CONTRAST, &contrast);
}

void u8g_DrawPixelLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y)
{
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_SET_PIXEL, arg);
}

void u8g_Draw8PixelLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel)
{
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = dir;
  arg->pixel = pixel;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_SET_8PIXEL, arg);
}

#ifdef U8G_DEV_MSG_IS_BBX_INTERSECTION
uint8_t u8g_IsBBXIntersectionLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)
{  
  return u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_IS_BBX_INTERSECTION, &arg);
}
#endif



u8g_uint_t u8g_GetWidthLL(u8g_t *u8g, u8g_dev_t *dev)
{
  u8g_uint_t r;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_WIDTH, &r);
  return r;
}

u8g_uint_t u8g_GetHeightLL(u8g_t *u8g, u8g_dev_t *dev)
{       
  u8g_uint_t r;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_HEIGHT, &r);
  return r;
}

u8g_uint_t u8g_GetModeLL(u8g_t *u8g, u8g_dev_t *dev)
{       
  return u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_MODE, NULL);
}



/*====================================================================*/

void u8g_UpdateDimension(u8g_t *u8g)
{
  u8g->width = u8g_GetWidthLL(u8g, u8g->dev);
  u8g->height = u8g_GetHeightLL(u8g, u8g->dev);
  u8g->mode = u8g_GetModeLL(u8g, u8g->dev);
  /* 9 Dec 2012: u8g_scale.c requires update of current page */
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
}

static void u8g_init_data(u8g_t *u8g)
{
  uint8_t i;
  u8g->font = NULL;
  u8g->cursor_font = NULL;
  u8g->cursor_bg_color = 0;
  u8g->cursor_fg_color = 1;
  u8g->cursor_encoding = 34;
  u8g->cursor_fn = (u8g_draw_cursor_fn)0;
  
  for( i = 0; i < U8G_PIN_LIST_LEN; i++ )
    u8g->pin_list[i] = U8G_PIN_NONE;

  u8g_SetColorIndex(u8g, 1);

  u8g_SetFontPosBaseline(u8g);
  
  u8g->font_height_mode = U8G_FONT_HEIGHT_MODE_XTEXT;
  u8g->font_ref_ascent = 0;
  u8g->font_ref_descent = 0;
  u8g->font_line_spacing_factor = 64;           /* 64 = 1.0, 77 = 1.2 line spacing factor */
  u8g->line_spacing = 0;
  
  u8g->state_cb = u8g_state_dummy_cb;
}

uint8_t u8g_Init(u8g_t *u8g, u8g_dev_t *dev)
{
  u8g_init_data(u8g);
  u8g->dev = dev;
  
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;
  u8g_UpdateDimension(u8g);
  return 1;
}

uint8_t u8g_InitSPI(u8g_t *u8g, u8g_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
    
  /* assign user pins */
  u8g->pin_list[U8G_PI_SCK] = sck;
  u8g->pin_list[U8G_PI_MOSI] = mosi;
  u8g->pin_list[U8G_PI_CS] = cs;
  u8g->pin_list[U8G_PI_A0] = a0;
  u8g->pin_list[U8G_PI_RESET] = reset;
  
  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;

  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;
}

uint8_t u8g_InitHWSPI(u8g_t *u8g, u8g_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset)
{
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
  
  
  /* assign user pins */
  u8g->pin_list[U8G_PI_CS] = cs;
  u8g->pin_list[U8G_PI_A0] = a0;
  u8g->pin_list[U8G_PI_RESET] = reset;
  
  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;

  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;
}

uint8_t u8g_InitI2C(u8g_t *u8g, u8g_dev_t *dev, uint8_t options)
{
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
    
  u8g->pin_list[U8G_PI_I2C_OPTION] = options;
  
  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;

  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;
}


uint8_t u8g_Init8BitFixedPort(u8g_t *u8g, u8g_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
    
  /* assign user pins */

  u8g->pin_list[U8G_PI_EN] = en;
  u8g->pin_list[U8G_PI_CS] = cs;
  u8g->pin_list[U8G_PI_DI] = di;
  u8g->pin_list[U8G_PI_RW] = rw;
  u8g->pin_list[U8G_PI_RESET] = reset;

  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;

  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;

}

uint8_t u8g_Init8Bit(u8g_t *u8g, u8g_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, 
  uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
    
  /* assign user pins */

  u8g->pin_list[U8G_PI_D0] = d0;
  u8g->pin_list[U8G_PI_D1] = d1;
  u8g->pin_list[U8G_PI_D2] = d2;
  u8g->pin_list[U8G_PI_D3] = d3;
  u8g->pin_list[U8G_PI_D4] = d4;
  u8g->pin_list[U8G_PI_D5] = d5;
  u8g->pin_list[U8G_PI_D6] = d6;
  u8g->pin_list[U8G_PI_D7] = d7;

  u8g->pin_list[U8G_PI_EN] = en;
  u8g->pin_list[U8G_PI_CS1] = cs1;
  u8g->pin_list[U8G_PI_CS2] = cs2;
  u8g->pin_list[U8G_PI_DI] = di;
  u8g->pin_list[U8G_PI_RW] = rw;
  u8g->pin_list[U8G_PI_RESET] = reset;
  
  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;

  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;
}

void u8g_FirstPage(u8g_t *u8g)
{
  u8g_FirstPageLL(u8g, u8g->dev);
}

uint8_t u8g_NextPage(u8g_t *u8g)
{
  if  ( u8g->cursor_fn != (u8g_draw_cursor_fn)0 )
  {
    u8g->cursor_fn(u8g);
  }
  return u8g_NextPageLL(u8g, u8g->dev);
}

uint8_t u8g_SetContrast(u8g_t *u8g, uint8_t contrast)
{
  return u8g_SetContrastLL(u8g, u8g->dev, contrast);
}


void u8g_DrawPixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y)
{
  u8g_DrawPixelLL(u8g, u8g->dev, x, y);
}

void u8g_Draw8Pixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel)
{
  u8g_Draw8PixelLL(u8g, u8g->dev, x, y, dir, pixel);
}

/* u8g_IsBBXIntersection() has been moved to u8g_clip.c */
#ifdef OBSOLETE_CODE
uint8_t u8g_IsBBXIntersection(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)
{
  /* new code */
  u8g_dev_arg_bbx_t arg;
  arg.x = x;
  arg.y = y;
  arg.w = w;
  arg.h = h;
  return u8g_is_box_bbx_intersection(&(u8g->current_page), &arg);

  /* old code */
  //return u8g_IsBBXIntersectionLL(u8g, u8g->dev, x, y, w, h);
}
#endif

void u8g_SetColorIndex(u8g_t *u8g, uint8_t idx)
{
  u8g->arg_pixel.color = idx;
  /*u8g->color_index = idx; */ /* must be removed */
}

uint8_t u8g_GetColorIndex(u8g_t *u8g)
{
  return u8g->arg_pixel.color;
}

uint8_t u8g_GetDefaultForegroundColor(u8g_t *u8g)
{
  uint8_t mode;
  mode = u8g_GetMode(u8g);
  if ( mode == U8G_MODE_R3G3B2 ) 
    return 255;     /* white */
  else if ( u8g_GetMode(u8g) == U8G_MODE_GRAY2BIT )
    return 3;         /* max intensity */
  else /* if ( u8g.getMode() == U8G_MODE_BW ) */
    return 1;         /* pixel on */
  return 1;
}

void u8g_SetDefaultForegroundColor(u8g_t *u8g)
{
  u8g_SetColorIndex(u8g, u8g_GetDefaultForegroundColor(u8g));
}

uint8_t u8g_GetDefaultBackgroundColor(u8g_t *u8g)
{
  return 0;
}

void u8g_SetDefaultBackgroundColor(u8g_t *u8g)
{
  u8g_SetColorIndex(u8g, u8g_GetDefaultBackgroundColor(u8g));         /* pixel on / black */
}

uint8_t u8g_GetDefaultMidColor(u8g_t *u8g)
{
  uint8_t mode;
  mode = u8g_GetMode(u8g);
  if ( mode == U8G_MODE_R3G3B2 ) 
    return 0x06d;     /* gray: 01101101 */
  else if ( u8g_GetMode(u8g) == U8G_MODE_GRAY2BIT )
    return 1;         /* low mid intensity */
  else /* if ( u8g.getMode() == U8G_MODE_BW ) */
    return 1;         /* pixel on */
  return 1;   /* default */
}

void u8g_SetDefaultMidColor(u8g_t *u8g)
{
  u8g_SetColorIndex(u8g, u8g_GetDefaultMidColor(u8g));
}




