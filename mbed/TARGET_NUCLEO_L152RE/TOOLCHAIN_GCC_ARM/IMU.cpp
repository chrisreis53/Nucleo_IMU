#include "mbed.h"
#include "IMU.h"

imu_data imudata;
int inc = 0;

int increment_imu(void){
	if(inc==50){
		inc=0;
		return 1;
	}else{
		inc++;
		return 0;
	}
}

void set_gyro(int x,int y,int z){
	imudata.gyro_x[inc-1]=x;
	imudata.gyro_y[inc-1]=y;
	imudata.gyro_z[inc-1]=z;
}

void set_accel(int x,int y,int z){
	imudata.accel_x[inc-1]=x;
	imudata.accel_y[inc-1]=y;
	imudata.accel_z[inc-1]=z;
}

void set_mag(int x,int y,int z){
	imudata.mag_x[inc-1]=x;
	imudata.mag_y[inc-1]=y;
	imudata.mag_z[inc-1]=z;
}

void set_all(int gx,int gy,int gz,int ax,int ay,int az,int mx,int my,int mz){

	set_gyro(gx,gy,gz);
	set_accel(ax,ay,az);
	set_mag(mx,my,mz);

}

imu_data get_imu(void){
	return imudata;
}
