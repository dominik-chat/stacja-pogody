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
#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ess.h"

#define MODULE main
#include <logging/log.h>
LOG_MODULE_REGISTER(MODULE);


static const struct bt_data adver[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
		(sizeof(CONFIG_BT_DEVICE_NAME) - 1)),
};

static const struct bt_data serv[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_16_ENCODE(BT_UUID_ESS_VAL)),
};


static int read_bmp(const struct device *dev, double *temp, double *press)
{
	struct sensor_value tmp;

	if (dev == NULL) {
		return -EINVAL;
	}

	sensor_sample_fetch(dev);
	sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &tmp);
	*temp = sensor_value_to_double(&tmp);
	sensor_channel_get(dev, SENSOR_CHAN_PRESS, &tmp);
	*press = sensor_value_to_double(&tmp);

	return 0;
}

void main(void)
{
	const struct device *dev;
	double temp, press;
	int err;

	dev = DEVICE_DT_GET_ANY(bosch_bme280);
	if (dev == NULL) {
		LOG_ERR("Invalid DTS.");
		return;
	}

	if (!device_is_ready(dev)) {
		LOG_ERR("Error initializing bmp280.");
		return;
	}

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed .");
		return;
	}

	if (bt_ess_init()) {
		LOG_ERR("Error initializing ESS.");
		return;
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN, adver, ARRAY_SIZE(adver),
			      serv, ARRAY_SIZE(serv));
	if (err) {
		LOG_ERR("Advertising failed.");
		return;
	}

	while (1) {
		err = read_bmp(dev, &temp, &press);
		if (err) {
			LOG_ERR("Invalid value.");
			return;
		}

		//printk("Temperature: %f oC, Pressure: %f kPa\n", temp, press);
		printk("Press %i ", bt_ess_send_pressure(press));
		printk("Temp %i \n", bt_ess_send_temperature(temp));

		k_sleep(K_MSEC(1000));
	}
}
