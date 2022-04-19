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

#define MODULE main
#include <logging/log.h>
LOG_MODULE_REGISTER(MODULE);


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

	while (1) {
		err = read_bmp(dev, &temp, &press);
		if (err) {
			LOG_ERR("Invalid value.");
			return;
		}

		printk("Temperature: %f oC, Pressure: %f kPa\n", temp, press);

		k_sleep(K_MSEC(1000));
	}
}
