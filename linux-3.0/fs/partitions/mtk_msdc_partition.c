/*
 *
 *  fs/partitions/mtkemmc.c
 *
 *  this partition system using for EMMC
 *
 *  maked by jun.wang
 *
 */

#include <linux/proc_fs.h>
#include <linux/mmc/card.h>

#include "check.h"
#include "mtk_msdc_partition.h"

/*
 * Many architectures don't like unaligned accesses, while
 * the nr_sects and start_sect partition table entries are
 * at a 2 (mod 4) address.
 */
#include <asm/unaligned.h>

mtk_partition mtk_msdc_partition;

/* the command line passed to mtdpart_setupd() */
static char *cmdline;

static char *newpart(char *s)
{
    char *retptr;
    unsigned long long size = 0;
    unsigned long long offset = 0;
    char *name;
    int name_len;
    unsigned char partno;
    mtk_part *part;
#ifdef CC_MTD_ENCRYPT_SUPPORT 
    unsigned int attr = 0;
#endif
    /* fetch the partition size */
	size = memparse(s, &s);
    BUG_ON(size < 512);
    BUG_ON(size % 512);
    size /= 512;

    /* check for offset */
    if (*s == '@')
    {
        s++;
        offset = memparse(s, &s);
        BUG_ON(offset % 512);
    }
    
    /* now look for name */
    if (*s == '(')
    {
    	char *p;

        name = ++s;
    	p = strchr(name, ')');
    	BUG_ON(!p);
        
    	name_len = p - name;
    	s = p + 1;
    }
    else
    {
        name = NULL;
    	name_len = 8; /* Part_000 */
    }

    if (strncmp(s, "ro", 2) == 0)
    {
        s += 2;
    }

    if (strncmp(s, "lk", 2) == 0)
    {
        s += 2;
    }

    if (strncmp(s, "enc", 3) == 0)
    {
        s += 3;
#ifdef CC_MTD_ENCRYPT_SUPPORT 
        attr |= ATTR_ENCRYPTION;
#endif
    }

    /* test if more partitions are following */
    if (*s == ',')
    {
        /* return (updated) pointer command line string */
        retptr = ++s;
    }
    else
    {
        retptr = NULL;
    }
    
    mtk_msdc_partition.nparts++;
    partno= mtk_msdc_partition.nparts;
    part = &(mtk_msdc_partition.parts[mtk_msdc_partition.nparts]);
    
    if (name)
    {
    	/* FIXME: what if name size is over 32 bytes? */
    	BUG_ON(name_len >= 31);
    	strlcpy(part->name, name, name_len + 1);
    }
    else
    {
    	sprintf(part->name, "Part_%02d", partno);
    }

    if (partno > 1)
    {
        mtk_part *prevpart = &(mtk_msdc_partition.parts[partno-1]);
        part->start_sect = offset + prevpart->start_sect + prevpart->nr_sects;
    }
    else
    {
        part->start_sect = offset;
    }
    
    part->nr_sects = size;
#ifdef CC_MTD_ENCRYPT_SUPPORT 
    part->attr = attr;
#endif
    return retptr;
}

int msdcpart_setup_real(void)
{
    char *p;
    
    if(mtk_msdc_partition.fg_loaded == 1)
    {
        return 0;
    }

    memset(&mtk_msdc_partition, 0, sizeof(mtk_msdc_partition));
    
    /* fetch <mtd-id> */
    if (!(p = strchr(cmdline, ':')))
    {
        printk(KERN_ERR "mtdpart_setup_real: no mtd-id!\n");
        return -1;
    }
    
    p++;

    while (p)
    {
        p = newpart(p);
    }

    mtk_msdc_partition.fg_loaded = 1;

    return 0;
}

#ifdef CONFIG_PROC_FS
/*====================================================================*/

// Support for /proc/partinfo
static struct proc_dir_entry *proc_partinfo;

static int partinfo_read_proc (char *page, char **start, off_t off, int count,
			  int *eof, void *data_unused)
{
	int len, l, partno;
    off_t begin = 0;
    mtk_part *part;

	len = sprintf(page, "dev:    offset   size     name\n");

	for (partno = 1; partno <= mtk_msdc_partition.nparts; partno++)
    {
        part = &(mtk_msdc_partition.parts[partno]);

        l = sprintf(page + len, "part%02d: %8.8llx %8.8llx \"%s\"\n", partno,
				(unsigned long long)part->start_sect,
				(unsigned long long)part->nr_sects,
				part->name);
	    len += l;

        if (len+begin > off+count)
        {
            goto done;
        }
        
	    if (len+begin < off)
        {
            begin += len;
            len = 0;
	    }
	}

	*eof = 1;

done:
	if (off >= len+begin)
	{
	    return 0;
	}
    
	*start = page + (off-begin);
	return ((count < begin+len-off) ? count : begin+len-off);
}
#endif /* CONFIG_PROC_FS */

int mtkmsdc_partition(struct parsed_partitions *state)
{
    int partno;
    mtk_part *part;
    static int i = 0;

    if(i == 1)
    {
        return 0;
    }

    BUG_ON(msdcpart_setup_real());
    BUG_ON(mtk_msdc_partition.nparts >= state->limit);

    for(partno = 1; partno <= mtk_msdc_partition.nparts; partno++)
    {
        part = &(mtk_msdc_partition.parts[partno]);
        //put_named_partition(state, partno, part->start_sect, part->nr_sects, part->name, strlen(part->name));
        put_partition(state, partno, part->start_sect, part->nr_sects);
        
        printk(KERN_INFO "part %02d: startblk 0x%08llx, blkcnt 0x%08llx, <%s>\n",
             partno, part->start_sect, part->nr_sects, part->name);
    }
    
#ifdef CONFIG_PROC_FS
	if ((proc_partinfo = create_proc_entry("partinfo", 0, NULL)))
		proc_partinfo->read_proc = partinfo_read_proc;
#endif /* CONFIG_PROC_FS */

    i = 1;

	return 1;
}

/*
 * This is the handler for our kernel parameter, called from
 * main.c::checksetup(). Note that we can not yet kmalloc() anything,
 * so we only save the commandline for later processing.
 *
 * This function needs to be visible for bootloaders.
 */
static int msdcpart_setup(char *s)
{
	cmdline = s;
	return 1;
}
__setup("mtdparts=", msdcpart_setup);

