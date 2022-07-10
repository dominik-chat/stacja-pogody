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
static uint32_t pressure;
static int16_t temperature;


static uint32_t press_to_val(double press)
{
	return (press * 10 * 1000);
}

static int16_t temp_to_val(double temp)
{
	return (temp * 100);
}

static ssize_t read_press(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			  void *buf, uint16_t len, uint16_t offset)
{
	const uint32_t *press = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, press,
				 sizeof(*press));

	return 0;
}

static ssize_t read_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	const int16_t *temp = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, temp,
				 sizeof(*temp));

	return 0;
}

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
	BT_GATT_CHARACTERISTIC(BT_UUID_PRESSURE,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_press, NULL, &pressure),
	BT_GATT_CCC(press_ccc_cfg, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_temp, NULL, &temperature),
	BT_GATT_CCC(temp_ccc_cfg,  BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


int bt_ess_init()
{
	return 0;
}

int bt_ess_send_pressure(double press)
{
	pressure = press_to_val(press);

	if (!notify_press) {
		return -EACCES;
	}

	return bt_gatt_notify(NULL, &ess_svc.attrs[2], &pressure,
							sizeof(pressure));
}

int bt_ess_send_temperature(double temp)
{
	temperature = temp_to_val(temp);

	if (!notify_temp) {
		return -EACCES;
	}

	return bt_gatt_notify(NULL, &ess_svc.attrs[4], &temperature,
							sizeof(temperature));
}
