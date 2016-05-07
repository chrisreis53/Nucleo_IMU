#include "mbed.h"
#include "gps.h"

Ticker print_ticker;

Serial pc(USBTX,USBRX);

//void print_handler(void){
//	pc.printf("TEST:  %s\n\r",gps_alt());
//}

int main() {
	pc.baud(256000);
	gps_init();
	pc.printf("\n\n**********TEST**********\n\n\r");
	print_ticker.attach(&gps_alt,2.5);
    while (true) {
        // Do other things...
    }
}
