#include "mbed.h"
#include "gps.h"
#include "IMU.h"
#include "SDFileSystem.h"

typedef enum {
    GPS_GET,
    IMU_GET,
    WRITE_DATA
} sensor_status;

struct data{
	gps_data gpsdata;
	imu_data imudata;
};

//
sensor_status sen_status;
//Timers
Ticker print_tick;
Ticker sensor_tick;
Ticker IMU_tick;
//Analog Pins
AnalogIn gx(PA_0);
AnalogIn gy(PA_1);
AnalogIn gz(PA_4);
AnalogIn ax(PB_0);
AnalogIn ay(PC_1);
AnalogIn az(PC_0);
//Serial Devices
Serial pc(USBTX,USBRX);
//SPI spi_1(PA_7,PA_6,PA_5);//MOSI, MISO, SCK
SDFileSystem sd(PA_7,PA_6,PA_5,PA_9,"sd");





data sensordata;

void print_handler(void){
	gps_data gpsdata = sensordata.gpsdata;
	imu_data imudata = sensordata.imudata;
	printf("TIME:  %i:%i:%2i.%2i   ALT:%i\n\r", gpsdata.hours,gpsdata.minutes,gpsdata.seconds,gpsdata.microseconds,(int)gpsdata.altitude);
	for(int i = 0;i<50;i++){
		printf("G: %i, %i, %i\tA: %i, %i, %i\n\r",imudata.gyro_x[i],imudata.gyro_y[i],imudata.gyro_z[i],imudata.accel_x[i],imudata.accel_y[i],imudata.accel_z[i]);
	}
	mkdir("/sd/mydir", 0777);

	FILE *fp = fopen("/sd/mydir/sdtest.txt", "w+");
	if(fp == NULL) {
		error("Could not open file for write\n\c");
	}
	fprintf(fp, "Hello fun SD Card World! TIME:  %i:%i:%2i.%2i ALT:%i\n\r",gpsdata.hours,gpsdata.minutes,gpsdata.seconds,gpsdata.microseconds,(int)gpsdata.altitude);
	fclose(fp);

}

void imu_handler(){

	if (increment_imu()) {
		sensordata.imudata = get_imu();
		print_handler();
	}else{
		set_all((int)gx.read_u16(),(int)gy.read_u16(),(int)gz.read_u16(),(int)ax.read_u16(),(int)ay.read_u16(),(int)az.read_u16(),0,0,0);
	}

}

void sensor_handler(){
	switch(sen_status){
	case GPS_GET:
		sensordata.gpsdata = get_gps();
		set_all((int)gx.read_u16(),(int)gy.read_u16(),(int)gz.read_u16(),(int)ax.read_u16(),(int)ay.read_u16(),(int)az.read_u16(),0,0,0);
		sen_status = IMU_GET;
		break;
	case IMU_GET:
		if (increment_imu()) {
			set_all((int)gx.read_u16(),(int)gy.read_u16(),(int)gz.read_u16(),(int)ax.read_u16(),(int)ay.read_u16(),(int)az.read_u16(),0,0,0);
			sensordata.imudata = get_imu();
			print_handler();
			sen_status = GPS_GET;
		}else{
			set_all((int)gx.read_u16(),(int)gy.read_u16(),(int)gz.read_u16(),(int)ax.read_u16(),(int)ay.read_u16(),(int)az.read_u16(),0,0,0);
		}
	default:
		break;
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
	sensor_tick.attach(&sensor_handler,0.02);
    while (true) {
        // Do other things...
    }
}
