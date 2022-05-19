/*
 * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#include <common.h>
#include <linux/bug.h>
#include <linux/delay.h>

#include "flash_com.h"
#include "rkflash_debug.h"
#include "rk_sftl.h"
#include "sfc.h"
#include "sfc_nand.h"

static struct nand_info spi_nand_tbl[] = {
	/* TC58CVG0S0HxAIx */
	{0x98C2, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x02, 0xD8, 0x00, 18, 8, 0xFF, 0xFF, {4, 8, 0xff, 0xff}, NULL},
	/* TC58CVG1S0HxAIx */
	{0x98CB, 4, 64, 2, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x02, 0xD8, 0x00, 19, 8, 0xFF, 0xFF, {4, 8, 0xff, 0xff}, NULL},
	/* MX35LF1GE4AB */
	{0xC212, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 4, 0xB0, 0, {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* MX35LF2GE4AB */
	{0xC222, 4, 64, 2, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 19, 4, 0xB0, 0, {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* GD5F1GQ4UAYIG */
	{0xC8F1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 8, 0xB0, 0, {4, 8, 0xff, 0xff}, NULL},
	/* MT29F1G01ZAC */
	{0x2C12, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x00, 18, 1, 0xB0, 0, {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* GD5F2GQ40BY2GR */
	{0xC8D2, 4, 64, 2, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 19, 8, 0xB0, 0, {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp3},
	/* GD5F1GQ4RB9IGR */
	{0xC8D1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 8, 0xB0, 0, {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp3},
	/* IS37SML01G1 */
	{0xC821, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x00, 18, 1, 0xFF, 0xFF, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* W25N01GV */
	{0xEFAA, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 1, 0xFF, 0xFF, {4, 20, 36, 0xff}, &sfc_nand_ecc_status_sp1},
	/* HYF2GQ4UAACAE */
	{0xC952, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 14, 0xB0, 0, {4, 36, 0xff, 0xff}, NULL},
	/* HYF2GQ4UDACAE */
	{0xC922, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 4, 0xB0, 0, {4, 20, 0xff, 0xff}, NULL},
	/* HYF2GQ4UHCCAE */
	{0xC95A, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 14, 0xB0, 0, {4, 36, 0xff, 0xff}, NULL},
	/* HYF1GQ4UDACAE */
	{0xC921, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 4, 0xB0, 0, {4, 20, 0xff, 0xff}, NULL},
	/* F50L1G41LB */
	{0xC801, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 1, 0xFF, 0xFF, {20, 36, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* XT26G02A */
	{0x0BE2, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 1, 0xB0, 0x0, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp4},
	/* XT26G01A */
	{0x0BE1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 1, 0xB0, 0x0, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp4},
	/* FS35ND01G-S1 */
	{0xCDB1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 4, 0xB0, 0x0, {16, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp5},
	/* FS35ND02G-S2 */
	{0xCDA2, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x03, 0x02, 0xD8, 0x00, 19, 4, 0xFF, 0xFF, {16, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp5},
	/* DS35Q1GA-1B */
	{0xE571, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 4, 0xB0, 0x0, {4, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* DS35Q2GA-1B */
	{0xE572, 4, 64, 2, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 19, 4, 0xB0, 0x0, {4, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* EM73C044SNC-G */
	{0xD522, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 8, 0xB0, 0x0, {4, 20, 0xff, 0xff}, NULL},
	/* EM73D044SNB-G */
	{0xD520, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 19, 8, 0xB0, 0x0, {4, 20, 0xff, 0xff}, NULL},
	/* ATO25D1GA */
	{0x9B12, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x40, 18, 1, 0xB0, 0x0, {20, 36, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* XT26G02B */
	{0x0BF2, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 1, 0xB0, 0x0, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp5},
	/* XT26G01B */
	{0x0BF1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 1, 0xB0, 0x0, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp4},
	/* HYF4GQ4UAACBE */
	{0xC9D4, 8, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 20, 4, 0xB0, 0, {32, 64, 36, 68}, NULL},
	/* FM25S01 */
	{0xA1A1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 1, 0xB0, 0, {0, 4, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* HYF1GQ4UPACAE */
	{0xC9A1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 4, 0xB0, 0, {4, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp1},
	/* EM73E044SNA-G */
	{0xD503, 8, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 20, 8, 0xB0, 0, {4, 40, 8, 44}, NULL},
	/* GD5F2GQ5UEYIG */
	{0xC852, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 19, 4, 0xB0, 0, {4, 20, 0xff, 0xff}, &sfc_nand_ecc_status_sp2},
	/* GD5F1GQ4R */
	{0xC8C1, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 8, 0xB0, 0,  {4, 8, 0xff, 0xff}, &sfc_nand_ecc_status_sp3},
	/* TC58CVG2S0HRAIJ */
	{0x98ED, 8, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 20, 8, 0xFF, 0xFF, {4, 12, 8, 16}, NULL},
	/* TC58CVG1S3HRAIJ */
	{0x98EB, 4, 64, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 19, 8, 0xFF, 0xFF, {4, 8, 0xff, 0xff}, NULL},
	/* TC58CVG0S3HRAIJ */
	{0x98E2, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x0C, 18, 8, 0xFF, 0xFF, {4, 8, 0xff, 0xff}, NULL},
	/* XT26G04A */
	{0x0BE3, 4, 128, 1, 2048, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 20, 1, 0xB0, 0x0, {8, 12, 0xff, 0xff}, &sfc_nand_ecc_status_sp4},
	/* FS35ND01G-S1Y2 */
	{0xCDEA, 4, 64, 1, 1024, 0x13, 0x10, 0x03, 0x02, 0x6B, 0x32, 0xD8, 0x4C, 18, 4, 0xFF, 0xFF, {4, 8, 12, 16}, &sfc_nand_ecc_status_sp1},
};

static struct nand_info *p_nand_info;
static u32 gp_page_buf[SFC_NAND_PAGE_MAX_SIZE / 4];
static struct SFNAND_DEV sfc_nand_dev;

static struct nand_info *sfc_nand_get_info(u8 *nand_id)
{
	u32 i;
	u32 id = (nand_id[0] << 8) | (nand_id[1] << 0);

	for (i = 0; i < ARRAY_SIZE(spi_nand_tbl); i++) {
		if (spi_nand_tbl[i].id == id)
			return &spi_nand_tbl[i];
	}
	return NULL;
}

static int sfc_nand_write_en(void)
{
	int ret;
	struct rk_sfc_op op;

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = CMD_WRITE_EN;

	op.sfctrl.d32 = 0;

	ret = sfc_request(&op, 0, NULL, 0);
	return ret;
}

static int sfc_nand_rw_preset(void)
{
	int ret;
	struct rk_sfc_op op;
	u8 status = 0xFF;

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = 0;
	op.sfcmd.b.rw = SFC_WRITE;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.datalines = 2;

	ret = sfc_request(&op, 0, &status, 1);
	return ret;
}

static int sfc_nand_read_feature(u8 addr, u8 *data)
{
	int ret;
	struct rk_sfc_op op;

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = 0x0F;
	op.sfcmd.b.addrbits = SFC_ADDR_XBITS;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.addrbits = 8;

	*data = 0;

	ret = sfc_request(&op, addr, data, 1);
	if (ret != SFC_OK)
		return ret;
	return SFC_OK;
}

static int sfc_nand_write_feature(u32 addr, u8 status)
{
	int ret;
	struct rk_sfc_op op;

	sfc_nand_write_en();

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = 0x1F;
	op.sfcmd.b.addrbits = SFC_ADDR_XBITS;
	op.sfcmd.b.rw = SFC_WRITE;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.addrbits = 8;

	ret = sfc_request(&op, addr, &status, 1);
	if (ret != SFC_OK)
		return ret;
	return ret;
}

static int sfc_nand_wait_busy(u8 *data, int timeout)
{
	int ret;
	int i;
	u8 status;

	*data = 0;
	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return ret;
		*data = status;
		if (!(status & (1 << 0)))
			return SFC_OK;
		sfc_delay(1);
	}
	return -1;
}

/*
 * ecc default:
 * ecc bits: 0xC0[4,5]
 * 0x00, No bit errors were detected
 * 0x01, Bit errors were detected and corrected.
 * 0x10, Multiple bit errors were detected and not corrected.
 * 0x11, Bits errors were detected and corrected, bit error count
 *	exceed the bit flip detection threshold
 */
static u32 sfc_nand_ecc_status(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 4) & 0x03;

	if (ecc <= 1)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 2)
		ret = (u32)SFC_NAND_ECC_ERROR;
	else
		ret = SFC_NAND_ECC_REFRESH;

	return ret;
}

/*
 * ecc spectial type1:
 * ecc bits: 0xC0[4,5]
 * 0x00, No bit errors were detected;
 * 0x01, Bits errors were detected and corrected, bit error count
 *	may reach the bit flip detection threshold;
 * 0x10, Multiple bit errors were detected and not corrected;
 * 0x11, Reserved.
 */
u32 sfc_nand_ecc_status_sp1(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 4) & 0x03;

	if (ecc == 0)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 1)
		ret = SFC_NAND_ECC_REFRESH;
	else
		ret = (u32)SFC_NAND_ECC_ERROR;

	return ret;
}

/*
 * ecc spectial type2:
 * ecc bits: 0xC0[4,5] 0xF0[4,5]
 * [0x0000, 0x0011], No bit errors were detected;
 * [0x0100, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000, 0x1011], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100, 0x1111], reserved.
 */
u32 sfc_nand_ecc_status_sp2(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status, status1;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		ret = sfc_nand_read_feature(0xF0, &status1);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 4) & 0x03;
	ecc = (ecc << 2) | ((status1 >> 4) & 0x03);
	if (ecc < 7)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 7)
		ret = SFC_NAND_ECC_REFRESH;
	else
		ret = (u32)SFC_NAND_ECC_ERROR;

	return ret;
}

/*
 * ecc spectial type3:
 * ecc bits: 0xC0[4,5] 0xF0[4,5]
 * [0x0000, 0x0011], No bit errors were detected;
 * [0x0100, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000, 0x1011], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100, 0x1111], Bit error count equals the bit flip
 *	detectio nthreshold
 */
u32 sfc_nand_ecc_status_sp3(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status, status1;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		ret = sfc_nand_read_feature(0xF0, &status1);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 4) & 0x03;
	ecc = (ecc << 2) | ((status1 >> 4) & 0x03);
	if (ecc < 7)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 7 || ecc >= 12)
		ret = SFC_NAND_ECC_REFRESH;
	else
		ret = (u32)SFC_NAND_ECC_ERROR;

	return ret;
}

/*
 * ecc spectial type4:
 * ecc bits: 0xC0[2,5]
 * [0x0000], No bit errors were detected;
 * [0x0001, 0x0111], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x1000], Multiple bit errors were detected and
 *	not corrected.
 * [0x1100], Bit error count equals the bit flip
 *	detectionthreshold
 * else, reserved
 */
u32 sfc_nand_ecc_status_sp4(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 2) & 0x0f;
	if (ecc < 7)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 7 || ecc == 12)
		ret = SFC_NAND_ECC_REFRESH;
	else
		ret = (u32)SFC_NAND_ECC_ERROR;

	return ret;
}

/*
 * ecc spectial type5:
 * ecc bits: 0xC0[4,6]
 * [0x0], No bit errors were detected;
 * [0x001, 0x011], Bit errors were detected and corrected. Not
 *	reach Flipping Bits;
 * [0x100], Bit error count equals the bit flip
 *	detectionthreshold
 * [0x101, 0x110], Reserved;
 * [0x111], Multiple bit errors were detected and
 *	not corrected.
 */
u32 sfc_nand_ecc_status_sp5(void)
{
	u32 ret;
	u32 i;
	u8 ecc;
	u8 status;
	u32 timeout = 1000 * 1000;

	for (i = 0; i < timeout; i++) {
		ret = sfc_nand_read_feature(0xC0, &status);
		if (ret != SFC_OK)
			return SFC_NAND_ECC_ERROR;
		if (!(status & (1 << 0)))
			break;
		sfc_delay(1);
	}

	ecc = (status >> 4) & 0x07;
	if (ecc < 4)
		ret = SFC_NAND_ECC_OK;
	else if (ecc == 4)
		ret = SFC_NAND_ECC_REFRESH;
	else
		ret = (u32)SFC_NAND_ECC_ERROR;

	return ret;
}

u32 sfc_nand_erase_block(u8 cs, u32 addr)
{
	int ret;
	struct rk_sfc_op op;
	u8 status;

	rkflash_print_dio("%s %x\n", __func__, addr);
	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = p_nand_info->block_erase_cmd;
	op.sfcmd.b.addrbits = SFC_ADDR_24BITS;
	op.sfcmd.b.rw = SFC_WRITE;

	op.sfctrl.d32 = 0;

	sfc_nand_write_en();
	ret = sfc_request(&op, addr, NULL, 0);
	if (ret != SFC_OK)
		return ret;
	ret = sfc_nand_wait_busy(&status, 1000 * 1000);
	if (status & (1 << 2))
		return SFC_NAND_PROG_ERASE_ERROR;

	return ret;
}

static u32 sfc_nand_prog_page_raw(u8 cs, u32 addr, u32 *p_page_buf)
{
	int ret;
	u32 plane;
	struct rk_sfc_op op;
	u8 status;
	u32 page_size = SFC_NAND_SECTOR_FULL_SIZE * p_nand_info->sec_per_page;

	rkflash_print_dio("%s %x %x\n", __func__, addr, p_page_buf[0]);
	sfc_nand_write_en();
	if (sfc_nand_dev.prog_lines == DATA_LINES_X4 &&
	    p_nand_info->feature & FEA_SOFT_QOP_BIT &&
	    sfc_get_version() < SFC_VER_3)
		sfc_nand_rw_preset();

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = sfc_nand_dev.page_prog_cmd;
	op.sfcmd.b.addrbits = SFC_ADDR_XBITS;
	op.sfcmd.b.rw = SFC_WRITE;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.datalines = sfc_nand_dev.prog_lines;
	op.sfctrl.b.addrbits = 16;
	plane = p_nand_info->plane_per_die == 2 ? ((addr >> 6) & 0x1) << 12 : 0;
	sfc_request(&op, plane, p_page_buf, page_size);

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = p_nand_info->page_prog_cmd;
	op.sfcmd.b.addrbits = SFC_ADDR_24BITS;
	op.sfcmd.b.rw = SFC_WRITE;

	op.sfctrl.d32 = 0;
	ret = sfc_request(&op, addr, p_page_buf, 0);
	if (ret != SFC_OK)
		return ret;
	ret = sfc_nand_wait_busy(&status, 1000 * 1000);
	if (status & (1 << 3))
		return SFC_NAND_PROG_ERASE_ERROR;

	return ret;
}

u32 sfc_nand_prog_page(u8 cs, u32 addr, u32 *p_data, u32 *p_spare)
{
	int ret;
	u32 sec_per_page = p_nand_info->sec_per_page;
	u32 data_size = sec_per_page * SFC_NAND_SECTOR_SIZE;
	struct nand_mega_area *meta = &p_nand_info->meta;

	memcpy(gp_page_buf, p_data, data_size);
	memset(&gp_page_buf[data_size / 4], 0xff, sec_per_page * 16);
	gp_page_buf[(data_size + meta->off0) / 4] = p_spare[0];
	gp_page_buf[(data_size + meta->off1) / 4] = p_spare[1];
	if (sec_per_page == 8) {
		gp_page_buf[(data_size + meta->off2) / 4] = p_spare[2];
		gp_page_buf[(data_size + meta->off3) / 4] = p_spare[3];
	}
	ret = sfc_nand_prog_page_raw(cs, addr, gp_page_buf);

	return ret;
}

static u32 sfc_nand_read_page_raw(u8 cs, u32 addr, u32 *p_page_buf)
{
	int ret;
	u32 plane;
	struct rk_sfc_op op;
	u32 ecc_result;
	u32 page_size = SFC_NAND_SECTOR_FULL_SIZE * p_nand_info->sec_per_page;
	u8 status;

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = p_nand_info->page_read_cmd;
	op.sfcmd.b.rw = SFC_WRITE;
	op.sfcmd.b.addrbits = SFC_ADDR_24BITS;

	op.sfctrl.d32 = 0;

	sfc_request(&op, addr, p_page_buf, 0);
	if (sfc_nand_dev.read_lines == DATA_LINES_X4 &&
	    p_nand_info->feature & FEA_SOFT_QOP_BIT &&
	    sfc_get_version() < SFC_VER_3)
		sfc_nand_rw_preset();

	sfc_nand_wait_busy(&status, 1000 * 1000);
	if (p_nand_info->ecc_status)
		ecc_result = p_nand_info->ecc_status();
	else
		ecc_result = sfc_nand_ecc_status();

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = sfc_nand_dev.page_read_cmd;
	op.sfcmd.b.addrbits = SFC_ADDR_24BITS;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.datalines = sfc_nand_dev.read_lines;

	plane = p_nand_info->plane_per_die == 2 ? ((addr >> 6) & 0x1) << 12 : 0;
	ret = sfc_request(&op, plane << 8, p_page_buf, page_size);
	rkflash_print_dio("%s %x %x\n", __func__, addr, p_page_buf[0]);

	if (ret != SFC_OK)
		return SFC_NAND_ECC_ERROR;

	return ecc_result;
}

u32 sfc_nand_read_page(u8 cs, u32 addr, u32 *p_data, u32 *p_spare)
{
	u32 ret;
	u32 sec_per_page = p_nand_info->sec_per_page;
	u32 data_size = sec_per_page * SFC_NAND_SECTOR_SIZE;
	struct nand_mega_area *meta = &p_nand_info->meta;

	ret = sfc_nand_read_page_raw(cs, addr, gp_page_buf);
	memcpy(p_data, gp_page_buf, data_size);
	p_spare[0] = gp_page_buf[(data_size + meta->off0) / 4];
	p_spare[1] = gp_page_buf[(data_size + meta->off1) / 4];
	if (p_nand_info->sec_per_page == 8) {
		p_spare[2] = gp_page_buf[(data_size + meta->off2) / 4];
		p_spare[3] = gp_page_buf[(data_size + meta->off3) / 4];
	}

	if (ret != SFC_NAND_ECC_OK) {
		rkflash_print_error("%s[0x%x], ret=0x%x\n", __func__, addr, ret);
		if (p_data)
			rkflash_print_hex("data:", p_data, 4, 8);
		if (p_spare)
			rkflash_print_hex("spare:", p_spare, 4, 2);
	}

	return ret;
}

u32 sfc_nand_check_bad_block(u8 cs, u32 addr)
{
	u32 ret;
	u32 data_size = p_nand_info->sec_per_page * SFC_NAND_SECTOR_SIZE;

	ret = sfc_nand_read_page_raw(cs, addr, gp_page_buf);
	if (ret == SFC_NAND_ECC_ERROR)
		return true;
	/* Original bad block */
	if ((gp_page_buf[data_size / 4] & 0xFF) != 0xFF)
		return true;

	return false;
}

u32 sfc_nand_mark_bad_block(u8 cs, u32 addr)
{
	u32 ret;
	u32 data_size = p_nand_info->sec_per_page * SFC_NAND_SECTOR_SIZE;

	ret = sfc_nand_read_page_raw(cs, addr, gp_page_buf);
	if (ret)
		return SFC_NAND_HW_ERROR;
	gp_page_buf[data_size / 4] = 0x0;
	ret = sfc_nand_prog_page_raw(cs, addr, gp_page_buf);
	if (ret)
		return SFC_NAND_HW_ERROR;

	return ret;
}

int sfc_nand_read_id(u8 *data)
{
	int ret;
	struct rk_sfc_op op;

	op.sfcmd.d32 = 0;
	op.sfcmd.b.cmd = CMD_READ_JEDECID;
	op.sfcmd.b.addrbits = SFC_ADDR_XBITS;

	op.sfctrl.d32 = 0;
	op.sfctrl.b.addrbits = 8;

	ret = sfc_request(&op, 0, data, 3);

	return ret;
}

/*
 * Read the 1st page's 1st byte of a phy_blk
 * If not FF, it's bad blk
 */
static int sfc_nand_get_bad_block_list(u16 *table, u32 die)
{
	u16 blk;
	u32 bad_cnt, page;
	u32 blk_per_die;
	u32 *pread;
	u32 *pspare_read;

	rkflash_print_info("%s\n", __func__);
	pread = ftl_malloc(SFC_NAND_PAGE_MAX_SIZE);
	pspare_read = ftl_malloc(8);
	if (!pread || !pspare_read) {
		kfree(pread);
		kfree(pspare_read);
		return -1;
	}

	bad_cnt = 0;
	blk_per_die = p_nand_info->plane_per_die *
			p_nand_info->blk_per_plane;
	for (blk = 0; blk < blk_per_die; blk++) {
		page = (blk + blk_per_die * die) *
			p_nand_info->page_per_blk;
		sfc_nand_read_page(0, page, pread, pspare_read);

		if (pread[0] != 0xFFFFFFFF ||
		    pspare_read[0] != 0xFFFFFFFF) {
			table[bad_cnt++] = blk;
			rkflash_print_error("die[%d], bad_blk[%d]\n", die, blk);
		}
	}
	ftl_free(pread);
	ftl_free(pspare_read);

	return (int)bad_cnt;
}

void sfc_nand_ftl_ops_init(void)
{
	/* para init */
	g_nand_phy_info.nand_type	= 1;
	g_nand_phy_info.die_num		= 1;
	g_nand_phy_info.plane_per_die	= p_nand_info->plane_per_die;
	g_nand_phy_info.blk_per_plane	= p_nand_info->blk_per_plane;
	g_nand_phy_info.page_per_blk	= p_nand_info->page_per_blk;
	g_nand_phy_info.page_per_slc_blk = p_nand_info->page_per_blk;
	g_nand_phy_info.byte_per_sec	= SFC_NAND_SECTOR_SIZE;
	g_nand_phy_info.sec_per_page	= p_nand_info->sec_per_page;
	g_nand_phy_info.sec_per_blk	= p_nand_info->sec_per_page *
					  p_nand_info->page_per_blk;
	g_nand_phy_info.reserved_blk	= 8;
	g_nand_phy_info.blk_per_die	= p_nand_info->plane_per_die *
					  p_nand_info->blk_per_plane;
	g_nand_phy_info.ecc_bits	= p_nand_info->max_ecc_bits;

	/* driver register */
	g_nand_ops.get_bad_blk_list	= sfc_nand_get_bad_block_list;
	g_nand_ops.erase_blk		= sfc_nand_erase_block;
	g_nand_ops.prog_page		= sfc_nand_prog_page;
	g_nand_ops.read_page		= sfc_nand_read_page;
	g_nand_ops.bch_sel		= NULL;
}

static int sfc_nand_enable_QE(void)
{
	int ret = SFC_OK;
	u8 status;
	int bit_offset = p_nand_info->QE_bits;

	if (bit_offset == 0xFF)
		return SFC_OK;

	ret = sfc_nand_read_feature(p_nand_info->QE_address, &status);
	if (ret != SFC_OK)
		return ret;

	if (status & (1 << bit_offset))   /* is QE bit set */
		return SFC_OK;

	status |= (1 << bit_offset);
		return sfc_nand_write_feature(p_nand_info->QE_address, status);

	return ret;
}

u32 sfc_nand_init(void)
{
	u8 status, id_byte[8];

	sfc_nand_read_id(id_byte);
	rkflash_print_error("sfc_nand id: %x %x %x\n",
			    id_byte[0], id_byte[1], id_byte[2]);
	if (id_byte[0] == 0xFF || id_byte[0] == 0x00)
		return (u32)FTL_NO_FLASH;

	p_nand_info = sfc_nand_get_info(id_byte);
	if (!p_nand_info)
		return (u32)FTL_UNSUPPORTED_FLASH;

	sfc_nand_dev.manufacturer = id_byte[0];
	sfc_nand_dev.mem_type = id_byte[1];
	sfc_nand_dev.capacity = p_nand_info->density;
	sfc_nand_dev.block_size = p_nand_info->page_per_blk * p_nand_info->sec_per_page;
	sfc_nand_dev.page_size = p_nand_info->sec_per_page;

	/* disable block lock */
	sfc_nand_write_feature(0xA0, 0);
	sfc_nand_dev.read_lines = DATA_LINES_X1;
	sfc_nand_dev.prog_lines = DATA_LINES_X1;
	sfc_nand_dev.page_read_cmd = p_nand_info->read_cache_cmd_1;
	sfc_nand_dev.page_prog_cmd = p_nand_info->prog_cache_cmd_1;
	if (p_nand_info->feature & FEA_4BIT_READ) {
		if (sfc_nand_enable_QE() == SFC_OK) {
			sfc_nand_dev.read_lines = DATA_LINES_X4;
			sfc_nand_dev.page_read_cmd =
				p_nand_info->read_cache_cmd_4;
		}
	}

	if (p_nand_info->feature & FEA_4BIT_PROG &&
	    sfc_nand_dev.read_lines == DATA_LINES_X4) {
		sfc_nand_dev.prog_lines = DATA_LINES_X4;
		sfc_nand_dev.page_prog_cmd = p_nand_info->prog_cache_cmd_4;
	}

	sfc_nand_read_feature(0xA0, &status);
	rkflash_print_info("sfc_nand A0 = 0x%x\n", status);
	sfc_nand_read_feature(0xB0, &status);
	rkflash_print_info("sfc_nand B0 = 0x%x\n", status);
	rkflash_print_info("read_lines = %x\n", sfc_nand_dev.read_lines);
	rkflash_print_info("prog_lines = %x\n", sfc_nand_dev.prog_lines);
	rkflash_print_info("page_read_cmd = %x\n", sfc_nand_dev.page_read_cmd);
	rkflash_print_info("page_prog_cmd = %x\n", sfc_nand_dev.page_prog_cmd);

	return SFC_OK;
}

void sfc_nand_deinit(void)
{
	/* to-do */
}

struct SFNAND_DEV *sfc_nand_get_private_dev(void)
{
	return &sfc_nand_dev;
}
