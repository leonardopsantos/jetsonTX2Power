#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "jetson_tx2_power.h"

static int running;

void handle_sigint(int signal)
{
	running = 0;
}

int main(int argc, char *argv[])
{
char *csv_filename = NULL;
int c;
int msecs = 0;
int loops = 0;
struct timespec delay;
struct sigaction sa;

	running = 1;

	opterr = 0;

	while ((c = getopt (argc, argv, "hf:c:t:")) != -1) {
		switch (c) {
			case 'f':
				csv_filename = optarg;
				break;
			case 't':
				msecs = atoi(optarg);
				delay.tv_sec=msecs/1000;
				delay.tv_nsec=(msecs%1000)*1000000;
				break;
			case 'c':
				loops = atoi(optarg);
				break;
			case 'h':
				printf("Reads the power information from the INA3221 device on the Jetson TX2 board.\n");
				printf("The option \'-f <csv file>\' writes a CSV file. If the file already exists the values will be appended at the end.\n");
				printf("The option \'-c <number>\' loops \'number\' times, can be used with -t.\n");
				printf("The option \'-t <period>\' takes a sample every \'period\' miliseconds, can be used with -c.\n");
				return 0;
			case '?':
				if (optopt == 'f'|| optopt == 'c' || optopt == 't')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
				return 1;
			default:
				abort ();
		}
	}

	create_devices();

	if(msecs == 0 && loops == 0) {
		if( csv_filename == NULL )
			print_values();
		else
			to_csv(csv_filename);
	} if(msecs != 0 && loops == 0) {
		signal(SIGINT, handle_sigint);
		while( running == 1 ) {
			if( csv_filename == NULL )
				print_values();
			else
				to_csv(csv_filename);

			nanosleep(&delay, NULL);
		}
	} if(msecs == 0 && loops != 0) {
	    for(c = 0; c < loops; c++) {
		if( csv_filename == NULL )
			print_values();
		else
			to_csv(csv_filename);
	    }
	} else {
	    for(c = 0; c < loops; c++) {
		if( csv_filename == NULL )
			print_values();
		else
			to_csv(csv_filename);
		nanosleep(&delay, NULL);
	    }
	}

	destroy_devices();

	return 0;
}
