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

#ifndef _ESS_H_
#define _ESS_H_

int bt_ess_init(void);

int bt_ess_send_pressure(double press);

int bt_ess_send_temperature(double temp);

#endif /* _ESS_H_ */
