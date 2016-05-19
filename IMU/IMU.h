#ifndef IMU_H_   /* Include guard */
#define IMU_H_
#include "mbed.h"

struct imu_data{

	int scale = 1000;
	uint gyro_x[60];
	uint gyro_y[60];
	uint gyro_z[60];
	uint accel_x[60];
	uint accel_y[60];
	uint accel_z[60];
	uint mag_x[60];
	uint mag_y[60];
	uint mag_z[60];

};

void set_gyro(int,int,int);
void set_accel(int,int,int);
void set_mag(int,int,int);
void set_all(uint,uint,uint,uint,uint,uint,uint,uint,uint);
int increment_imu(void);
imu_data get_imu(void);

#endif //IMU_H
