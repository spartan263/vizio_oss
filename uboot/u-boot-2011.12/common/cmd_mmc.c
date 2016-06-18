/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <jffs2/jffs2.h>
#include <mmc.h>
#include <bootimg.h>


#if defined(CC_EMMC_BOOT)

#if defined(CONFIG_CMD_JFFS2) && defined(CONFIG_JFFS2_CMDLINE)
/* parition handling routines */
int mtdparts_init(void);
int id_parse(const char *id, const char **ret_id, u8 *dev_type, u8 *dev_num);
int find_dev_and_part(const char *id, struct mtd_device **dev, u8 *part_num, struct part_info **part);
int find_partition(const char *part_name, u8 *part_num, u32 *part_size, u32 *part_offset);
#endif

static void print_mmcinfo(struct mmc *mmc)
{
	printf("Device: %s\n", mmc->name);
	printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
	printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
			(mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
			(mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

	printf("Tran Speed: %d\n", mmc->tran_speed);
	printf("Rd Block Len: %d\n", mmc->read_bl_len);

	printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
			(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	printf("Capacity: %lld\n", mmc->capacity);

	printf("Bus Width: %d-bit\n", mmc->bus_width);
}

int do_mmcinfo (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc *mmc;
	int dev_num;

	if (argc < 2)
	{
		dev_num = 0;
	}
	else
	{
		dev_num = simple_strtoul(argv[1], NULL, 0);
	}

	mmc = find_mmc_device(dev_num);

	if (mmc) 
  {
		mmc_init(mmc);
		print_mmcinfo(mmc);
	}

	return 0;
}

U_BOOT_CMD(
	mmcinfo, 2, 0, do_mmcinfo,
	"display MMC info",
	"<dev num>\n"
	"    - device number of the device to dislay info of\n"
	""
);

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int rc = 0;

	switch (argc) 
  {
	case 3:
    if (strcmp(argv[1], "rescan") == 0) 
    {
			int dev = simple_strtoul(argv[2], NULL, 10);
			struct mmc *mmc = find_mmc_device(dev);

			if (!mmc)
			{
				return 1;
			}

			mmc_init(mmc);
			return 0;
		} 
    else if (strncmp(argv[1], "part", 4) == 0) 
    {
			int dev = simple_strtoul(argv[2], NULL, 10);
			block_dev_desc_t *mmc_dev;
			struct mmc *mmc = find_mmc_device(dev);

			if (!mmc) 
      {
				puts("no mmc devices available\n");
				return 1;
			}
            
			mmc_init(mmc);
			mmc_dev = mmc_get_dev(dev);
			if (mmc_dev != NULL && mmc_dev->type != DEV_TYPE_UNKNOWN) 
			{
				print_part(mmc_dev);
				return 0;
			}

			puts("get mmc type error!\n");
			return 1;
		}
		else if (strcmp(argv[1], "eraseall") == 0)
    {
      int dev = simple_strtoul(argv[2], NULL, 10);
			struct mmc *mmc = find_mmc_device(dev);
			u32 n;

			if (!mmc)
			{
				return 1;
			}  
			
			n = mmc_erase_all(mmc);
      
      return n;	  	
    } 
		
	case 0:
	case 1:
	case 4:
    return cmd_usage(cmdtp);

	case 2:
		if (!strcmp(argv[1], "list"))
    {
			print_mmc_devices('\n');
			return 0;
		}
		return 1;
        
	default: /* at least 5 args */
		if ((strcmp(argv[1], "read") == 0) || (strcmp(argv[1], "read.b") == 0))
    {
			int dev = simple_strtoul(argv[2], NULL, 10);
			void *addr = (void *)simple_strtoul(argv[3], NULL, 16);
			u32 cnt = simple_strtoul(argv[5], NULL, 16);
			u32 n;
			u32 blk = simple_strtoul(argv[4], NULL, 16);
            
			struct mmc *mmc = find_mmc_device(dev);
			if (!mmc)
			{
				return 1;
			}

      if (strcmp(argv[1], "read") == 0)
      {
			    printf("\nMMC block read: dev # %d, block # %d, count %d ... ", dev, blk, cnt);
      }
      else
      {
			    printf("\nMMC byte read: dev # %d, offset # %d, count %d ... ", dev, blk, cnt);
      }

			mmc_init(mmc);

      if (strcmp(argv[1], "read") == 0)
      {
        n = mmc->block_dev.block_read(dev, blk, cnt, addr);
                
        /* flush cache after read */
        flush_cache((ulong)addr, cnt * 512); /* FIXME */
      }
      else
      {
        n = mmc->block_dev.byte_read(dev, blk, cnt, addr);
                
        /* flush cache after read */
        flush_cache((ulong)addr, cnt); /* FIXME */
      }
            
      if (strcmp(argv[1], "read") == 0)
      {
        printf("%d blocks read: %s\n", n, (n==cnt) ? "OK" : "ERROR");
      }
      else
      {
        printf("%d bytes read: %s\n", n, (n==cnt) ? "OK" : "ERROR");
      }
			return (n == cnt) ? 0 : 1;
		} 
    else if ((strcmp(argv[1], "write") == 0) || (strcmp(argv[1], "write.b") == 0))
    {
			int dev = simple_strtoul(argv[2], NULL, 10);
			void *addr = (void *)simple_strtoul(argv[3], NULL, 16);
			u32 cnt = simple_strtoul(argv[5], NULL, 16);
			u32 n;
			int blk = simple_strtoul(argv[4], NULL, 16);
			struct mmc *mmc = find_mmc_device(dev);
			if (!mmc)
			{
				return 1;
			}

      if (strcmp(argv[1], "read") == 0)
      {
        printf("\nMMC block write: dev # %d, block # %d, count %d ... ", dev, blk, cnt);
      }
      else
      {
        printf("\nMMC byte write: dev # %d, offset # %d, count %d ... ", dev, blk, cnt);
      }

			mmc_init(mmc);

      if (strcmp(argv[1], "write") == 0)
      {
        n = mmc->block_dev.block_write(dev, blk, cnt, addr);
      }
      else
      {
        n = mmc->block_dev.byte_write(dev, blk, cnt, addr);
      }

      if (strcmp(argv[1], "read") == 0)
      {
        printf("%d blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");
      }
      else
      {
        printf("%d bytes written: %s\n", n, (n == cnt) ? "OK" : "ERROR");
      }
			return (n == cnt) ? 0 : 1;
		} 
    else if ((strcmp(argv[1], "erase") == 0) || 
    	       (strcmp(argv[1], "trim") == 0) ||
    	       (strcmp(argv[1], "secureerase") == 0) ||
    	       (strcmp(argv[1], "securetrim1") == 0) ||
    	       (strcmp(argv[1], "securetrim2") == 0))
    {
      int dev = simple_strtoul(argv[2], NULL, 10);
			u32 bytestart = simple_strtoul(argv[3], NULL, 16);
			u32 length = simple_strtoul(argv[4], NULL, 16);
			int mode;
			u32 n;
			
			struct mmc *mmc = find_mmc_device(dev);
			if (!mmc)
			{
				return 1;
			}  
			
			if (strcmp(argv[1], "erase") == 0)
      {
          mode = 0;      
      } 
      else if (strcmp(argv[1], "trim") == 0)
      {
          mode = 1;      
      }
      else if (strcmp(argv[1], "secureerase") == 0)
      {
          mode = 2;      
      }
      else if (strcmp(argv[1], "securetrim1") == 0)
      {
          mode = 3;      
      }	
      else if (strcmp(argv[1], "securetrim2") == 0)
      {
          mode = 4;      
      }
      
      n = mmc_erase(mmc, bytestart, length, mode);
      
      return n;
    }
    else if (strcmp(argv[1], "wp") == 0)
    {
      int dev = simple_strtoul(argv[2], NULL, 10);
			u32 bytestart = simple_strtoul(argv[3], NULL, 16);
			u32 level = simple_strtoul(argv[4], NULL, 10);
			u32 type = simple_strtoul(argv[5], NULL, 10);
			u32 fgen = simple_strtoul(argv[6], NULL, 10);
			u32 n;
			
			struct mmc *mmc = find_mmc_device(dev);
			if (!mmc)
			{
				return 1;
			}  
			
			n = mmc_wp(mmc, bytestart, level, type, fgen);
			
			return n;
				
    }
    else
    {
			rc = cmd_usage(cmdtp);
    }

		return rc;
	}
}

U_BOOT_CMD(
	mmc, 7, 1, do_mmcops,
	"MMC sub system",
	"read <device num> addr blk# cnt\n"
	"mmc write <device num> addr blk# cnt\n"
	"mmc eraseall\n"
	"mmc erase <device num> byte_addr length mode\n"
	"mmc wp <device num> byte_addr level type en\n"
	"mmc rescan <device num>\n"
	"mmc part <device num> - lists available partition on mmc\n"
	"mmc list - lists available devices");

static int emmc_read(u64 offset, void* addr, size_t cnt)
{
    int r;
    struct mmc *mmc;
    
    mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
    if (!mmc)
    {
        return 1;
    }

    if (mmc_init(mmc))
    {
        puts("MMC init failed\n");
        return 1;
    }

    r = mmc->block_dev.byte_read(CONFIG_SYS_MMC_ENV_DEV, offset, cnt, addr);
    if (r != cnt)
    {
        puts("** Read error\n");
        return 1;
    }
    
    return cnt;
}

#ifdef CC_SECURE_ROM_BOOT
// Auth kernel image at addr
static void emmc_auth_image(struct part_info *part, ulong addr, ulong skip)
{
	image_header_t *hdr = (image_header_t *)addr;
    u32 sig_offset = image_get_image_size(hdr);
    
    emmc_read(part->offset+sig_offset+skip, (void *)CFG_LOAD_ADDR, 0x200);
    sig_authetication(addr, sig_offset, (unsigned long *)CFG_LOAD_ADDR, (unsigned long *)(CFG_LOAD_ADDR+0x100));
}
#else
#define emmc_auth_image(part, addr, skip)
#endif

static int emmc_load_image(cmd_tbl_t *cmdtp, u64 offset, ulong addr, char *cmd)
{
  u32 n;
  char *ep;
  size_t cnt;
  char *local_args[3];
  char local_args_temp[3][0x30];
    
  image_header_t *hdr;
  struct mmc *mmc;
    
	mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	if (!mmc)
	{
		return 1;
	}
    
	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return 1;
	}

  printf("\nLoading from mmc%d, offset 0x%llx\n", CONFIG_SYS_MMC_ENV_DEV, offset);

  n = mmc->block_dev.byte_read(CONFIG_SYS_MMC_ENV_DEV, offset, mmc->read_bl_len, (void *)addr);
  if (n != mmc->read_bl_len)
  {
    puts("** Read error\n");
    show_boot_progress (-56);
    return 1;
  }
  show_boot_progress (56);

  switch (genimg_get_format ((void *)addr))
  {
  case IMAGE_FORMAT_LEGACY:
    hdr = (image_header_t *)addr;

    show_boot_progress (57);
    image_print_contents (hdr);

    cnt = image_get_image_size (hdr);
    break;

  default:
    show_boot_progress (-57);
    puts ("** Unknown image type\n");
    return 1;
  }
    
  n = mmc->block_dev.byte_read(CONFIG_SYS_MMC_ENV_DEV, offset, cnt, (void *)addr);
  if (n != cnt)
  {
    puts("** Read error\n");
    show_boot_progress (-58);
    return 1;
  }
  show_boot_progress (58);

#if !(CONFIG_FAST_BOOT) //skip verify for fast boot
  if (((ep = getenv("verify")) != NULL) && (strcmp(ep, "y") == 0))
  {
    puts ("   Verifying Checksum ... ");
    if (!image_check_dcrc (hdr))
    {
      printf ("Bad Data CRC\n");
      show_boot_progress (-58);
      return NULL;
    }
    puts ("OK\n");
  }
#endif

  if(cmd == NULL)
  {
    return 0;
  }

  /* Loading ok, update default load address */
  load_addr = addr;

  /* Check if we should attempt an auto-start */
  if (((ep = getenv("autostart")) != NULL) && 
       (strcmp(ep, "yes") == 0) &&
       (hdr->ih_type == IH_TYPE_KERNEL))
  {
    local_args[0] = cmd;

    printf("Automatic boot of image at addr 0x%08lx ...\n", addr);

    if ((ep = getenv("initrd")) != NULL)
    {
      local_args[1] = local_args_temp[1];
      local_args[2] = local_args_temp[2];
      addr = CFG_RD_ADDR;
      sprintf(local_args[1], "0x%x", (unsigned int)load_addr);
      sprintf(local_args[2], "0x%x", (unsigned int)addr);
      do_bootm(cmdtp, 0, 3, local_args);
    }
    else
    {
      local_args[1] = NULL;
      local_args[2] = NULL;
      do_bootm(cmdtp, 0, 1, local_args);
    }
        
    return cnt;
  }
    
  return 0;
}

int emmc_read_from_partition(unsigned char* buf, char* pname, u64 offset, unsigned int size)
{
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;

    if ((mtdparts_init() == 0) && (find_dev_and_part(pname, &dev, &pnum, &part) == 0))
    {
        emmc_read(part->offset+offset, buf, size);
        return 0;
    }
    
    return 1;
}


#define EMMC_ASYNC_SIZE (63*1024)

struct emmc_fill_data
{
    int cmd_data_left;
    int recv_data_left;
};

int emmc_fill(void *data)
{
    int size, recv_size, ret;
    struct emmc_fill_data *ef = (struct emmc_fill_data *)data;

    if (!ef->recv_data_left)
        return -1;

    // Wait for previous block complete.
    recv_size = size = (ef->recv_data_left < EMMC_ASYNC_SIZE) ? ef->recv_data_left : EMMC_ASYNC_SIZE;
    ret = emmc_async_dma_wait_finish();
    
    if (ret)
        printf("emmc_async_dma_wait_finish %d ret %d\n", size, ret);
    
    ef->recv_data_left -= size;

    // Now send command for next.
    if (ef->cmd_data_left)
    {
        size = (ef->cmd_data_left < EMMC_ASYNC_SIZE) ? ef->cmd_data_left : EMMC_ASYNC_SIZE;
        ret = emmc_async_dma_start_trigger(size);
        if (ret)
            printf("emmc_async_dma_start_trigger %d ret %d\n", size, ret);
        ef->cmd_data_left -= size;
    }

    return recv_size;
}
extern int unlzo_read(u8 *input, int in_len, int (*fill) (void *fill_data), void *fill_data, u8 *output, int *posp);

// Load & decompress LZO image at the same time.
//
// @param cnt      Size of compressed image on EMMC.
// @param src_buf  Output buffer of source compressed image.
// @param out_buf  Output buffer of decompressed data
// @param decomp_size  Decompressed data size.
// @param skip     Number of bytes to skip at start.
static int emmc_read_unlzo(u64 offset, u8 *src_buf, u8 *out_buf, size_t cnt, int *decomp_size, int skip)
{
    int ret, len, dma_size0, dma_size1, rval=0;
    struct emmc_fill_data fdata;

    // align to 512 bytes
    len = (cnt + 511) & (~511);

    dma_size0 = (len < EMMC_ASYNC_SIZE) ? len : EMMC_ASYNC_SIZE;
    fdata.recv_data_left = fdata.cmd_data_left = len - dma_size0;

    ret = emmc_async_read_setup(0, offset, len, src_buf);
    if (ret)
        printf("emmc_async_read_setup done %d\n", ret);

    // Read first block, must wait for first block
    emmc_async_dma_start_trigger(dma_size0);
    emmc_async_dma_wait_finish();

    // Check for LZO header, return fail if not found.
    if (*(int*)(src_buf+skip) != 0x4f5a4c89)
    {
        rval=1;
        emmc_async_read_stop(0, 0);
        goto out;
    }

    // Now, read next, don't wait.
    if (fdata.cmd_data_left)
    {
        dma_size1 = (fdata.cmd_data_left < EMMC_ASYNC_SIZE) ? fdata.cmd_data_left : EMMC_ASYNC_SIZE;
        emmc_async_dma_start_trigger(dma_size1);
        fdata.cmd_data_left -= dma_size1;
    }

    ret = unlzo_read(src_buf+skip, dma_size0-skip, emmc_fill, &fdata, out_buf, decomp_size);
    if (ret) {
        printf("block unlzo failed.. %d\n", ret);
        rval=1;
        goto out;
    }

out:
    // Send finish command.
    ret = emmc_async_read_finish(0);
    if (ret)
        printf("emmc_async_read_finish done %d\n", ret);

    return rval;
}

static int emmc_load_image_lzo(cmd_tbl_t *cmdtp, struct part_info *part, char *cmd)
{
    int r;
    ulong addr;
	u64 offset;
    size_t cnt;
    int decomp_size;
    char *local_args[3];

    // Booting UIMAGE + LZO kernel.
    // !!!!!FIXME!!!!! We don't know the size of image, try part size.
    addr = 0x7fc0;
    cnt = part->size;
    offset = part->offset;
    r = emmc_read_unlzo(offset, (void*)CFG_RD_SRC_ADDR, (void*)addr, cnt, &decomp_size, 0x40);

    if (!r)
    {
        // verify the kernel.
        emmc_auth_image(part, CFG_RD_SRC_ADDR, 0);
        
        // Good. Decompress work, boot it
        load_addr = addr;
        local_args[0] = cmd;
        local_args[1] = NULL;
        local_args[2] = NULL;
        do_bootm(cmdtp, 0, 1, local_args);
    }

    // read fail, go back and try normal flow
    return 0;
}


#define WITH_PADDING_SIZE(size, page)    (((size)+(page)-1) & (~((page)-1)))

int do_emmcboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    u8 pnum;
    char *tmp, env[64];
    ulong addr;
	u64 rootfsoffset = 0;
    
    struct mtd_device *dev;
    struct part_info *part;
    
    if (argc >= 2) 
    {
        if ((mtdparts_init() == 0) &&
            (find_dev_and_part(argv[1], &dev, &pnum, &part) == 0)) 
        {
            // Fix kernel address at 0x8000 to avoid from reallocation time
            addr = 0x8000;
		
            if (dev->id->type != MTD_DEV_TYPE_EMMC) 
            {
                puts("Not a EMMC device\n");
                return 1;
            }
            
            if (argc > 3)
            {
                goto usage;
            }
            
            if (argc == 3)
            {
                if ((strcmp(argv[2], "rootfs")  == 0) || 
                    (strcmp(argv[2], "rootfsA") == 0) || 
                    (strcmp(argv[2], "rootfsB") == 0))
                {
                    u8 part_num;
                    u32 part_size, part_offset;
                    
                    if (find_partition(argv[2], &part_num, &part_size, &part_offset) != 0)
                    {
                        printf("\n** Ramdisk partition %s not found!\n", argv[2]);
                        show_boot_progress(-55);
                        return 1;
                    }
                    
                    rootfsoffset = part_offset;
                }
                else
                {
                    goto usage;
                }
            }
            else if (argc == 2)
            {
                // android boot
                char *local_args[3];
                char local_args_temp[3][16];
                char env[32];
                struct boot_img_hdr hdr;
                int magic_found, ret;
                int cnt, decomp_size;
                u64 offset;

                // 1. load header
                sprintf(env, "0x%x", sizeof(boot_img_hdr));
                setenv("squashfs_size", env);
                offset = part->offset;
                cnt = 2048;
                emmc_read(offset, (void*)CFG_RD_ADDR, cnt);
                memcpy((void*)&hdr, (void*)CFG_RD_ADDR, sizeof(boot_img_hdr));
                magic_found = !strncmp((char*)hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);

                if (magic_found && strstr(argv[0], ".lzo"))
                {
                    // 2. load kernel
                    offset += hdr.page_size;
                    addr = 0x7fc0;
                    cnt = WITH_PADDING_SIZE(hdr.kernel_size, hdr.page_size);
                    ret = emmc_read_unlzo(offset, (void*)CFG_RD_SRC_ADDR, (void*)addr, cnt, &decomp_size, 0x40);
                    if (ret)
                    {
                        // LZO read fail, read as normal uImage.
                        addr = 0x8000;
                        emmc_read(offset, (void*)addr, WITH_PADDING_SIZE(hdr.kernel_size, hdr.page_size));
                    }
                    
                    // 3. verify the kernel.
                    emmc_auth_image(part, CFG_RD_SRC_ADDR, hdr.page_size);

                    // 4. load ramdisk
                    offset += WITH_PADDING_SIZE(hdr.kernel_size, hdr.page_size);
                    cnt = hdr.ramdisk_size;
                    emmc_read_unlzo(offset, (void*)CFG_RD_SRC_ADDR, (void*)CFG_RD_ADDR, cnt, &decomp_size, 0);

                    sprintf(env, "0x%x", CFG_RD_ADDR);
                    setenv("initrd", env);
                    sprintf(env, "0x%x", CFG_RD_ADDR+decomp_size);
                    setenv("initrd_end", env);

                    // 5. launch kernel
                    local_args[1] = local_args_temp[1];
                    local_args[2] = local_args_temp[2];
                    sprintf(local_args[1], "0x%x", addr);
                    sprintf(local_args[2], "0x%x", CFG_RD_ADDR);
                    return do_bootm(cmdtp, 0, 3, local_args);
                }
                else if (magic_found)
                {
                    // 2. load kernel
                    offset += hdr.page_size;
                    emmc_read(offset, (void*)addr, WITH_PADDING_SIZE(hdr.kernel_size, hdr.page_size));

                    // 3. load ramdisk
                    offset += WITH_PADDING_SIZE(hdr.kernel_size, hdr.page_size);
                    emmc_read(offset, (void*)CFG_RD_ADDR, hdr.ramdisk_size);

                    sprintf(env, "0x%x", CFG_RD_ADDR);
                    setenv("initrd", env);
                    sprintf(env, "0x%x", CFG_RD_ADDR+hdr.ramdisk_size);
                    setenv("initrd_end", env);

                    // 4. launch kernel
                    local_args[1] = local_args_temp[1];
                    local_args[2] = local_args_temp[2];
                    sprintf(local_args[1], "0x%x", addr);
                    sprintf(local_args[2], "0x%x", CFG_RD_ADDR);
                    return do_bootm(cmdtp, 0, 3, local_args);
                }
                else
                {
                    printf("\n no valid boot image!\n");
                    show_boot_progress(-55);
                    return 1;
                }
            }

            // Load rootfs
            if (((tmp = getenv("ramdisk")) != NULL) && (strcmp(tmp, "yes") == 0))
            {
                emmc_load_image(cmdtp, rootfsoffset, CFG_RD_ADDR, argv[0]);
                
                sprintf(env, "0x%x", CFG_RD_ADDR);
                setenv("initrd", env);
            }
            else
            {
                setenv("initrd", NULL);
            }

            // Loading .lzo kernel if specified.
            tmp = strchr(argv[0], '.');
            if (tmp && !strcmp(tmp, ".lzo"))
                emmc_load_image_lzo(cmdtp, part, argv[0]);

            // Load kernel
            return emmc_load_image(cmdtp, part->offset, addr, argv[0]);
        }
    }
    
    return 0;
    
usage:
    printf("Usage:\n%s\n", cmdtp->usage);
    show_boot_progress(-53);
    return 1;
}

U_BOOT_CMD(eboot, 3, 1, do_emmcboot,
    "eboot   - boot from EMMC device\n",
    "[.lzo] [partition] | [[[loadAddr] dev] offset]\n");

#endif // CC_EMMC_BOOT

