#include "mbed.h"
#include "gps.h"
#include "IMU.h"

//Timers
Ticker print_tick;
Ticker sensor_tick;
//Analog Pins
AnalogIn gx(PA_0);
AnalogIn gy(PA_1);
AnalogIn gz(PA_4);
AnalogIn ax(PB_0);
AnalogIn ay(PC_1);
AnalogIn az(PC_0);
//Serial Devices
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

	if (increment_imu()) {
		print_test
	}else{
		set_all(gx.read_u16(),gy.read_u16(),gz_data = gz.read_u16(),ax_data = ax.read_u16(),ay_data = ay.read_u16(),az_data = az.read_u16(),0,0,0);
	}

}

void print_test(imu_data imu, gps_data gps){
	for(int i = 0; i<50;i++){
		printf("TEST:GX-%i GY-%i GZ-%i AX-%i AY-%i AZ-%i\n\r",imu.gyro_x[i],imu.gyro_y[i],imu.gyro_y[i],imu.accel_x,imu.accel_y,imu.accel_z);
	}
}

int main() {
	pc.baud(256000);
	gps_init();
	pc.printf("\n\n**********TEST**********\n\n\r");
	print_tick.attach(&print_handler,1);
	sensor_tick.attach(&imu_handler,0.02);
	sensor_tick.attach();
    while (true) {
        // Do other things...
    }
}
