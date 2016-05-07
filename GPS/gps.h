#ifndef GPS_H_   /* Include guard */
#define GPS_H_
#include "mbed.h"

struct gps_data{
	float altitude;
	int fix_quality;
	int satellites_tracked;
	float lat;
	float lng;
	float speed_kts;
	float speed_kph;
	float true_track;
	float mag_track;
	float hdop;
	int time;
    int day;
    int month;
    int year;
    int hours;
    int minutes;
    int seconds;
    int microseconds;


};

void gps_init(void);  /* GPS init */
void gps_handler(void); /* GPS Interrupt Handler */
void gps_parser(char*);
void gps_alt(void);




#endif // GPS_H_
