#include "mbed.h"
#include "gps.h"

Ticker print_ticker;

Serial pc(USBTX,USBRX);

void print_handler(void){
	gps_data gpsdata = get_gps();
	printf("TIME:  %i:%i:%2i.%2i   ALT:%i\n\r", gpsdata.hours,gpsdata.minutes,gpsdata.seconds,gpsdata.microseconds,(int)gpsdata.altitude);
}

int main() {
	pc.baud(256000);
	gps_init();
	pc.printf("\n\n**********TEST**********\n\n\r");
	print_ticker.attach(&print_handler,2.5);
    while (true) {
        // Do other things...
    }
}
