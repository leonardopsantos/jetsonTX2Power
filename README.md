# jetsonTX2Power

Reads the hardware sensors on the [Jetson TX2](https://developer.nvidia.com/embedded/buy/jetson-tx2) [embedded kit](https://www.nvidia.com/en-us/autonomous-machines/embedded-systems-dev-kits-modules/) board and either prints the values on the screen os writes a CSV file.

A very useful resource is the [Jetson TX2 - INA226 (Power Monitor with i2C Interface)](https://devtalk.nvidia.com/default/topic/1000830/?offset=12#5252962) forum thread on the NVIDIA Developers forum.

## Usage

There's a Python module and C++ and C code. All can be used independently.

The SYSFS paths are:

```bash
/sys/devices/3160000.i2c/i2c-0/0-0040/iio_device
/sys/devices/3160000.i2c/i2c-0/0-0041/iio_device
/sys/devices/3160000.i2c/i2c-0/0-0042/iio_device
/sys/devices/3160000.i2c/i2c-0/0-0043/iio_device
```

I'll write an explanation for the power rails soon.

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
