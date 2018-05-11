#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "jetson_tx2_power.hh"

using namespace std;

template<typename T, size_t N>
T * end(T (&ra)[N]) {
    return ra + N;
}

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
				cout << "Reads the power information from the INA3221 device on the Jetson TX2 board" << endl;
				cout << "The option \'-f <csv file>\' writes a CSV file. If the file already exists the values will be appended at the end." << endl;
				cout << "The option \'-c <number>\' loops \'number\' times, can be used with -t." << endl;
				cout << "The option \'-t <period>\' takes a sample every \'period\' miliseconds, can be used with -c." << endl;
				return 0;
			case '?':
				if (optopt == 'f'|| optopt == 'c' || optopt == 't')
					cerr << "Option -"<< optopt << "requires an argument." << endl;
				else if (isprint (optopt))
					cerr << "Unknown option `-"<< optopt << "\'." << endl;
				else
					cerr << "Unknown option character `\\x"<< optopt << "\'." << endl;
				return 1;
			default:
				abort ();
		}
	}

	std::vector<PowerReadingDevice> devices = create_devices();

	if(msecs == 0 && loops == 0) {
		if( csv_filename == NULL )
			print_values(devices);
		else
			to_csv(csv_filename, devices);
	} if(msecs != 0 && loops == 0) {
		signal(SIGINT, handle_sigint);
		while( running == 1 ) {
			if( csv_filename == NULL )
				print_values(devices);
			else
				to_csv(csv_filename, devices);

			nanosleep(&delay, NULL);
		}
	} if(msecs == 0 && loops != 0) {
	    for(c = 0; c < loops; c++) {
		if( csv_filename == NULL )
			print_values(devices);
		else
			to_csv(csv_filename, devices);
	    }
	} else {
	    for(c = 0; c < loops; c++) {
		if( csv_filename == NULL )
			print_values(devices);
		else
			to_csv(csv_filename, devices);
		nanosleep(&delay, NULL);
	    }
	}

	return (0);
}
