/*
 * Copyright (C) 2022 Dominik Chat, Paweł Majtas
 *
 * This file is part of Stacja Pogody - remote weather station based on NRF DK
 * and a Raspberry Pi.
 *
 * Stacja Pogody is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Stacja Pogody is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stacja Pogody. If not, see <https://www.gnu.org/licenses/>.
 */

#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ess.h"


static bool notify_press;
static bool notify_temp;


static void press_ccc_cfg(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_press = (value == BT_GATT_CCC_NOTIFY);
}

static void temp_ccc_cfg(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_temp = (value == BT_GATT_CCC_NOTIFY);
}


BT_GATT_SERVICE_DEFINE(ess_svc,
BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),
	BT_GATT_CHARACTERISTIC(BT_UUID_PRESSURE, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, NULL, NULL, NULL),
	BT_GATT_CCC(press_ccc_cfg, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE, BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, NULL, NULL, NULL),
	BT_GATT_CCC(temp_ccc_cfg,  BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


int bt_ess_init()
{
	return 0;
}

int bt_ess_send_pressure(double press)
{
	uint32_t val;

	if (!notify_press) {
		return -EACCES;
	}

	val = (press * 10 * 1000);
	return bt_gatt_notify(NULL, &ess_svc.attrs[2], &val, sizeof(val));
}

int bt_ess_send_temperature(double temp)
{
	int16_t val;

	if (!notify_temp) {
		return -EACCES;
	}

	val = (temp * 100);
	return bt_gatt_notify(NULL, &ess_svc.attrs[4], &val, sizeof(val));
}
