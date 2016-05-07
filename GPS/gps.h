#ifndef GPS_H_   /* Include guard */
#define GPS_H_
#include "mbed.h"



int gps_datardy(void);
char* gps_data(void);
void gps_init(void);  /* GPS init */
void gps_print(void); /* GPS print line */
void gps_handler(void); /* GPS Interrupt Handler */
void gps_parser(char*);
void gga_parser(void);
void rmc_parser(void);

#endif // GPS_H_
