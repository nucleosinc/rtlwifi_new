/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8822B_SUPPORT == 1)

void phydm_dynamic_switch_htstf_mumimo_8822b(struct PHY_DM_STRUCT *p_dm)
{
	u8 rssi_l2h = 40, rssi_h2l = 35;

	/*if Pin > -60dBm, enable HT-STF gain controller, otherwise, if rssi < -65dBm, disable the controller*/

	if (p_dm->rssi_min >= rssi_l2h)
		odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(17), 0x1);
	else if (p_dm->rssi_min < rssi_h2l)
		odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(17), 0x0);
}

void phydm_dynamic_parameters_ota(struct PHY_DM_STRUCT *p_dm)
{
	u8 rssi_l2h = 40, rssi_h2l = 35;

	if ((*p_dm->p_channel <= 14) &&
	    (*p_dm->p_band_width == CHANNEL_WIDTH_20)) {
		if (p_dm->rssi_min >= rssi_l2h) {
			/*if (p_dm->bhtstfdisabled == false)*/
			odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(17), 0x1);

			odm_set_bb_reg(p_dm, BBREG_0x98c, 0x7fc0000, 0x0);
			odm_set_bb_reg(p_dm, BBREG_0x818, 0x7000000, 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc04, BIT(18), 0x0);
			odm_set_bb_reg(p_dm, BBREG_0xe04, BIT(18), 0x0);
			if (p_dm->p_advance_ota & PHYDM_HP_OTA_SETTING_A) {
				odm_set_bb_reg(p_dm, BBREG_0x19d8, MASKDWORD,
					       0x444);
				odm_set_bb_reg(p_dm, BBREG_0x19d4, MASKDWORD,
					       0x4444aaaa);
			} else if (p_dm->p_advance_ota &
				   PHYDM_HP_OTA_SETTING_B) {
				odm_set_bb_reg(p_dm, BBREG_0x19d8, MASKDWORD,
					       0x444);
				odm_set_bb_reg(p_dm, BBREG_0x19d4, MASKDWORD,
					       0x444444aa);
			}
		} else if (p_dm->rssi_min < rssi_h2l) {
			/*if (p_dm->bhtstfdisabled == true)*/
			odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(17), 0x0);

			odm_set_bb_reg(p_dm, BBREG_0x98c, MASKDWORD,
				       0x43440000);
			odm_set_bb_reg(p_dm, BBREG_0x818, 0x7000000, 0x4);
			odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(18) | BIT(21)),
				       0x0);
			odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(18) | BIT(21)),
				       0x0);
			odm_set_bb_reg(p_dm, BBREG_0x19d8, MASKDWORD, 0xaaa);
			odm_set_bb_reg(p_dm, BBREG_0x19d4, MASKDWORD,
				       0xaaaaaaaa);
		}
	} else {
		//odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(17), 0x0);
		odm_set_bb_reg(p_dm, BBREG_0x98c, MASKDWORD, 0x43440000);
		odm_set_bb_reg(p_dm, BBREG_0x818, 0x7000000, 0x4);
		odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(18) | BIT(21)), 0x0);
		odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(18) | BIT(21)), 0x0);
		odm_set_bb_reg(p_dm, BBREG_0x19d8, MASKDWORD, 0xaaa);
		odm_set_bb_reg(p_dm, BBREG_0x19d4, MASKDWORD, 0xaaaaaaaa);
	}
}

static void _set_tx_a_cali_value(struct PHY_DM_STRUCT *p_dm,
				 enum rf_path rf_path, u8 offset,
				 u8 tx_a_bias_offset)
{
	u32 modi_tx_a_value = 0;
	u8 tmp1_byte = 0;
	boolean is_minus = false;
	u8 comp_value = 0;

	switch (offset) {
	case 0x0:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X10124);
		break;
	case 0x1:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X10524);
		break;
	case 0x2:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X10924);
		break;
	case 0x3:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X10D24);
		break;
	case 0x4:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X30164);
		break;
	case 0x5:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X30564);
		break;
	case 0x6:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X30964);
		break;
	case 0x7:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X30D64);
		break;
	case 0x8:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X50195);
		break;
	case 0x9:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X50595);
		break;
	case 0xa:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X50995);
		break;
	case 0xb:
		odm_set_rf_reg(p_dm, rf_path, RFREG_0x18, 0xFFFFF, 0X50D95);
		break;
	default:
		PHYDM_DBG(p_dm, ODM_COMP_API, ("Invalid TxA band offset...\n"));
		return;
		break;
	}

	/* Get TxA value */
	modi_tx_a_value = odm_get_rf_reg(p_dm, rf_path, RFREG_0x61, 0xFFFFF);
	tmp1_byte = (u8)modi_tx_a_value & (BIT(3) | BIT(2) | BIT(1) | BIT(0));

	/* check how much need to calibration */
	switch (tx_a_bias_offset) {
	case 0xF6:
		is_minus = true;
		comp_value = 3;
		break;

	case 0xF4:
		is_minus = true;
		comp_value = 2;
		break;

	case 0xF2:
		is_minus = true;
		comp_value = 1;
		break;

	case 0xF3:
		is_minus = false;
		comp_value = 1;
		break;

	case 0xF5:
		is_minus = false;
		comp_value = 2;
		break;

	case 0xF7:
		is_minus = false;
		comp_value = 3;
		break;

	case 0xF9:
		is_minus = false;
		comp_value = 4;
		break;

	/* do nothing case */
	case 0xF0:
	default:
		PHYDM_DBG(p_dm, ODM_COMP_API,
			  ("No need to do TxA bias current calibration\n"));
		return;
		break;
	}

	/* calc correct value to calibrate */
	if (is_minus) {
		if (tmp1_byte >= comp_value) {
			tmp1_byte -= comp_value;
			//modi_tx_a_value += tmp1_byte;
		} else {
			tmp1_byte = 0;
		}
	} else {
		tmp1_byte += comp_value;
		if (tmp1_byte >= 7) {
			tmp1_byte = 7;
		}
	}

	/* Write back to RF reg */
	odm_set_rf_reg(p_dm, rf_path, RFREG_0x30, 0xFFFF,
		       (offset << 12 | (modi_tx_a_value & 0xff0) | tmp1_byte));
}

static void _txa_bias_cali_4_each_path(struct PHY_DM_STRUCT *p_dm, u8 rf_path,
				       u8 efuse_value)
{
	/* switch on set TxA bias */
	odm_set_rf_reg(p_dm, rf_path, RFREG_0xef, 0xFFFFF, 0x200);

	/* Set 12 sets of TxA value */
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x0, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x1, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x2, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x3, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x4, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x5, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x6, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x7, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x8, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0x9, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0xa, efuse_value);
	_set_tx_a_cali_value(p_dm, (enum rf_path)rf_path, 0xb, efuse_value);

	// switch off set TxA bias
	odm_set_rf_reg(p_dm, rf_path, RFREG_0xef, 0xFFFFF, 0x0);
}

/* for 8822B PCIE D-cut patch only */
/* Normal driver and MP driver need this patch */

void phydm_txcurrentcalibration(struct PHY_DM_STRUCT *p_dm)
{
	u8 efuse0x3D8, efuse0x3D7;
	u32 orig_rf0x18_path_a = 0, orig_rf0x18_path_b = 0;

	if (!(p_dm->support_ic_type & ODM_RTL8822B))
		return;

	PHYDM_DBG(
		p_dm, ODM_COMP_MP,
		("8822b 5g tx current calibration BBREG_0x3d7=0x%X BBREG_0x3d8=0x%X\n",
		 p_dm->efuse0x3d7, p_dm->efuse0x3d8));

	/* save original 0x18 value */
	orig_rf0x18_path_a =
		odm_get_rf_reg(p_dm, RF_PATH_A, RFREG_0x18, 0xFFFFF);
	orig_rf0x18_path_b =
		odm_get_rf_reg(p_dm, RF_PATH_B, RFREG_0x18, 0xFFFFF);

	/* define efuse content */
	efuse0x3D8 = p_dm->efuse0x3d8;
	efuse0x3D7 = p_dm->efuse0x3d7;

	/* check efuse content to judge whether need to calibration or not */
	if (0xFF == efuse0x3D7) {
		PHYDM_DBG(
			p_dm, ODM_COMP_MP,
			("efuse content BBREG_0x3d7 == 0xFF, No need to do TxA cali\n"));
		return;
	}

	/* write RF register for calibration */
	_txa_bias_cali_4_each_path(p_dm, RF_PATH_A, efuse0x3D7);
	_txa_bias_cali_4_each_path(p_dm, RF_PATH_B, efuse0x3D8);

	/* restore original 0x18 value */
	odm_set_rf_reg(p_dm, RF_PATH_A, RFREG_0x18, 0xFFFFF,
		       orig_rf0x18_path_a);
	odm_set_rf_reg(p_dm, RF_PATH_B, RFREG_0x18, 0xFFFFF,
		       orig_rf0x18_path_b);
}

void phydm_1rcca_setting(struct PHY_DM_STRUCT *p_dm, boolean enable_1rcca)
{
	u32 reg_32;

	reg_32 = odm_get_bb_reg(p_dm, BBREG_0xa04, 0x0f000000);

	/* Enable or disable 1RCCA setting accrodding to the control from driver */
	if (enable_1rcca == true) {
		if (reg_32 == 0x0) {
			odm_set_bb_reg(p_dm, BBREG_0x808, MASKBYTE0,
				       0x13); /* CCK path-a */
		} else if (reg_32 == 0x5) {
			odm_set_bb_reg(p_dm, BBREG_0x808, MASKBYTE0,
				       0x23); /* CCK path-b */
		}
	} else {
		odm_set_bb_reg(p_dm, BBREG_0x808, MASKBYTE0,
			       0x33); /* disable 1RCCA */
		odm_set_bb_reg(p_dm, BBREG_0xa04, 0x0f000000,
			       0x0); /* CCK default is at path-a */
	}
}

void phydm_dynamic_select_cck_path_8822b(struct PHY_DM_STRUCT *p_dm)
{
	struct phydm_fa_struct *p_fa_cnt =
		(struct phydm_fa_struct *)phydm_get_structure(
			p_dm, PHYDM_FALSEALMCNT);
	struct drp_rtl8822b_struct *p_8822b = &p_dm->phydm_rtl8822b;

	if (p_dm->ap_total_num > 10) {
		if (p_8822b->path_judge & BIT(2))
			odm_set_bb_reg(p_dm, BBREG_0xa04, 0x0f000000,
				       0x0); /*fix CCK Path A if AP nums > 10*/
		return;
	}

	if (p_8822b->path_judge & BIT(2))
		return;

	ODM_RT_TRACE(p_dm, ODM_PHY_CONFIG, ODM_DBG_LOUD,
		     ("phydm 8822b cck rx path selection start\n"));

	if (p_8822b->path_judge & BB_PATH_A) {
		p_8822b->path_a_cck_fa = (u16)p_fa_cnt->cnt_cck_fail;
		p_8822b->path_judge &= ~BB_PATH_A;
		odm_set_bb_reg(p_dm, BBREG_0xa04, 0x0f000000,
			       0x5); /*change to path B collect CCKFA*/
	} else if (p_8822b->path_judge & BB_PATH_B) {
		p_8822b->path_b_cck_fa = (u16)p_fa_cnt->cnt_cck_fail;
		p_8822b->path_judge &= ~BB_PATH_B;

		if (p_8822b->path_a_cck_fa <= p_8822b->path_b_cck_fa)
			odm_set_bb_reg(p_dm, BBREG_0xa04, 0x0f000000,
				       0x0); /*FA A<=B choose A*/
		else
			odm_set_bb_reg(p_dm, BBREG_0xa04, 0x0f000000,
				       0x5); /*FA B>A choose B*/

		p_8822b->path_judge |=
			BIT(2); /*it means we have already choosed cck rx path*/
	}

	ODM_RT_TRACE(p_dm, ODM_PHY_CONFIG, ODM_DBG_LOUD,
		     ("path_a_fa = %d, path_b_fa = %d\n",
		      p_8822b->path_a_cck_fa, p_8822b->path_b_cck_fa));
}

void phydm_somlrxhp_setting(struct PHY_DM_STRUCT *p_dm, boolean switch_soml)
{
	if (switch_soml == true) {
		odm_set_bb_reg(p_dm, BBREG_0x19a8, MASKDWORD, 0xd90a0000);
		/* Following are RxHP settings for T2R as always low, workaround for OTA test, required to classify */
		odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(21) | BIT(18)), 0x0);
		odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(21) | BIT(18)), 0x0);
	} else {
		odm_set_bb_reg(p_dm, BBREG_0x19a8, MASKDWORD, 0x090a0000);
		odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(21) | BIT(18)), 0x0);
		odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(21) | BIT(18)), 0x0);
	}

	/* Dynamic RxHP setting with SoML on/off apply on all RFE type */
	if (!switch_soml && ((p_dm->rfe_type == 1) || (p_dm->rfe_type == 6) ||
			     (p_dm->rfe_type == 7) || (p_dm->rfe_type == 9))) {
		odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD, 0x08108000);
		odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x0);
	}

	if (*p_dm->p_channel <= 14) {
		if (switch_soml &&
		    (!((p_dm->rfe_type == 3) || (p_dm->rfe_type == 5) ||
		       (p_dm->rfe_type == 8)))) {
			odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD,
				       0x08108000);
			odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x0);
		}
	} else if (*p_dm->p_channel > 35) {
		if (switch_soml == true) {
			odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD,
				       0x08108000);
			odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x0);
		}
	}

#if 0
	if (!((p_dm->rfe_type == 1) || (p_dm->rfe_type == 6) || (p_dm->rfe_type == 7) || (p_dm->rfe_type == 9))) {
		if (*p_dm->p_channel <= 14) {
			/* TFBGA iFEM SoML on/off with RxHP always high-to-low */
			if ((switch_soml == true) && (!((p_dm->rfe_type == 3) || (p_dm->rfe_type == 5)))) {
				if (switch_soml == true) {
				odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD, 0x08108000);
				odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x0);
				odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(21)|(BIT(18))), 0x0);
				odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(21)|(BIT(18))), 0x0);
				} else {
				odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD, 0x08108492);
				odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x1);
				}
			}
		} else if (*p_dm->p_channel > 35) {
			if (switch_soml == true) {
				odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD, 0x08108000);
				odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x0);
				odm_set_bb_reg(p_dm, BBREG_0xc04, (BIT(21)|(BIT(18))), 0x0);
				odm_set_bb_reg(p_dm, BBREG_0xe04, (BIT(21)|(BIT(18))), 0x0);
			} else {
				odm_set_bb_reg(p_dm, BBREG_0x8cc, MASKDWORD, 0x08108492);
				odm_set_bb_reg(p_dm, BBREG_0x8d8, BIT(27), 0x1);
			}	
		}
		PHYDM_DBG(p_dm, ODM_COMP_API, ("Dynamic RxHP control with SoML is enable !!\n"));
	}
#endif
}

void phydm_config_tx2path_8822b(struct PHY_DM_STRUCT *p_dm,
				enum wireless_set wireless_mode,
				boolean is_tx2_path)
{
	if (wireless_mode == WIRELESS_CCK) {
		if (is_tx2_path == true)
			odm_set_bb_reg(p_dm, BBREG_0xa04, 0xf0000000, 0xc);
		else
			odm_set_bb_reg(p_dm, BBREG_0xa04, 0xf0000000, 0x8);
	} else {
		if (is_tx2_path == true)
			odm_set_bb_reg(p_dm, BBREG_0x93c, 0xf00000, 0x3);
		else
			odm_set_bb_reg(p_dm, BBREG_0x93c, 0xf00000, 0x1);
	}
}

void phydm_dynamic_ant_weighting_8822b(struct PHY_DM_STRUCT *p_dm)
{
	u8 rssi_l2h = 43, rssi_h2l = 37;
	u8 reg_8;

	if (p_dm->is_disable_dym_ant_weighting)
		return;

	if (*p_dm->p_channel <= 14) {
		if (p_dm->rssi_min >= rssi_l2h) {
			odm_set_bb_reg(p_dm, BBREG_0x98c, 0x7fc0000, 0x0);

			/*equal weighting*/
			reg_8 = (u8)odm_get_bb_reg(p_dm, BBREG_0xf94,
						   BIT(0) | BIT(1) | BIT(2));
			PHYDM_DBG(
				p_dm, ODM_COMP_API,
				("Equal weighting ,rssi_min = %d\n, BBREG_0xf94[2:0] = 0x%x\n",
				 p_dm->rssi_min, reg_8));
		} else if (p_dm->rssi_min <= rssi_h2l) {
			odm_set_bb_reg(p_dm, BBREG_0x98c, MASKDWORD,
				       0x43440000);

			/*fix sec_min_wgt = 1/2*/
			reg_8 = (u8)odm_get_bb_reg(p_dm, BBREG_0xf94,
						   BIT(0) | BIT(1) | BIT(2));
			PHYDM_DBG(
				p_dm, ODM_COMP_API,
				("AGC weighting ,rssi_min = %d\n, BBREG_0xf94[2:0] = 0x%x\n",
				 p_dm->rssi_min, reg_8));
		}
	} else {
		odm_set_bb_reg(p_dm, BBREG_0x98c, MASKDWORD, 0x43440000);

		reg_8 = (u8)odm_get_bb_reg(p_dm, BBREG_0xf94,
					   BIT(0) | BIT(1) | BIT(2));
		PHYDM_DBG(
			p_dm, ODM_COMP_API,
			("AGC weighting ,rssi_min = %d\n, BBREG_0xf94[2:0] = 0x%x\n",
			 p_dm->rssi_min, reg_8));
		/*fix sec_min_wgt = 1/2*/
	}
}

void phydm_rxagc_switch_8822b(struct PHY_DM_STRUCT *p_dm,
			      boolean enable_rxagc_switch)
{
	if ((p_dm->rfe_type == 15) || (p_dm->rfe_type == 16)) {
		PHYDM_DBG(p_dm, ODM_COMP_API, ("Microsoft case!\n"));

	} else {
		PHYDM_DBG(p_dm, ODM_COMP_API, ("Not Microsoft case\n"));
		return;
	}

	if (enable_rxagc_switch == true) {
		if ((*p_dm->p_channel >= 36) && (*p_dm->p_channel <= 64)) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x1);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x5);
		} else if ((*p_dm->p_channel >= 100) &&
			   (*p_dm->p_channel <= 144)) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x2);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x6);
		} else if (*p_dm->p_channel >= 149) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x3);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x7);
		}
		p_dm->brxagcswitch = true;
		PHYDM_DBG(
			p_dm, ODM_COMP_API,
			("Microsoft case! AGC table (path-b) is switched!\n"));

	} else {
		if ((*p_dm->p_channel >= 36) && (*p_dm->p_channel <= 64)) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x1);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x1);
		} else if ((*p_dm->p_channel >= 100) &&
			   (*p_dm->p_channel <= 144)) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x2);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x2);
		} else if (*p_dm->p_channel >= 149) {
			odm_set_bb_reg(p_dm, BBREG_0x958, BIT(4), 0x1);
			odm_set_bb_reg(p_dm, BBREG_0xc1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x3);
			odm_set_bb_reg(p_dm, BBREG_0xe1c,
				       (BIT(11) | BIT(10) | BIT(9) | BIT(8)),
				       0x3);
		}
		p_dm->brxagcswitch = false;
		PHYDM_DBG(p_dm, ODM_COMP_API,
			  ("AGC table are the same on path-a and b\n"));
	}
}

void phydm_hwsetting_8822b(struct PHY_DM_STRUCT *p_dm)
{
	struct drp_rtl8822b_struct *p_8822b = &p_dm->phydm_rtl8822b;

	if ((p_dm->p_advance_ota & PHYDM_HP_OTA_SETTING_A) ||
	    (p_dm->p_advance_ota & PHYDM_HP_OTA_SETTING_B)) {
		phydm_dynamic_parameters_ota(p_dm);
	} else {
		if (p_dm->bhtstfdisabled == false)
			phydm_dynamic_switch_htstf_mumimo_8822b(p_dm);
		else
			PHYDM_DBG(p_dm, ODM_PHY_CONFIG,
				  ("Default HT-STF gain control setting\n"));
	}

	phydm_dynamic_ant_weighting_8822b(p_dm);

	if (p_dm->p_advance_ota & PHYDM_ASUS_OTA_SETTING) {
		if (p_dm->rssi_min <= 20)
			phydm_somlrxhp_setting(p_dm, false);
		else if (p_dm->rssi_min >= 25)
			phydm_somlrxhp_setting(p_dm, true);
	}

	if ((p_dm->p_advance_ota & PHYDM_ASUS_OTA_SETTING_CCK_PATH) ||
	    (p_dm->p_advance_ota & PHYDM_HP_OTA_SETTING_CCK_PATH)) {
		if (p_dm->is_linked)
			phydm_dynamic_select_cck_path_8822b(p_dm);
		else
			p_8822b->path_judge |=
				((~BIT(2)) | BB_PATH_A | BB_PATH_B);
	}
}

#endif	/* RTL8822B_SUPPORT == 1 */
