/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: dtvbm11 $
 * $Date: 2016/03/31 $
 * $RCSfile: x_module.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef X_MODULE_H
#define X_MODULE_H


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_typedef.h"
#include "x_os.h"
#include <linux/module.h>
#include <linux/init.h>


//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

#ifdef MODULE_NODE_NAME

#ifdef MODULE_NODE_MAJOR
static int major = MODULE_NODE_MAJOR;
#else
static int major = 0;
#endif
module_param(major, int, 0);
#endif


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

static int __init _mod_init(unsigned long *_ioaddr)
{
    int result;
    UNUSED(result);

#if defined(MODULE_IO_ADDR) && defined(MODULE_IO_LEN)
    /* Get our needed resources. */
    if (! request_mem_region(MODULE_IO_ADDR, MODULE_IO_LEN,
        "MT5351 "MODULE_NODE_NAME))
    {
        printk(KERN_DEBUG MODULE_NODE_NAME
               ": can't get I/O mem address 0x%x\n", MODULE_IO_ADDR);
        return -ENODEV;
    }

    /* also, ioremap it */
    *_ioaddr = (unsigned long)ioremap(MODULE_IO_ADDR, MODULE_IO_LEN);
    /* Hmm... we should check the return value */
    printk(KERN_DEBUG MODULE_NODE_NAME"_ioaddr = 0x%lx\n", *_ioaddr);
#endif

#ifdef MODULE_NODE_NAME
    result = register_chrdev(major, MODULE_NODE_NAME, &MODULE_FOPS);
    if (result < 0)
    {
        printk(KERN_DEBUG MODULE_NODE_NAME": can't get major number\n");
        return result;
    }
    if (major == 0)
    {
        major = result;
    }
    printk(KERN_DEBUG MODULE_NODE_NAME" major = %d\n", major);
#endif

#ifdef MODULE_INIT_FUNC
    MODULE_INIT_FUNC();
#endif //MODULE_EXIT_FUNC

    return 0;
}

static void __exit _mod_exit(unsigned long _ioaddr)
{
#ifdef MODULE_NODE_NAME
    #ifdef MODULE_EXIT_FUNC
        MODULE_EXIT_FUNC();
    #endif //MODULE_EXIT_FUNC

    flush_scheduled_work();
    unregister_chrdev(major, MODULE_NODE_NAME);
#endif //MODULE_NODE_NAME

#if defined(MODULE_IO_ADDR) && defined(MODULE_IO_LEN)
    iounmap((void __iomem *)_ioaddr);
    release_mem_region(MODULE_IO_ADDR, MODULE_IO_LEN);
#endif
}


#define DECLARE_MODULE(mod)                                                 \
                                                                            \
unsigned long mod##_ioaddr = 0;                                             \
                                                                            \
static int __init mod##_init(void)                                          \
{                                                                           \
    _mod_init(&mod##_ioaddr);                                               \
                                                                            \
    return 0;                                                               \
}                                                                           \
                                                                            \
static void __exit mod##_exit(void)                                         \
{                                                                           \
    _mod_exit(mod##_ioaddr);                                                \
}                                                                           \
                                                                            \
module_init(mod##_init);                                                    \
module_exit(mod##_exit);                                                    \


#endif //X_MODULE_H

