/*
 * xvmalloc memory allocator
 *
 * Copyright (C) 2008, 2009, 2010  Nitin Gupta
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 */

#ifndef _XV_MALLOC_H_
#define _XV_MALLOC_H_

#include <linux/types.h>

struct xv_pool;
struct xv_page;
enum km_type;

struct xv_pool *xv_create_pool(void);
struct xv_pool *xv_create_phys_pool(u32 phyaddr, u32 physize);
void xv_destroy_pool(struct xv_pool *pool);

int xv_malloc(struct xv_pool *pool, u32 size, struct xv_page **page,
			u32 *offset, gfp_t flags);
void xv_free(struct xv_pool *pool, struct xv_page *page, u32 offset);

u32 xv_get_object_size(void *obj);
u64 xv_get_total_size_bytes(struct xv_pool *pool);

// XV Page allocator/mapper
struct xv_page *xv_alloc_page(struct xv_pool *pool, gfp_t flags);
void xv_free_page(struct xv_pool *pool, struct xv_page *page);
void* xv_map(struct xv_page *page, enum km_type type);
void xv_unmap(struct xv_page *page, void *addr, enum km_type type);

#endif
