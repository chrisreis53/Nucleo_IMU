#include "mbed.h"
#include "gps.h"
#include "IMU.h"

Ticker print_tick;
Ticker sensor_tick;

Serial pc(USBTX,USBRX);

struct data{
	gps_data gpsdata;
	imu_data imudata;
};

data sensordata;

void print_handler(void){
	gps_data gpsdata = sensordata.gpsdata;
	imu_data imudata = sensordata.imudata;
	printf("TIME:  %i:%i:%2i.%2i   ALT:%i\n\r", gpsdata.hours,gpsdata.minutes,gpsdata.seconds,gpsdata.microseconds,(int)gpsdata.altitude);
//	for(int i = 0;i<60;i++){
//		printf("G: %i, %i, %i\tA: %i, %i, %i\n\r",imudata.gyro_x[i],imudata.gyro_y[i],imudata.gyro_z[i],imudata.accel_x[i],imudata.accel_y[i],imudata.accel_z[i]);
//	}

}

void imu_handler(){

}

int main() {
	pc.baud(256000);
	gps_init();
	pc.printf("\n\n**********TEST**********\n\n\r");
	print_tick.attach(&print_handler,1);
    while (true) {
        // Do other things...
    }
}
