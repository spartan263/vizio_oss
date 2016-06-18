/*
 * linux/arch/arm/mach-mt53xx/pdwnc_gpio6896.c
 *
 * Native GPIO driver.
 *
 * Copyright (c) 2006-2012 MediaTek Inc.
 * $Author: dtvbm11 $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */


#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>

#include <mach/mtk_gpio_internal.h>
#include <mach/mt53xx_linux.h>
#include <mach/platform.h>

#include <asm/io.h>

#define ASSERT(x)
#define VERIFY(x)
#define LOG(...)

#define u4IO32Read4B    __raw_readl
#define vIO32Write4B(addr,val)    __raw_writel(val,addr)

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#if defined(CONFIG_ARCH_MT5398)
#include <mach/mt5398/x_gpio_hw.h>
//pdwnc gpio int mark
#define REG_RW_PDWNC_INTEN_GPIO_MASK 0x0c0f00ff
#elif defined(CONFIG_ARCH_MT5880)
#include <mach/mt5880/x_gpio_hw.h>
//pdwnc gpio int mark
#define REG_RW_PDWNC_INTEN_GPIO_MASK 0x000000ff 
#endif


//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static mtk_gpio_callback_t _afnPDWNCGpioCallback[TOTAL_PDWNC_GPIO_NUM];
static void* _au4PdwncGpioCallBackArg[TOTAL_PDWNC_GPIO_NUM];

#if defined(CONFIG_ARCH_MT5398)
static const INT8 _ai1PdwncInt2Gpio[MAX_PDWNC_INT_ID] =
{//check 2804c[7:0] [18] and [24:31], translate them to bit offset within 280A8(offset of sysytem gpio number), only 17 gpio can issue interrupt
    0, 1, 2, 3, 4, 5, 6, 7,
   -1, -1, -1, -1, -1, -1, -1, -1,
   8, 9, 10, 11, -1, -1, -1, -1,
   -1, -1, 13, 15, -1, -1, -1,-1   
};

static const INT8 _ai1PdwncGpio2Int[TOTAL_PDWNC_GPIO_NUM] =
{
	0, 1, 2, 3, 4, 5, 6, 7,
    16, 17, 18, 19, -1, 26, -1, 27,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1
};
#elif defined(CONFIG_ARCH_MT5880)
static const INT8 _ai1PdwncInt2Gpio[MAX_PDWNC_INT_ID] =
{//check 2804c[7:0] [18] and [24:31], translate them to bit offset within 280A8(offset of sysytem gpio number), only 17 gpio can issue interrupt
    0, 1, 2, 3, 4, 5, 6, 9,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1,-1   
};

static const INT8 _ai1PdwncGpio2Int[TOTAL_PDWNC_GPIO_NUM] =
{
	0, 1, 2, 3, 4, 5, 6, -1,
   -1, 7, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,
   -1
};
#endif

//-----------------------------------------------------------------------------
// Prototypes 
//-----------------------------------------------------------------------------
//INT32 PDWNC_GpioEnable(INT32 i4Gpio, INT32 *pfgSet);
//INT32 PDWNC_GpioOutput(INT32 i4Gpio, INT32 *pfgSet);
//INT32 PDWNC_GpioInput(INT32 i4Gpio);
INT32 PDWNC_GpioIntrq(INT32 i4Gpio, INT32 *pfgSet);
INT32 PDWNC_GpioReg(INT32 i4Gpio, MTK_GPIO_IRQ_TYPE eType, mtk_gpio_callback_t callback, void *data);

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** PDWNC_InitGpio(). Initialize the interrupt routine for GPIO irq.
 *  @param void
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_InitGpio(void) //need not request isr, mtk pdwnc gpio driver will reg isr, then call kernel isr related func.
{
    UINT32 i;	

    // Clear callback function array.
    for (i=0; i<TOTAL_PDWNC_GPIO_NUM;i++)
    {
        _afnPDWNCGpioCallback[i] = NULL;
        _au4PdwncGpioCallBackArg[i] = 0;
    }
	
    // disable all gpio interrupt 
    vIO32Write4B(PDWNC_ARM_INTEN, u4IO32Read4B(PDWNC_ARM_INTEN) & (~(REG_RW_PDWNC_INTEN_GPIO_MASK)) );
    vIO32Write4B(PDWNC_EXINT2ED, 0);
    vIO32Write4B(PDWNC_EXINTLEV, 0);
    vIO32Write4B(PDWNC_EXINTPOL, 0);    
    vIO32Write4B(PDWNC_INTCLR, REG_RW_PDWNC_INTEN_GPIO_MASK); //just clean gpio int.

    return 0;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

INT32 PDWNC_ServoGpioRangeCheck(INT32 i4Gpio)
{
    if((i4Gpio >= (ADC2GPIO_CH_ID_MIN + SERVO_0_ALIAS)) && (i4Gpio <= (ADC2GPIO_CH_ID_MAX + SERVO_0_ALIAS)))//only srvad2~7 can be configured as gpio for 5398
    {
        return 1;
    }
    else
    {   
        return 0;
    }   
}

INT32 PDWNC_GpioRangeCheck(INT32 i4Gpio)
{
		if ((i4Gpio >= OPCTRL(0)) && (i4Gpio <= (TOTAL_PDWNC_GPIO_NUM + OPCTRL(0))))
		{
			return 1;
		}
		else
		{	
			return 0;
		}	
}
//-----------------------------------------------------------------------------
/** PDWNC_GpioEnable() The GPIO input/output mode setting functions. It will
 *  check the i4Gpio and set to related register bit as 0 or 1.  In this 
 *  function, 0 is input mode and 1 is output mode.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *  the current mode of gpio number (0 or 1).  If it's not NULL, it must
 *  reference to a integer.  If the integer is 0, this function will set the
 *  mode of the gpio number as input mode, otherwise set as output mode.
 *  @retval If pfgSet is NULL, it return 0 or 1 (0 is input mode, 1 is output
 *          mode.)  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioEnable(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val;
    INT32 i4Idx;	
    unsigned long rCrit;

    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    i4Idx = i4Gpio;	
	i4Idx &= 0x1f;

    spin_lock_irqsave(&mt53xx_bim_lock, rCrit);

    if (i4Gpio <= 31)
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN0);
    }
    else
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOEN1);
    }

    if (pfgSet == NULL)
    {
        spin_unlock_irqrestore(&mt53xx_bim_lock, rCrit);

        return ((u4Val & (1U << i4Idx)) ? 1 : 0);
    }
    u4Val = (*pfgSet) ?
                (u4Val | (1U << i4Idx)) :
                (u4Val & ~(1U << i4Idx));
				
    if (i4Gpio <= 31)
    {
        vIO32Write4B(PDWNC_GPIOEN0, u4Val);
    }
    else
    {
        vIO32Write4B(PDWNC_GPIOEN1, u4Val);
    }

	spin_unlock_irqrestore(&mt53xx_bim_lock, rCrit);
	
    return (*pfgSet);
}

//-----------------------------------------------------------------------------
/** PDWNC_GpioOutput() The GPIO output value setting functions. It will check
 *  the i4Gpio and set to related register bit as 0 or 1.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *          the bit of gpio number (0 or 1).  If it's not NULL, it must 
 *          reference to a integer.  If the integer is 0, this function 
 *          will set the bit of the gpio number as 0, otherwise set as 1.
 *  @retval If pfgSet is NULL, it return 0 or 1 (the bit value of the gpio
 *          number of output mode.  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioOutput(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val,u4Val1;
    INT32 i4Idx,i4Val;	
    unsigned long rCrit;

    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    i4Idx = i4Gpio;	
	i4Idx &= 0x1f;
	
    spin_lock_irqsave(&mt53xx_bim_lock, rCrit);

    if (pfgSet == NULL)	//NULL: for query gpio status, must be GPIO output , but not change pin level
    {
        spin_unlock_irqrestore(&mt53xx_bim_lock, rCrit);

		if(PDWNC_GpioEnable(i4Gpio, NULL) == 0)//old is input state, change to be output
		{
			//get pin level		
		    if (i4Gpio <= 31)
		    {
				u4Val1 = u4IO32Read4B(PDWNC_GPIOIN0);
		    }
		    else
		    {
				u4Val1 = u4IO32Read4B(PDWNC_GPIOIN1);
		    }

			//get current out register setting
			if (i4Gpio <= 31)
			{
				u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
			}
			else
			{
				u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
			}

			u4Val = (u4Val1 & (1U << i4Idx)) ?
						(u4Val | (1U << i4Idx)) :
						(u4Val & ~(1U << i4Idx));

			if (i4Gpio <= 31)
			{
				vIO32Write4B(PDWNC_GPIOOUT0, u4Val);
			}
			else
			{
				vIO32Write4B(PDWNC_GPIOOUT1, u4Val);
			}
		
			// Set the output mode.
			i4Val = 1;
			VERIFY(1 == PDWNC_GpioEnable(i4Gpio, &i4Val));

		}
		
	//get out value
		if (i4Gpio <= 31)
		{
			u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
		}
		else
		{
			u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
		}

        return ((u4Val & (1U << i4Idx)) ? 1 : 0);
    }

//get out value
	if (i4Gpio <= 31)
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT0);
	}
	else
	{
		u4Val = u4IO32Read4B(PDWNC_GPIOOUT1);
	}

    u4Val = (*pfgSet) ?
                (u4Val | (1U << i4Idx)) :
                (u4Val & ~(1U << i4Idx));
				
    if (i4Gpio <= 31)
    {
        vIO32Write4B(PDWNC_GPIOOUT0, u4Val);
    }
    else
    {
        vIO32Write4B(PDWNC_GPIOOUT1, u4Val);
    }

	spin_unlock_irqrestore(&mt53xx_bim_lock, rCrit);

    // Set the output mode.
    i4Val = 1;
    VERIFY(1 == PDWNC_GpioEnable(i4Gpio, &i4Val));

    // _PdwncSetGpioPin(i4Gpio);
 
    return (*pfgSet);
}

//-----------------------------------------------------------------------------
/** PDWNC_GpioInput()  The GPIO input reading functions. It will check the 
 *  i4Gpio and read related register bit to return.
 *  @param i4Gpio the gpio number to read.
 *  @retval 0 or 1.  (GPIO input value.)
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioInput(INT32 i4Gpio)
{
    UINT32 u4Val;
    INT32 i4Idx = 0;
	
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    i4Idx = i4Gpio;	
	i4Idx &= 0x1f;
    
    if (i4Gpio <= 31)
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOIN0);		
		return ((u4Val & (1U << i4Idx)) ? 1U : 0);	
    }
    else
    {
        u4Val = u4IO32Read4B(PDWNC_GPIOIN1);
		return ((u4Val & (1U << i4Idx)) ? 1U : 0);	
    }
}


//-----------------------------------------------------------------------------
/** PDWNC_GpioIntrq() The GPIO interrupt enable setting functions. It will
 *  check the i4Gpio and set to related register bit as 0 or 1.  In this 
 *  function, 0 is interrupt disable mode and 1 is interrupt enable mode.
 *  @param i4Gpio the gpio number to set or read.
 *  @param pfgSet A integer pointer.  If it's NULL, this function will return
 *  the current setting of gpio number (0 or 1).  If it's not NULL, it must
 *  reference to a integer.  If the integer is 0, this function will set the
 *  mode of the gpio number as interrupt disable mode, otherwise set as
 *  interrupt enable mode.
 *  @retval If pfgSet is NULL, it return 0 or 1 (0 is interrupt disable mode,
 *          1 is interrupt enable mode.)  Otherwise, return (*pfgSet).
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioIntrq(INT32 i4Gpio, INT32 *pfgSet)
{
    UINT32 u4Val;
    INT32 i4Int;
    UNUSED(_ai1PdwncGpio2Int);
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }

    if((i4Int = _ai1PdwncGpio2Int[i4Gpio]) < 0)
    {
        return -1;    
    }

    u4Val = u4IO32Read4B(PDWNC_ARM_INTEN);
    if (pfgSet == NULL)
    {
        return ((u4Val & (1U << i4Int)) ? 1 : 0);
    }
    u4Val = (*pfgSet) ?
            (u4Val | OPCTRL_INTEN(i4Int)) :
            (u4Val & ~ OPCTRL_INTEN(i4Int));
    vIO32Write4B(PDWNC_ARM_INTEN, u4Val);

    return (*pfgSet);
}


//-----------------------------------------------------------------------------
/* PDWNC_GpioReg() to setup the PDWNC Gpio interrupt callback function, type,
 *      and state.
 *  @param i4Gpio should be between 0~7.
 *  @param eType should be one of enum GPIO_TYPE.
 *  @param pfnCallback the callback function.
 *  @retval 0 successful, -1 failed.
 */
//-----------------------------------------------------------------------------
INT32 PDWNC_GpioReg(INT32 i4Gpio, MTK_GPIO_IRQ_TYPE eType, mtk_gpio_callback_t callback, void *data)
{
    UINT32 u4Val;
    INT32 i4Int;
	
    if ((i4Gpio < 0) || (i4Gpio >= TOTAL_PDWNC_GPIO_NUM))
    {
        return -1;
    }
	
    if((eType != MTK_GPIO_TYPE_NONE) && (callback == NULL))
    {
        return -1;        
    }

    if((i4Int = _ai1PdwncGpio2Int[i4Gpio]) < 0)    
    {
        return -1;        
    }

     /* Set the register and callback function. */
    //i4Int = (i4Int>=24)? (i4Int-16):(i4Int);
    //u4Gpio = _ai1PdwncInt2Gpio[i4Int];
    switch(eType)
    {
        case MTK_GPIO_TYPE_INTR_FALL:
            u4Val = u4IO32Read4B(PDWNC_EXINT2ED);
            u4Val &= ~(1U << i4Int); // Disable double edge trigger.
            vIO32Write4B(PDWNC_EXINT2ED, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTLEV);
            u4Val &= ~(1U << i4Int);  // Set Edge trigger.
            vIO32Write4B(PDWNC_EXINTLEV, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTPOL);
            u4Val &= ~(1U << i4Int);       // Set Falling Edge interrupt.
            vIO32Write4B(PDWNC_EXINTPOL, u4Val);
            break;
            
        case MTK_GPIO_TYPE_INTR_RISE:            
            u4Val = u4IO32Read4B(PDWNC_EXINT2ED);
            u4Val &= ~(1U << i4Int); // Disable double edge trigger.
            vIO32Write4B(PDWNC_EXINT2ED, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTLEV);
            u4Val &= ~(1U << i4Int);  // Set Edge trigger.
            vIO32Write4B(PDWNC_EXINTLEV, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTPOL);
            u4Val |= (1U << i4Int);       // Set rasing Edge interrupt.
            vIO32Write4B(PDWNC_EXINTPOL, u4Val);
            break;               
            
        case MTK_GPIO_TYPE_INTR_BOTH:            
            u4Val = u4IO32Read4B(PDWNC_EXINT2ED);
            u4Val |= (1U << i4Int); // enable double edge trigger.
            vIO32Write4B(PDWNC_EXINT2ED, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTLEV);
            u4Val &= ~(1U << i4Int);  // Set Edge trigger.
            vIO32Write4B(PDWNC_EXINTLEV, u4Val);
            break;
            
        case MTK_GPIO_TYPE_INTR_LEVEL_LOW:            
            u4Val = u4IO32Read4B(PDWNC_EXINT2ED);
            u4Val &= ~(1U << i4Int); // Disable double edge trigger.
            vIO32Write4B(PDWNC_EXINT2ED, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTLEV);
            u4Val |= (1U << i4Int);  // Set Level trigger.
            vIO32Write4B(PDWNC_EXINTLEV, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTPOL);
            u4Val &= ~(1U << i4Int);       // Set level low interrupt.
            vIO32Write4B(PDWNC_EXINTPOL, u4Val);
            break;
            
        case MTK_GPIO_TYPE_INTR_LEVEL_HIGH:            
            u4Val = u4IO32Read4B(PDWNC_EXINT2ED);
            u4Val &= ~(1U << i4Int); // Disable double edge trigger.
            vIO32Write4B(PDWNC_EXINT2ED, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTLEV);
            u4Val |= (1U << i4Int);  // Set level trigger.
            vIO32Write4B(PDWNC_EXINTLEV, u4Val);
            u4Val = u4IO32Read4B(PDWNC_EXINTPOL);
            u4Val |= (1U << i4Int);       // Set level high interrupt.
            vIO32Write4B(PDWNC_EXINTPOL, u4Val);
            break;
            
        default:
			u4Val = u4IO32Read4B(PDWNC_ARM_INTEN);
			u4Val &= ~(1U << i4Int);	   // Disable interrupt.
			vIO32Write4B(PDWNC_ARM_INTEN, u4Val);
			return 0;
        }

		u4Val = u4IO32Read4B(PDWNC_ARM_INTEN);
		u4Val |= (1U << i4Int); 	   // Enable interrupt.
		vIO32Write4B(PDWNC_ARM_INTEN, u4Val);

    if (callback)
    {
        _afnPDWNCGpioCallback[i4Gpio] = callback;        
        _au4PdwncGpioCallBackArg[i4Gpio] = data;
    }
//    printf("Interrupt seting is:0x%8x,0x%8x,0x%8x\n",u4IO32Read4B(PDWNC_EXINT2ED),u4IO32Read4B(PDWNC_EXINTLEV),u4IO32Read4B(PDWNC_EXINTPOL));
    
    return 0;
}

static UINT32 PDWNC_GPIO_POLL_IN_REG(INT32 i4Idx)
{//only for pdwnc gpio
    return PDWNC_GPIO_IN_REG(i4Idx);
}

//-----------------------------------------------------------------------------
/** mtk_pdwnc_gpio_isr_func() is the pdwnc gpio isr related  function
 */
//-----------------------------------------------------------------------------
int mtk_pdwnc_gpio_isr_func(unsigned u2IntIndex)
{
    INT32 i4GpioNum;
	
	BUG_ON(u2IntIndex >= MAX_PDWNC_INT_ID);
	
	// handle edge triggered interrupt.	
	i4GpioNum = _ai1PdwncInt2Gpio[u2IntIndex];
    if((i4GpioNum != -1))
    {
        if ((_afnPDWNCGpioCallback[i4GpioNum] != NULL))
        { 
            _afnPDWNCGpioCallback[i4GpioNum]((i4GpioNum + OPCTRL(0)),  PDWNC_GPIO_POLL_IN_REG(GPIO_TO_INDEX(i4GpioNum)) & (1U << (i4GpioNum & GPIO_INDEX_MASK))? 1:0, _au4PdwncGpioCallBackArg[i4GpioNum]);
        } 			
    }
	
    vIO32Write4B(PDWNC_INTCLR,  _PINT(u2IntIndex));
    return 0;

}

EXPORT_SYMBOL(mtk_pdwnc_gpio_isr_func);


