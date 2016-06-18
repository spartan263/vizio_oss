/*
 * Squashfs - a compressed read only filesystem for Linux
 *
 * Copyright (c) 2010 LG Electronics
 * Chan Jeong <chan.jeong@lge.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * lzo_wrapper.c
 */

#include <linux/mutex.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/lzo.h>

#include "squashfs_fs.h"
#include "squashfs_fs_sb.h"
#include "squashfs.h"
#include "decompressor.h"

#define USE_PARALLEL_LZO

struct squashfs_lzo {
	void	*input;
	void	*output;

#ifdef USE_PARALLEL_LZO
	// Data used by parallel unlzo
	
	// For buffer input
	int      devblksize;
	struct buffer_head **bh;
	int      bh_num;			// Number of bh.
	int      bh_index;                      // Next buffer to get data from.
	void    *buff;				// Point to next location to put new data.
	int      bytes;                         // Number of bytes left to read.
	int      offset;                        // Offset to read from next buffer.


	// For pages output
	int      out_bytes;			// Output bytes already writen.
	int	 out_left;			// Output bytes left.
	int	 page_num;			// Number of output page
	void   **out_buffer;			// Output buffer
	int	 out_index;			// Output page index.
#endif
};

static void *lzo_init(struct squashfs_sb_info *msblk, void *buff, int len)
{
	int block_size = max_t(int, msblk->block_size, SQUASHFS_METADATA_SIZE);

	struct squashfs_lzo *stream = kzalloc(sizeof(*stream), GFP_KERNEL);
	if (stream == NULL)
		goto failed;
	stream->input = vmalloc(block_size);
	if (stream->input == NULL)
		goto failed;
	stream->output = vmalloc(block_size);
	if (stream->output == NULL)
		goto failed2;

	return stream;

failed2:
	vfree(stream->input);
failed:
	ERROR("Failed to allocate lzo workspace\n");
	kfree(stream);
	return ERR_PTR(-ENOMEM);
}


static void lzo_free(void *strm)
{
	struct squashfs_lzo *stream = strm;

	if (stream) {
		vfree(stream->input);
		vfree(stream->output);
	}
	kfree(stream);
}

#ifdef USE_PARALLEL_LZO
static int lzo1x_decompress_fill(const unsigned char *in, size_t src_len, size_t *pin_len,
				int (*fill) (void *fill_data, size_t out_now),
				void *fill_data,
			   unsigned char *out, size_t *out_len);

static int lzo_get_one_buffer(struct squashfs_lzo *stream)
{
	int avail;
	int i = stream->bh_index;
	struct buffer_head *bh = stream->bh[i];

	if (i >= stream->bh_num)
		return 0;

	wait_on_buffer(bh);
	if (!buffer_uptodate(bh))
	{
		printk("Buffer not uptodate\n");
		return 0;
	}

	avail = min(stream->bytes, stream->devblksize - stream->offset);
	memcpy(stream->buff, bh->b_data + stream->offset, avail);	
	stream->buff += avail;
	stream->bytes -= avail;
	stream->offset = 0;
	put_bh(bh);

	stream->bh_index++;

	return avail;
}

static int lzo_fill_func(void *fill_data, size_t out_now)
{
	struct squashfs_lzo *stream=(struct squashfs_lzo *)fill_data;
	size_t avail;

	// See if we could write some data out. Only write full pages.
	while ((out_now - stream->out_bytes) > PAGE_CACHE_SIZE+4 && stream->out_left)
	{
		avail = min_t(int, stream->out_left, PAGE_CACHE_SIZE);
		memcpy(stream->out_buffer[stream->out_index], stream->output + stream->out_bytes, avail);
		stream->out_bytes += avail;
		stream->out_left -= avail;
		stream->out_index++;
	}

	// Get one buffer.
	avail = lzo_get_one_buffer(stream);
	if (avail <= 0)
		return 0;

	return avail;
}

static int lzo_uncompress(struct squashfs_sb_info *msblk, void **buffer,
	struct buffer_head **bh, int b, int offset, int length, int srclength,
	int pages)
{
	struct squashfs_lzo *stream = msblk->stream;
	void *buff = stream->input;
	int i, bytes = length, res;
	size_t out_len = srclength, avail;

	mutex_lock(&msblk->read_data_mutex);

	// Init parallel decomp data.
	stream->devblksize = msblk->devblksize;
	stream->bh = bh;
	stream->bh_num = b;
	stream->bh_index = 0;
	stream->buff = buff;
	stream->bytes = bytes;
	stream->offset = offset;

	stream->out_bytes = 0;
	stream->out_left = min_t(int, out_len, pages*PAGE_CACHE_SIZE);
	stream->page_num = pages;
	stream->out_buffer = buffer;
	stream->out_index = 0;

	// Wait on first buffer.
	avail = lzo_get_one_buffer(stream);
	if (!avail)
		goto block_release;

	// Lets start unlzo now.
	res = lzo1x_decompress_fill(stream->input, (size_t)length, &avail, 
			lzo_fill_func, stream, stream->output, &out_len);
	if (res != LZO_E_OK)
	{
		goto block_release;
	}

	// Some data is copied in fill func. Now, copy data left.
	res = bytes = (int)out_len;
	for (i = stream->out_index, buff = stream->output + stream->out_bytes; bytes && i < pages; i++) {
		avail = min_t(int, bytes, PAGE_CACHE_SIZE);
		memcpy(buffer[i], buff, avail);
		buff += avail;
		bytes -= avail;
	}

	mutex_unlock(&msblk->read_data_mutex);
	return res;

block_release:
	// Free all unused buffer.
	for (i=stream->bh_index; i < b; i++)
		put_bh(bh[i]);

	mutex_unlock(&msblk->read_data_mutex);

	ERROR("lzo decompression failed, data probably corrupt\n");
	return -EIO;
}

static inline int fetch_more_data(const unsigned char *ip, const unsigned char **ip_end, 
                                size_t *pin_len, size_t size_need, size_t out_now,
				int (*fill) (void *fill_data, size_t out_now),
				void *fill_data)
{
	int size_left = *ip_end - ip;
	int ret;

	while (size_left < size_need)
	{
		ret = fill(fill_data, out_now);
		if (ret <= 0)
                    return 1;

                size_left += ret;
                *ip_end += ret;
                *pin_len += ret;
	}

	return 0;
}

#include <asm/unaligned.h>

#define M2_MAX_OFFSET	0x0800

#define HAVE_IP(x, ip_end, ip) (((size_t)(ip_end - ip) < (x)) && \
		(((ipsrc_end - ip) < (x)) || fetch_more_data(ip, &ip_end, pin_len, x, op-out, fill, fill_data)))
#define HAVE_OP(x, op_end, op) ((size_t)(op_end - op) < (x))
#define HAVE_LB(m_pos, out, op) (m_pos < out || m_pos >= op)

#define COPY4(dst, src)	\
		put_unaligned(get_unaligned((const u32 *)(src)), (u32 *)(dst))

static int lzo1x_decompress_fill(const unsigned char *in, size_t src_len, size_t *pin_len,
				int (*fill) (void *fill_data, size_t out_now),
				void *fill_data,
			   unsigned char *out, size_t *out_len)
{
	const unsigned char * const ipsrc_end = in + src_len;
	const unsigned char * ip_end = in + *pin_len;
	unsigned char * const op_end = out + *out_len;
	const unsigned char *ip = in, *m_pos;
	unsigned char *op = out;
	size_t t;

	*out_len = 0;

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		if (HAVE_OP(t, op_end, op))
			goto output_overrun;
		if (HAVE_IP(t + 1, ip_end, ip))
			goto input_overrun;
		do {
			*op++ = *ip++;
		} while (--t > 0);
		goto first_literal_run;
	}

	while (!HAVE_IP(1, ip_end, ip)) {
		t = *ip++;
		if (t >= 16)
			goto match;
		if (t == 0) {
			if (HAVE_IP(1, ip_end, ip))
				goto input_overrun;
			while (*ip == 0) {
				t += 255;
				ip++;
				if (HAVE_IP(1, ip_end, ip))
					goto input_overrun;
			}
			t += 15 + *ip++;
		}
		if (HAVE_OP(t + 3, op_end, op))
			goto output_overrun;
		if (HAVE_IP(t + 4, ip_end, ip))
			goto input_overrun;

		COPY4(op, ip);
		op += 4;
		ip += 4;
		if (--t > 0) {
			if (t >= 4) {
				do {
					COPY4(op, ip);
					op += 4;
					ip += 4;
					t -= 4;
				} while (t >= 4);
				if (t > 0) {
					do {
						*op++ = *ip++;
					} while (--t > 0);
				}
			} else {
				do {
					*op++ = *ip++;
				} while (--t > 0);
			}
		}

first_literal_run:
		if (HAVE_IP(2, ip_end, ip))
			goto input_overrun;

		t = *ip++;
		if (t >= 16)
			goto match;
		m_pos = op - (1 + M2_MAX_OFFSET);
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;

		if (HAVE_LB(m_pos, out, op))
			goto lookbehind_overrun;

		if (HAVE_OP(3, op_end, op))
			goto output_overrun;
		*op++ = *m_pos++;
		*op++ = *m_pos++;
		*op++ = *m_pos;

		goto match_done;

		do {
match:
			if (t >= 64) {
				if (HAVE_IP(1, ip_end, ip))
					goto input_overrun;
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;
				if (HAVE_LB(m_pos, out, op))
					goto lookbehind_overrun;
				if (HAVE_OP(t + 3 - 1, op_end, op))
					goto output_overrun;
				goto copy_match;
			} else if (t >= 32) {
				t &= 31;
				if (t == 0) {
					if (HAVE_IP(1, ip_end, ip))
						goto input_overrun;
					while (*ip == 0) {
						t += 255;
						ip++;
						if (HAVE_IP(1, ip_end, ip))
							goto input_overrun;
					}
					t += 31 + *ip++;
				}
				if (HAVE_IP(2, ip_end, ip))
					goto input_overrun;
				m_pos = op - 1;
				m_pos -= get_unaligned_le16(ip) >> 2;
				ip += 2;
			} else if (t >= 16) {
				m_pos = op;
				m_pos -= (t & 8) << 11;

				t &= 7;
				if (t == 0) {
					if (HAVE_IP(1, ip_end, ip))
						goto input_overrun;
					while (*ip == 0) {
						t += 255;
						ip++;
						if (HAVE_IP(1, ip_end, ip))
							goto input_overrun;
					}
					t += 7 + *ip++;
				}
				if (HAVE_IP(2, ip_end, ip))
					goto input_overrun;
				m_pos -= get_unaligned_le16(ip) >> 2;
				ip += 2;
				if (m_pos == op)
					goto eof_found;
				m_pos -= 0x4000;
			} else {
				if (HAVE_IP(1, ip_end, ip))
					goto input_overrun;
				m_pos = op - 1;
				m_pos -= t >> 2;
				m_pos -= *ip++ << 2;

				if (HAVE_LB(m_pos, out, op))
					goto lookbehind_overrun;
				if (HAVE_OP(2, op_end, op))
					goto output_overrun;

				*op++ = *m_pos++;
				*op++ = *m_pos;
				goto match_done;
			}

			if (HAVE_LB(m_pos, out, op))
				goto lookbehind_overrun;
			if (HAVE_OP(t + 3 - 1, op_end, op))
				goto output_overrun;

			if (t >= 2 * 4 - (3 - 1) && (op - m_pos) >= 4) {
				COPY4(op, m_pos);
				op += 4;
				m_pos += 4;
				t -= 4 - (3 - 1);
				do {
					COPY4(op, m_pos);
					op += 4;
					m_pos += 4;
					t -= 4;
				} while (t >= 4);
				if (t > 0)
					do {
						*op++ = *m_pos++;
					} while (--t > 0);
			} else {
copy_match:
				*op++ = *m_pos++;
				*op++ = *m_pos++;
				do {
					*op++ = *m_pos++;
				} while (--t > 0);
			}
match_done:
			t = ip[-2] & 3;
			if (t == 0)
				break;
match_next:
			if (HAVE_OP(t, op_end, op))
				goto output_overrun;
			if (HAVE_IP(t + 1, ip_end, ip))
				goto input_overrun;

			*op++ = *ip++;
			if (t > 1) {
				*op++ = *ip++;
				if (t > 2)
					*op++ = *ip++;
			}

			t = *ip++;
		} while (!HAVE_IP(1, ip_end, ip));
	}

	*out_len = op - out;
	return LZO_E_EOF_NOT_FOUND;

eof_found:
	*out_len = op - out;
	return (ip == ipsrc_end ? LZO_E_OK :
		(ip < ipsrc_end ? LZO_E_INPUT_NOT_CONSUMED : LZO_E_INPUT_OVERRUN));
input_overrun:
	*out_len = op - out;
	return LZO_E_INPUT_OVERRUN;

output_overrun:
	*out_len = op - out;
	return LZO_E_OUTPUT_OVERRUN;

lookbehind_overrun:
	*out_len = op - out;
	return LZO_E_LOOKBEHIND_OVERRUN;
}

#else
static int lzo_uncompress(struct squashfs_sb_info *msblk, void **buffer,
	struct buffer_head **bh, int b, int offset, int length, int srclength,
	int pages)
{
	struct squashfs_lzo *stream = msblk->stream;
	void *buff = stream->input;
	int avail, i, bytes = length, res;
	size_t out_len = srclength;

	mutex_lock(&msblk->read_data_mutex);

	for (i = 0; i < b; i++) {
		wait_on_buffer(bh[i]);
		if (!buffer_uptodate(bh[i]))
			goto block_release;

		avail = min(bytes, msblk->devblksize - offset);
		memcpy(buff, bh[i]->b_data + offset, avail);
		buff += avail;
		bytes -= avail;
		offset = 0;
		put_bh(bh[i]);
	}

	res = lzo1x_decompress_safe(stream->input, (size_t)length,
					stream->output, &out_len);
	if (res != LZO_E_OK)
		goto failed;

	res = bytes = (int)out_len;
	for (i = 0, buff = stream->output; bytes && i < pages; i++) {
		avail = min_t(int, bytes, PAGE_CACHE_SIZE);
		memcpy(buffer[i], buff, avail);
		buff += avail;
		bytes -= avail;
	}

	mutex_unlock(&msblk->read_data_mutex);
	return res;

block_release:
	for (; i < b; i++)
		put_bh(bh[i]);

failed:
	mutex_unlock(&msblk->read_data_mutex);

	ERROR("lzo decompression failed, data probably corrupt\n");
	return -EIO;
}
#endif /* USE_PARALLEL_LZO */

const struct squashfs_decompressor squashfs_lzo_comp_ops = {
	.init = lzo_init,
	.free = lzo_free,
	.decompress = lzo_uncompress,
	.id = LZO_COMPRESSION,
	.name = "lzo",
	.supported = 1
};
