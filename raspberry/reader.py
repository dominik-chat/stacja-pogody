# Copyright (C) 2022 Dominik Chat, Paweł Majtas
#
# This file is part of Stacja Pogody - remote weather station based on NRF DK
# and a Raspberry Pi.
#
# Stacja Pogody is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# Stacja Pogody is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Stacja Pogody. If not, see <https://www.gnu.org/licenses/>.

import gatt

class Stacja(gatt.Device):
    def connect_succeeded(self):
        super().connect_succeeded()
        print("[%s] Connected" % (self.mac_address))

    def connect_failed(self, error):
        super().connect_failed(error)
        print("[%s] Connection failed: %s" % (self.mac_address, str(error)))

    def disconnect_succeeded(self):
        super().disconnect_succeeded()
        print("[%s] Disconnected" % (self.mac_address))

    def services_resolved(self):
        super().services_resolved()

        ess_service = next(
            s for s in self.services
            if s.uuid == '0000181a-0000-1000-8000-00805f9b34fb')

        temperature_characteristic = next(
            c for c in ess_service.characteristics
            if c.uuid == '00002a6e-0000-1000-8000-00805f9b34fb')
        
        pressure_characteristic = next(
            c for c in ess_service.characteristics
            if c.uuid == '00002a6d-0000-1000-8000-00805f9b34fb')

        temperature_characteristic.enable_notifications()
        pressure_characteristic.enable_notifications()

    def characteristic_value_updated(self, characteristic, value):
        if characteristic.uuid == '00002a6e-0000-1000-8000-00805f9b34fb':
            print("Temperature:", int.from_bytes(value, "little", signed=True)/100)
        if characteristic.uuid == '00002a6d-0000-1000-8000-00805f9b34fb':
            print("Pressure:", int.from_bytes(value, "little", signed=True)/1000)

manager = gatt.DeviceManager(adapter_name='hci0')
device = Stacja(mac_address='cb:42:72:a9:a2:22', manager=manager)
device.connect()
manager.run()
