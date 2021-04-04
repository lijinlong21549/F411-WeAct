/**
  ******************************************************************************
  * @file    USB_Host/HID_Standalone/Src/keyboard.c 
  * @author  MCD Application Team
  * @brief   This file implements the HID keyboard functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KYBRD_FIRST_COLUMN               (uint16_t) 8
#define KYBRD_LAST_COLUMN                (uint16_t) BSP_LCD_GetXSize()
#define KYBRD_FIRST_LINE                 (uint8_t) 70
#define SMALL_FONT_COLUMN_WIDTH                    8
#define SMALL_FONT_LINE_WIDTH                      15
#define KYBRD_LAST_LINE                  (uint16_t)155
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern HID_DEMO_StateMachine hid_demo;
extern uint8_t *DEMO_KEYBOARD_menu[];
extern uint8_t prev_select;
extern uint32_t hid_demo_ready;
uint8_t KeybrdCharYpos = 0;
uint16_t KeybrdCharXpos = 0;
uint16_t CurrentLastXpos[KYBRD_LAST_LINE] = {0};
/* Private function prototypes -----------------------------------------------*/
static void USR_KEYBRD_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Manages Keyboard Menu Process.  
  * @param  None
  * @retval None
  */
void HID_KeyboardMenuProcess(void)
{
  switch(hid_demo.keyboard_state)
  {
  case HID_KEYBOARD_IDLE:
    hid_demo.keyboard_state = HID_KEYBOARD_START;
    HID_SelectItem(DEMO_KEYBOARD_menu, 0);   
    hid_demo.select = 0;
    prev_select = 0;       
    break;
    
  case HID_KEYBOARD_WAIT:
    if(hid_demo.select != prev_select)
    {
      prev_select = hid_demo.select;
      HID_SelectItem(DEMO_KEYBOARD_menu, (hid_demo.select >> 4));
      /* Handle select item */
      switch(hid_demo.select)
      {
      case 0x0F: 
        hid_demo.keyboard_state = HID_KEYBOARD_START;
        hid_demo.select = 0;
        prev_select = 0;
        break;
        
      case 0x1F: /* Return */
        LCD_LOG_ClearTextZone();
        hid_demo.state = HID_DEMO_REENUMERATE;
        hid_demo.select = 0;
        prev_select = 0;
        break;
        
      default:
        break;
      }
    }
    break; 
    
  case HID_KEYBOARD_START:
    USR_KEYBRD_Init();   
    hid_demo.keyboard_state = HID_KEYBOARD_WAIT;
    break;  
    
  default:
    break;
  }
}

/**
  * @brief  Init Keyboard window.     
  * @param  None
  * @retval None
  */
static void USR_KEYBRD_Init(void)
{
  LCD_LOG_ClearTextZone();
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  
  BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"Use Keyboard to type characters:                                                            "); 
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  
  KeybrdCharYpos = KYBRD_FIRST_LINE;
  KeybrdCharXpos = KYBRD_FIRST_COLUMN;
}

/**
  * @brief  Processes Keyboard data.
  * @param  data: Keyboard data to be displayed
  * @retval None
  */
void USR_KEYBRD_ProcessData(uint8_t data)
{
  if(data == '\n')
  {
    KeybrdCharXpos = KYBRD_FIRST_COLUMN;
    
    /* Increment char Y position */
    KeybrdCharYpos += SMALL_FONT_LINE_WIDTH;
    
    if(KeybrdCharYpos > KYBRD_LAST_LINE)
    {
      LCD_LOG_ClearTextZone();
      KeybrdCharYpos = KYBRD_FIRST_LINE;
      KeybrdCharXpos = KYBRD_FIRST_COLUMN;
    }
  }
  else if(data == '\r')
  {
    /* Manage deletion of character and update cursor location */
    if(KeybrdCharXpos == KYBRD_FIRST_COLUMN) 
    {
      /* First character of first line to be deleted */
      if(KeybrdCharYpos == KYBRD_FIRST_LINE)
      {  
        KeybrdCharXpos = KYBRD_FIRST_COLUMN; 
      }
      else
      {
        KeybrdCharYpos -= SMALL_FONT_LINE_WIDTH;
        KeybrdCharXpos = (KYBRD_LAST_COLUMN - SMALL_FONT_COLUMN_WIDTH); 
      }
    }
    else
    {
      if(CurrentLastXpos[KeybrdCharYpos] > KYBRD_FIRST_COLUMN)
      {
        CurrentLastXpos[KeybrdCharYpos] -= SMALL_FONT_COLUMN_WIDTH;
        KeybrdCharXpos = CurrentLastXpos[KeybrdCharYpos];
      }
      else if(KeybrdCharYpos > KYBRD_FIRST_LINE)
      {
        KeybrdCharYpos -= SMALL_FONT_LINE_WIDTH;
        CurrentLastXpos[KeybrdCharYpos] -= SMALL_FONT_COLUMN_WIDTH;
        KeybrdCharXpos = CurrentLastXpos[KeybrdCharYpos];
      }
      else
      {
      }
    }
    BSP_LCD_DisplayChar(CurrentLastXpos[KeybrdCharYpos], KeybrdCharYpos, ' ');
  }
  else
  {
    /* Update the cursor position on LCD */
    BSP_LCD_DisplayChar(KeybrdCharXpos, KeybrdCharYpos, data);  
    
    /* Increment char X position */
    KeybrdCharXpos += SMALL_FONT_COLUMN_WIDTH;
    
    CurrentLastXpos[KeybrdCharYpos] = KeybrdCharXpos;
    /* Check if the X position has reached the last column */
    if(KeybrdCharXpos == KYBRD_LAST_COLUMN)
    {
      KeybrdCharXpos = KYBRD_FIRST_COLUMN;
      
      /* Increment char Y position */
      KeybrdCharYpos += SMALL_FONT_LINE_WIDTH;
    }
    
    if(KeybrdCharYpos > KYBRD_LAST_LINE)
    {
      LCD_LOG_ClearTextZone();
      KeybrdCharYpos = KYBRD_FIRST_LINE;
      /* Start New Display of the cursor position on LCD */
      BSP_LCD_DisplayChar(KeybrdCharXpos,KeybrdCharYpos, data);  
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
