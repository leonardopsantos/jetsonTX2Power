#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "jetson_tx2_power.h"

struct rail_power_t rail_power[] = {
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH0"in_voltage0_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH0"in_current0_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH0"in_voltage1_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH0"in_current1_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH0"in_voltage2_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH0"in_current2_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH1"in_voltage0_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH1"in_current0_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH1"in_voltage1_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH1"in_current1_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH1"in_voltage2_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH1"in_current2_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH2"in_voltage0_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH2"in_current0_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH2"in_voltage1_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH2"in_current1_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH2"in_voltage2_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH2"in_current2_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH3"in_voltage0_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH3"in_current0_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH3"in_voltage1_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH3"in_current1_input", .fd = -1}},
	{   .name[0] = '\0',
	    .voltage = { .dev = I2C_FULL_PATH3"in_voltage2_input", .fd = -1},
	    .current = { .dev = I2C_FULL_PATH3"in_current2_input", .fd = -1}}
};

int create_devices(void)
{
int i, fd, n;
char buf[256];
char *pos;

	for( i = 0; i < 12; i++) {
		if( strlen(rail_power[i].name) > 0 &&
		    rail_power[i].voltage.fd != -1 &&
		    rail_power[i].current.fd != -1 )
			continue;

		if( i < 3 )
			sprintf(buf, I2C_FULL_PATH0"rail_name_%d", i);
		else if( i >= 3 && i < 6 )
			sprintf(buf, I2C_FULL_PATH1"rail_name_%d", i-3);
		else if( i >= 6 && i < 9 )
			sprintf(buf, I2C_FULL_PATH2"rail_name_%d", i-6);
		else
			sprintf(buf, I2C_FULL_PATH3"rail_name_%d", i-9);

		fd = open(buf, O_RDONLY | O_NONBLOCK);
		if (fd < 0) {
			perror("1: open()");
			fprintf(stderr, "%s\n", buf);
			exit(1);
		}
		lseek(fd, 0, 0);
		n = read(fd, rail_power[i].name, sizeof(rail_power[i].name)-1);
		if( n < 0 ) {
		    close(fd);
		    return -1;
		}
		if ((pos=strchr(rail_power[i].name, '\n')) != NULL)
		    *pos = '\0';
		close(fd);

		rail_power[i].voltage.fd = open(rail_power[i].voltage.dev, O_RDONLY | O_NONBLOCK);
		if (rail_power[i].voltage.fd < 0) {
			perror("open()");
			exit(1);
		}
		rail_power[i].current.fd = open(rail_power[i].current.dev, O_RDONLY | O_NONBLOCK);
		if (rail_power[i].current.fd < 0) {
			perror("open()");
			exit(1);
		}

	}
	return 0;
}

int destroy_devices(void)
{
int i;

	for( i = 0; i < 12; i++) {
		close(rail_power[i].voltage.fd);
		close(rail_power[i].current.fd);
	}
	return 0;
}

int update_power_values(void)
{
int i, n;
char buf[256];

	for( i = 0; i < 12; i++) {
		lseek(rail_power[i].voltage.fd, 0, 0);
		n = read(rail_power[i].voltage.fd, buf, sizeof(buf)-1);
		if (n > 0) {
			rail_power[i].voltage.value = strtod(buf, NULL);
		}
		lseek(rail_power[i].current.fd, 0, 0);
		n = read(rail_power[i].current.fd, buf, sizeof(buf)-1);
		if (n > 0) {
			rail_power[i].current.value = strtod(buf, NULL);
		}
	}
	return 0;
}

int to_csv(char *csv_filename)
{
int i;
int fd_csv;
int open_flags = O_WRONLY | O_NONBLOCK;
int write_header = 0;
char buf[8192];
struct timeval now;
time_t t;
struct tm *tmp;


	if( access( csv_filename, F_OK ) != -1 ) {
		open_flags |= O_APPEND;
		
	} else {
		open_flags |= O_CREAT;
		write_header = 1;
	}

	fd_csv = open(csv_filename,open_flags, S_IRUSR | S_IWUSR| S_IRGRP| S_IWGRP);

	if(fd_csv <0)
	    return -1;

	if( write_header == 1 ) {
		write(fd_csv,"date,",strlen("date,"));
		write(fd_csv,"time,",strlen("time,"));
		memset(buf, '\0', sizeof(buf));
		for( i = 0; i < 11; i++) {
			sprintf(buf+strlen(buf), "%s voltage,", rail_power[i].name);
			sprintf(buf+strlen(buf), "%s current,", rail_power[i].name);
		}
		sprintf(buf+strlen(buf), "%s voltage,", rail_power[i].name);
		sprintf(buf+strlen(buf), "%s current\n", rail_power[i].name);

		write(fd_csv,buf,strlen(buf));
	}

	update_power_values();

	gettimeofday(&now, NULL);
	t = now.tv_sec;
	tmp = localtime(&t);

	memset(buf, '\0', sizeof(buf));
	strftime(buf, sizeof(buf), "%Y-%m-%d", tmp);
	write(fd_csv,buf,strlen(buf));

	memset(buf, '\0', sizeof(buf));
	strftime(buf, sizeof(buf), ",%H:%M:%S", tmp);
	sprintf(buf+strlen(buf), ":%ld", now.tv_usec/1000);
	write(fd_csv,buf,strlen(buf));

	for( i = 0; i < 11; i++) {
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, ",%f,%f", rail_power[i].voltage.value, rail_power[i].current.value);
		write(fd_csv,buf,strlen(buf));
	}
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, ",%f,%f\n", rail_power[i].voltage.value, rail_power[i].current.value);
	write(fd_csv,buf,strlen(buf));

	close(fd_csv);

	return 0;
}

int print_values(void)
{
int i;
time_t result;

	result = time(NULL);

	update_power_values();
	
	for( i = 0; i < 12; i++) {
		printf("%.19s\n", ctime(&result));
		printf("%s : %f mV, %f mA\n", 
			rail_power[i].name,
			rail_power[i].voltage.value,
			rail_power[i].current.value);
	}

	return 0;
}
