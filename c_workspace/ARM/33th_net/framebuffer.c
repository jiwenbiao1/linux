/*
 * FILE: framebuffer.c
 * ʵ����framebuffer�ϻ��㡢���ߡ���ͬ��Բ�������ĺ���
 */

#include <types.h>
#include "framebuffer.h"

extern unsigned int fb_base_addr;
extern unsigned int bpp;
extern unsigned int xsize;
extern unsigned int ysize;

static int lcd_x = 0;
static int lcd_y = 0;

//typedef UINT32 FB_24BPP[272][480];

/* 
 * ����
 * ���������
 *     x��y : ��������
 *     color: ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void PutPixel(UINT32 x, UINT32 y, UINT32 color)
{
    UINT8 red,green,blue;

    switch (bpp){
        case 24:
        {
            UINT32 *addr = (UINT32 *)fb_base_addr + (y * xsize + x);
            *addr =  color;
            break;
        }

        case 16:
        {
            UINT16 *addr = (UINT16 *)fb_base_addr + (y * xsize + x);
            red   = (color >> 19) & 0x1f;
            green = (color >> 10) & 0x3f;
            blue  = (color >>  3) & 0x1f;
            color = (red << 11) | (green << 5) | blue; // ��ʽ5:6:5
            *addr = (UINT16) color;
            break;
        }
        
        case 8:
        {
            UINT8 *addr = (UINT8 *)fb_base_addr + (y * xsize + x);
            *addr = (UINT8) color;  /* ��ɫ�������ֵ */
            break;
        }

        default:
            break;
    }
}

/* 
 * ����
 * ���������
 *     x1��y1 : �������
 *     x2��y2 : �յ�����
 *     color  : ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void DrawLine(int x1,int y1,int x2,int y2,int color)
{
    int dx,dy,e;
    dx=x2-x1; 
    dy=y2-y1;
    
    if(dx>=0)
    {
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 1/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 2/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 8/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 7/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
    else //dx<0
    {
        dx=-dx;     //dx=abs(dx)
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 4/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 3/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 5/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 6/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
}

/* 
 * ����ͬ��Բ
 */
void Mire(void)
{
    UINT32 x,y;
    UINT32 color;
    UINT8 red,green,blue,alpha;

    for (y = 0; y < ysize; y++)
        for (x = 0; x < xsize; x++){
            color = ((x-xsize/2)*(x-xsize/2) + (y-ysize/2)*(y-ysize/2))/64;
            red   = (color/8) % 256;
            green = (color/4) % 256;
            blue  = (color/2) % 256;
            alpha = (color*2) % 256;

            color |= ((UINT32)alpha << 24);
            color |= ((UINT32)red   << 16);
            color |= ((UINT32)green << 8 );
            color |= ((UINT32)blue       );

            PutPixel(x,y,color);
        }
}

/* 
 * ����Ļ��ɵ�ɫ
 * ���������
 *     color: ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void ClearScr(UINT32 color)
{   
    UINT32 x,y;
    
    for (y = 0; y < ysize; y++)
        for (x = 0; x < xsize; x++)
            PutPixel(x, y, color);
}

#define FONTDATAMAX 2048

extern const unsigned char fontdata_8x8[FONTDATAMAX];

void lcd_putc(unsigned char c)
{
	int i,j;
	unsigned char line_dots;

	/* �����ģ */
	unsigned char *char_dots = fontdata_8x8 + c * 8;	

	cursor_timer_stop();
	hide_cursor();

	/* ��framebuffer����� */
	if (c == '\n')
	{
		//y = (y + 8) % 272;
		lcd_y += 8;
		if (lcd_y >= 272)
		{
			lcd_y = 272 - 8;
			memmove(fb_base_addr, fb_base_addr+(8*480), (272-8)*480);
			memset(fb_base_addr+(272-8)*480, 0, 8*480);
		}
		goto exit ;
	}
	else if (c == '\r')
	{
		lcd_x = 0;
		goto exit;
	}

	for (i = 0; i < 8; i++)	
	{
		line_dots = char_dots[i];
		
		for (j = 0; j < 8; j++)
		{
			if (line_dots & (0x80 >> j))
			{
				PutPixel(lcd_x+j,  lcd_y+i, 1);  /* ��ɫ�������1����Ŀ����ɫֵ */
			}
			else
			{
				PutPixel(lcd_x+j,  lcd_y+i, 0);  /* ��ɫ�������0����Ŀ����ɫֵ */
			}
		}
	}

	//x = (x + 8) % 480;
	lcd_x += 8;
	if (lcd_x >= 480)
		lcd_x = 0;
	
	if (lcd_x == 0)
	{
		//y = (y + 8) % 272;
		lcd_y += 8;
		if (lcd_y >= 272)
		{
			lcd_y = 272 - 8;
			memmove(fb_base_addr, fb_base_addr+(8*480), (272-8)*480);
			memset(fb_base_addr+(272-8)*480, 0, 8*480);
		}
	}

exit:
	cursor_timer_start();
	show_cursor();	
}

void show_cross(int x, int y)
{
	DrawLine(x-5, y, x+5, y, 4);
	DrawLine(x, y-5, x, y+5, 4);
}

void hide_cross(int x, int y)
{
	DrawLine(x-5, y, x+5, y, 0);
	DrawLine(x, y-5, x, y+5, 0);
}

void hide_cursor(void)
{
	DrawLine(lcd_x, lcd_y, lcd_x, lcd_y+8, 0);
	DrawLine(lcd_x+1, lcd_y, lcd_x+1, lcd_y+8, 0);
}

void show_cursor(void)
{
	DrawLine(lcd_x, lcd_y, lcd_x, lcd_y+8, 4);
	DrawLine(lcd_x+1, lcd_y, lcd_x+1, lcd_y+8, 4);
}

void show_hide_cursor(void)
{
	static int has_cursor = 0;
	if (has_cursor)
	{
		has_cursor = 0;
		hide_cursor();
	}
	else
	{
		has_cursor = 1;
		show_cursor();
	}
}
