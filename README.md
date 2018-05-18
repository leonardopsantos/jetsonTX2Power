# jetsonTX2Power

Reads the hardware sensors on the [Jetson TX2](https://developer.nvidia.com/embedded/buy/jetson-tx2) [embedded kit](https://www.nvidia.com/en-us/autonomous-machines/embedded-systems-dev-kits-modules/) board and either prints the values on the screen os writes a CSV file.

A very useful resource is the [Jetson TX2 - INA226 (Power Monitor with i2C Interface)](https://devtalk.nvidia.com/default/topic/1000830/?offset=12#5252962) forum thread on the NVIDIA Developers forum.


## Power Rails on the TX2

The power rails on the TX2 module and the carrier board are monitored by four [INA3221](http://www.ti.com/product/INA3221) devices. These monitors can read up to three rails (both voltage and current) each. 
The monitors are exposed by SYSFS, device is ``in_voltageN_input`` [0..2] and the corresponding current device is ``in_currentN_input`` [0..2].

The TX2 has two INA3221 power monitors:
```bash
/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device
/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device
```

While the carrier board has other two INA3221:

```bash
/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device
/sys/devices/3160000.i2c/i2c-0/0-0043/iio_device
```

The power rails are (gathered from the NVIDIA Jetson TX2/TX2i OEM PRODUCT DESIGN GUIDE and the Jetson TX1-TX2 Developer Kit Carrier Board Design Files schematic):


#### ``/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device``

Location: On the TX2 module.

* rail_name_0: VDD_SYS_GPU
* in_voltage0_input: 19136
* Nominal voltage: 19 V
* Description: Tegra GPU & SRAM, GPU supply input

* rail_name_1: VDD_SYS_SOC
* in_voltage1_input: 19144
* Nominal voltage: 19 V
* Description: VDD_SYS_SOC_IN, SOC supply input.

* rail_name_2: VDD_4V0_WIFI
* in_voltage2_input: 4784
* Nominal voltage: 5 V
* Description: 

#### ``/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device/``
Location: on the TX2 module.

* rail_name_0: VDD_IN
* in_voltage0_input: 19144
* Nominal voltage: 19 V
* Description: VDD_IN Supply Monitor. It seems it DOES NOT power the VDD_SYS_CPU and VDD_SYS_GPU.

* rail_name_1: VDD_SYS_CPU
* in_voltage1_input: 19128
* Nominal voltage: 19 V
* Description: VDD_SYS_CPU_IN (CPU supply input)

* rail_name_2: VDD_SYS_DDR
* in_voltage2_input: 4776
* Nominal voltage: 5 V
* Description: VDD_5V0_SD0 (DDR supply input)

#### ``/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device``
Location: on the carrier board

* rail_name_0: VDD_MUX
* in_voltage0_input: 19160
* Nominal voltage: 19 V
* description: DC Jack to Carrier Board. Powers the VDD_5V0_IO_SYS and VDD_3V3_SYS rails. Does NOT power the VDD_IN rail in the TX2 module.

* rail_name_1: VDD_5V0_IO_SYS
* in_voltage1_input: 5000
* Nominal voltage: 5 V
* Description: 5V SYSTEM. Powers the VDD_1V8 rail.

* rail_name_2: VDD_3V3_SYS
* in_voltage2_input: 3344
* Nominal voltage: 3.3 V
* Description: 3V3 SYSTEM. Powers the VDD_3V3_IO_SLP (VDD_3V3_SLP) rail.

#### ``/sys/devices/3160000.i2c/i2c-0/0-0043/iio_device``
Location: on the carrier board

* rail_name_0: VDD_3V3_IO_SLP
* in_voltage0_input: 3344
* Nominal voltage: 3.3 V
* Description: 3V3 RUN RAIL. VDD_3V3_SLP on the carrier board schematic. Powers the 3.3V input of the SATA connector, display connector (pin 16), camera expansion (pins 84, 108, 110),2.8 V for the camera.

* rail_name_1: VDD_1V8_IO
* in_voltage1_input: 1816
* Nominal voltage: 1.8 V
* Description: 1.8V SYSTEM. VDD_1V8 on the carrier board schematic.

* rail_name_2: VDD_3V3_SYS_M2
* in_voltage2_input: 3344
* Nominal voltage: 3.3 V
* Description: M.2 3.3V. Powers the M.2 connector.


## Usage

There's a Python module and C++ and C code. All can be used independently.

### Python

First you need to get a list of the hardware sensors, use the `create_devices()` function, like so:

```python
import jetsonTX2Power as tx2pwr

pwr_devices=tx2pwr.create_devices()
```

The `pwr_devices` variable will be used to all other functions. The `create_devices()` function opens the I2C sys devices in Linux and reads the rail names. The list of devices returned by `create_devices()` doesn't have any state or open file descriptor, so there's no `destroy` function or method.

To write to a CSV file, use the function `to_csv`:

```python
def to_csv(csv_file, devices, comment='', xtra_fields=None):
```

The arguments are:

1. csv_file: a string with the file named to be used. If the file doesn't exists it will be created and the header will be written. If it exists the new values will be appended.

2. devices: The return value from the `create_devices` function.

3. comment: a simple comment that will be placed on the _comment_ column.

4. xtra_fields: dictionary with custom fields:value pairs. If it is passed as an argument, the _comment_ field will not be written, the curstom fields will be passed directly to the dictionary used by the [`csv.DictWriter`](https://docs.python.org/2/library/csv.html) function.

### C++

Pretty similitar to the Python script.

I promise to write a better help soon!

### C

Soon!


## Authors

* **[Leonardo Pereira Santos](https://github.com/leonardopsantos)** - *Initial work*

## License

This project is licensed under the GNU GPLv3 - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* [NVIDIA](http://www.nvidia.com) for creating the TX2 module!
* People on the [NVIDIA developer's forum](https://devtalk.nvidia.com/) for being so helpful!
