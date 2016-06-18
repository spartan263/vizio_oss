/*
 * (c) 1997-2010 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef _NFSB_H
#define _NFSB_H

#include <crypto/mpuint.h>
#include <linux/types.h>

#define NFSB_USE_RSA2048 0

#if NFSB_USE_RSA2048
    #define NFSB_RSAKEY_LEN  256
#else
    #define NFSB_RSAKEY_LEN  128
#endif

/**
 * A Netflix Secure Boot Header (NFSB) contains the dm-verity
 * information for a file system image. The header contents can be
 * used to establish trust of the file system data and is signed by an
 * RSA-2048 private key trusted by the kernel.
 */
#pragma pack(push)
#pragma pack(1)
struct nfsb_header {
	char magic[4];
	uint8_t version;
	uint32_t fs_offset;      /* file system image offset in 512B sectors (big-endian) */
	uint32_t fs_size;        /* file system image size in 512B sectors (big-endian) */
	uint8_t hash_depth;      /* verity bht depth */
	char hash_algo[16];      /* verity bht hash algorithm */
	uint32_t hash_size;      /* hash image size in 512B sectors (big-endian) */
	uint32_t hash_sectors;   /* sectors verified per verity hash */
	char verity_hash[65];    /* SHA-256 hash in hexadecimal + NULL */
	uint8_t rsa_sig[NFSB_RSAKEY_LEN];    /* RSA-2048/1024 signature */
};
#pragma pack(pop)

/**
 * Returns the file system image offset in bytes, relative to the
 * start of the header.
 *
 * @return the file system image offset in bytes.
 */
ssize_t nfsb_fs_offset(const struct nfsb_header *h);

/**
 * Returns the file system image size in bytes. This may be larger
 * than the original file system image size passed into nfsb_header()
 * since it is rounded up to the nearest page.
 *
 * @return the file system image size in bytes, rounded up to the
 *         nearest page.
 */
ssize_t nfsb_fs_size(const struct nfsb_header *h);

/**
 * Returns the hash image size in bytes. This may be larger than the
 * original file system image size passed into nfsb_header() since it
 * is rounded up to the nearest page.
 *
 * @return the hash image size in bytes, rounded up to the nearest
 *         page.
 */
ssize_t nfsb_hash_size(const struct nfsb_header *h);
 
/**
 * Returns the dm-verity bht depth.
 *
 * @return the dm-verity bht depth.
 */
uint8_t nfsb_hash_depth(const struct nfsb_header *h);

/**
 * Returns the dm-verity bht hash algorithm.
 *
 * @return the dm-verity bht hash algorithm.
 */
const char *nfsb_hash_algo(const struct nfsb_header *h);

/**
 * Returns the number of sectors verified by each hash.
 *
 * @return the number of sectors verified by each hash.
 */
uint32_t nfsb_hash_sectors(const struct nfsb_header *h);

/**
 * Returns the dm-verity target SHA-256 hash as a hexadecimal string
 * (64 characters + a NULL terminator).
 *
 * @return verity target hash.
 */
const char *nfsb_verity_hash(const struct nfsb_header *h);

/**
 * Verify a NFSB header with a RSA-2048 public key.
 *
 * @param[in] h header to verify.
 * @param[in] key RSA-2048 public key.
 * @param[in] exp RSA public modulus.
 * @return 0 if the header verification succeeds.
 */
int nfsb_verify(const struct nfsb_header *h, const mpuint *key, const mpuint *n);

/**
 * Read a NFSB header in portable form.
 *
 * @param[in] fd read file descriptor.
 * @param[in] h header struct to read into.
 * @return number of bytes written or -1 if an error occurred; errno
 *         will be set.
 */
ssize_t nfsb_read(int fd, struct nfsb_header *h);

/**
 * Read a NFSB header for non-rootfs image.
 *
 * @param[in] fp read file descriptor.
 * @param[in] h header struct to read into.
 * @return number of bytes written or -1 if an error occurred; errno
 *         will be set.
 */
ssize_t nfsb_nonroot_read(struct file *fp, struct nfsb_header *h);
	
/**
 * Check non-rootfs NFSB header.
 *
 * @param[in] h header struct to check.
 * @param[in] devname read file device name.
 * @return 1 if successful
 */
int dm_check_nonroot_nfsb(struct nfsb_header *h, char *devname);

#endif