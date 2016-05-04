#include "mbed.h"
#include "gps.h"

Ticker print_ticker;

Serial pc(USBTX,USBRX);

void print_handler(void){
	if(gps_datardy()){
		pc.printf("\r%s",gps_data());
	}
}

int main() {
	pc.baud(256000);
	gps_init();
	pc.printf("\n\n**********TEST**********\n\n\r");
	//print_ticker.attach(&print_handler,.001);
    while (true) {
        // Do other things...
    }
}
