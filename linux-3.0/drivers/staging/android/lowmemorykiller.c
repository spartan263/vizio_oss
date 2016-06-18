/* drivers/misc/lowmemorykiller.c
 *
 * The lowmemorykiller driver lets user-space specify a set of memory thresholds
 * where processes with a range of oom_adj values will get killed. Specify the
 * minimum oom_adj values in /sys/module/lowmemorykiller/parameters/adj and the
 * number of free pages in /sys/module/lowmemorykiller/parameters/minfree. Both
 * files take a comma separated list of numbers in ascending order.
 *
 * For example, write "0,8" to /sys/module/lowmemorykiller/parameters/adj and
 * "1024,4096" to /sys/module/lowmemorykiller/parameters/minfree to kill processes
 * with a oom_adj value of 8 or higher when the free memory drops below 4096 pages
 * and kill processes with a oom_adj value of 0 or higher when the free memory
 * drops below 1024 pages.
 *
 * The driver considers memory used for caches to be free, but if a large
 * percentage of the cached memory is locked this can be very inaccurate
 * and processes may not get killed until the normal oom killer is triggered.
 *
 * Copyright (C) 2007-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/notifier.h>
#include <linux/fs.h>
#include <linux/swap.h>

#define ENABLE_ZRAM

/* from xvalloc.c */
extern unsigned int phy_freepages;
extern void drop_cache_android(int drop_caches);

static uint32_t lowmem_debug_level = 2;
static int lowmem_adj[6] = {
	0,
	1,
	6,
	12,
};
static int lowmem_adj_size = 4;
static size_t lowmem_minfree[6] = {
	3 * 512,	/* 6MB */
	2 * 1024,	/* 8MB */
	4 * 1024,	/* 16MB */
	16 * 1024,	/* 64MB */
};
static int lowmem_minfree_size = 4;

static struct task_struct *lowmem_deathpending;
static unsigned long lowmem_deathpending_timeout;

unsigned int swap_threshold = (1 << 8);                /* Default swap_threshold = 8M */
unsigned int phy_swap_threshold = (1 << 8);				/* Default physica swap_threshold = 4M */
int snp_threshold = -2;                                 /* Swap has No Progress */
static int swap_no_progress;
int start_zram_oom_adj = 10;
int start_dropcache_oom_adj = 0;

#define lowmem_print(level, x...)			\
	do {						\
		if (lowmem_debug_level >= (level))	\
			printk(x);			\
	} while (0)

static int
task_notify_func(struct notifier_block *self, unsigned long val, void *data);

static struct notifier_block task_nb = {
	.notifier_call	= task_notify_func,
};

static int
task_notify_func(struct notifier_block *self, unsigned long val, void *data)
{
	struct task_struct *task = data;

	if (task == lowmem_deathpending)
		lowmem_deathpending = NULL;

	return NOTIFY_OK;
}

/* return 1 to use LMK 
 * return 0 to use swap
 */ 
static int swap_guard_fn()
{
	struct sysinfo si;
	unsigned long anon_pages = 0;
	static unsigned long pfreeswap = 0;

    si_meminfo(&si);
    si_swapinfo(&si);

    anon_pages = global_page_state(NR_ANON_PAGES);

	if((phy_freepages != -1) && (phy_freepages < phy_swap_threshold))
	{
		vm_swappiness >>= 1;
		swap_no_progress = snp_threshold;
		lowmem_print(3, "lowmem_shrink phy_freepages=%d, phy_freepages_thr=%d\n", 
					  phy_freepages, phy_swap_threshold);
		return 1;
	}
	else if (si.freeswap > swap_threshold)
	{
		if (unlikely(swap_no_progress >= 0)) {
        	swap_no_progress = snp_threshold;
       	} else if (anon_pages > (si.totalram >> 4)) {
        	if (likely(si.freeswap ^ pfreeswap)) {
        		pfreeswap = si.freeswap;
            	swap_no_progress = snp_threshold;
            } else {
            	swap_no_progress++;
            }
			lowmem_print(3, "lowmem_shrink freeswap=%d, swap_no_progress=%d\n", 
						pfreeswap, swap_no_progress);
            vm_swappiness = 100;
            return 0;
    	}
	} else {
		vm_swappiness >>= 1;
		swap_no_progress = snp_threshold;
		return 1;
	}
}

static int lowmem_shrink(struct shrinker *s, struct shrink_control *sc)
{
	struct task_struct *p;
	struct task_struct *selected = NULL;
	int rem = 0;
	int tasksize;
	int i;
	int min_adj = OOM_ADJUST_MAX + 1;
	int selected_tasksize = 0;
	int selected_oom_adj;
	int array_size = ARRAY_SIZE(lowmem_adj);
	int other_free = global_page_state(NR_FREE_PAGES);
	int other_file = global_page_state(NR_FILE_PAGES) -
#ifdef ENABLE_ZRAM
                        total_swapcache_pages -
#endif
						global_page_state(NR_SHMEM);

	/*
	 * If we already have a death outstanding, then
	 * bail out right away; indicating to vmscan
	 * that we have nothing further to offer on
	 * this pass.
	 *
	 */
	if (lowmem_deathpending &&
	    time_before_eq(jiffies, lowmem_deathpending_timeout))
		return 0;

	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;
	if (lowmem_minfree_size < array_size)
		array_size = lowmem_minfree_size;
	for (i = 0; i < array_size; i++) {
		if (other_free < lowmem_minfree[i] &&
		    other_file < lowmem_minfree[i]) {
			min_adj = lowmem_adj[i];
			break;
		}
	}

	if (sc->nr_to_scan > 0)
		lowmem_print(3, "lowmem_shrink %lu, %x, ofree %d %d, ma %d\n",
			     sc->nr_to_scan, sc->gfp_mask, other_free, other_file,
			     min_adj);
	rem = global_page_state(NR_ACTIVE_ANON) +
		global_page_state(NR_ACTIVE_FILE) +
		global_page_state(NR_INACTIVE_ANON) +
		global_page_state(NR_INACTIVE_FILE);

	if (sc->nr_to_scan <= 0 || min_adj == OOM_ADJUST_MAX + 1) {
		lowmem_print(5, "lowmem_shrink %lu, %x, return %d\n",
			     sc->nr_to_scan, sc->gfp_mask, rem);
		return rem;
	}

	selected_oom_adj = min_adj;

#ifdef ENABLE_ZRAM
	if(likely(selected_oom_adj <= start_zram_oom_adj))
	{
  		if (!swap_guard_fn()) {
			lowmem_print(3, "lowmem_shrink tend to swap, vm_swappiness to %d\n", vm_swappiness);
	      	return rem;
		}
		lowmem_print(3, "lowmem_shrink tend to lowmemory killer, vm_swappiness to %d\n", vm_swappiness);
	}
#endif

	if(unlikely(selected_oom_adj <= start_dropcache_oom_adj))
	{
		drop_cache_android(1);
		lowmem_print(3, "lowmem_shrink drop_cache 1\n");
	}
	
	read_lock(&tasklist_lock);
	for_each_process(p) {
		struct mm_struct *mm;
		struct signal_struct *sig;
		int oom_adj;

		task_lock(p);
		mm = p->mm;
		sig = p->signal;
		if (!mm || !sig) {
			task_unlock(p);
			continue;
		}
		oom_adj = sig->oom_adj;
		if (oom_adj < min_adj) {
			task_unlock(p);
			continue;
		}
		tasksize = get_mm_rss(mm);
#ifdef ENABLE_ZRAM
        /* We need this to correct the actual memory usage for ZRAM Swap Device. Suppose the compression rate is 0.5. */
        tasksize += (get_mm_counter(mm, MM_SWAPENTS) >> 1);
#endif

		task_unlock(p);
		if (tasksize <= 0)
			continue;
		if (selected) {
			if (oom_adj < selected_oom_adj)
				continue;
			if (oom_adj == selected_oom_adj &&
			    tasksize <= selected_tasksize)
				continue;
		}
		selected = p;
		selected_tasksize = tasksize;
		selected_oom_adj = oom_adj;
		lowmem_print(2, "select %d (%s), adj %d, size %d, to kill\n",
			     p->pid, p->comm, oom_adj, tasksize);
	}
	if (selected) {

		lowmem_print(1, "send sigkill to %d (%s), adj %d, size %d\n",
			     selected->pid, selected->comm,
			     selected_oom_adj, selected_tasksize);
		lowmem_deathpending = selected;
		lowmem_deathpending_timeout = jiffies + HZ;
		force_sig(SIGKILL, selected);
		rem -= selected_tasksize;
	}
	lowmem_print(4, "lowmem_shrink %lu, %x, return %d\n",
		     sc->nr_to_scan, sc->gfp_mask, rem);
	read_unlock(&tasklist_lock);
	return rem;
}

static struct shrinker lowmem_shrinker = {
	.shrink = lowmem_shrink,
	.seeks = DEFAULT_SEEKS * 16
};

static int __init lowmem_init(void)
{
	task_free_register(&task_nb);
	register_shrinker(&lowmem_shrinker);
	return 0;
}

static void __exit lowmem_exit(void)
{
	unregister_shrinker(&lowmem_shrinker);
	task_free_unregister(&task_nb);
}

module_param_named(zram_adj, start_zram_oom_adj, int, S_IRUGO | S_IWUSR);
module_param_named(dropcache_adj, start_dropcache_oom_adj, int, S_IRUGO | S_IWUSR);
module_param_named(cost, lowmem_shrinker.seeks, int, S_IRUGO | S_IWUSR);
module_param_array_named(adj, lowmem_adj, int, &lowmem_adj_size,
			 S_IRUGO | S_IWUSR);
module_param_array_named(minfree, lowmem_minfree, uint, &lowmem_minfree_size,
			 S_IRUGO | S_IWUSR);
module_param_named(debug_level, lowmem_debug_level, uint, S_IRUGO | S_IWUSR);

module_init(lowmem_init);
module_exit(lowmem_exit);

MODULE_LICENSE("GPL");

