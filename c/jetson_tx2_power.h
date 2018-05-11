
#ifndef _JETSON_TX2_PWR_H_
#define _JETSON_TX2_PWR_H_

#define I2C_BUS		"3160000.i2c/i2c-0"
#define I2C_FULL_PATH0	"/sys/devices/"I2C_BUS"/0-0040/iio_device/"
#define I2C_FULL_PATH1	"/sys/devices/"I2C_BUS"/0-0041/iio_device/"
#define I2C_FULL_PATH2	"/sys/devices/"I2C_BUS"/0-0042/iio_device/"
#define I2C_FULL_PATH3	"/sys/devices/"I2C_BUS"/0-0043/iio_device/"

struct rail_power_reading_t {
	char dev[256];
	int fd;
	float value;
};

struct rail_power_t {
	char name[64];
	struct rail_power_reading_t voltage;
	struct rail_power_reading_t current;
};

int create_devices(void);
int destroy_devices(void);
int update_power_values(void);
int to_csv(char *csv_filename);
int print_values(void);

#endif /* _JETSON_TX2_PWR_H_ */
