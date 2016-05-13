#ifndef IMU_H_   /* Include guard */
#define IMU_H_
#include "mbed.h"

struct imu_data{

	int scale = 1000;
	int gyro_x[60];
	int gyro_y[60];
	int gyro_z[60];
	int accel_x[60];
	int accel_y[60];
	int accel_z[60];
	int mag_x[60];
	int mag_y[60];
	int mag_z[60];

};

void set_gyro(int,int,int);
void set_accel(int,int,int);
void set_mag(int,int,int);
void set_all(int,int,int,int,int,int,int,int,int);
imu_data get_imu(void);

#endif //IMU_H
