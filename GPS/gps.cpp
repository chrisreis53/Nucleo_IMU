#include "mbed.h"
#include "gps.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>
#include "minmea.h"

#define PMTK_SET_NMEA_UPDATERATE_1HZ "$PMTK220,1000*1F\r\n"
#define PMTK_SET_NMEA_UPDATERATE_5HZ "$PMTK220,200*2C\r\n"
#define PMTK_SET_NMEA_UPDATERATE_10HZ "$PMTK220,100*2F\r\n"
#define PMTK_SET_NMEA_BAUDRATE_9600 "$PMTK251,9600*17\r\n"
#define PMTK_SET_NMEA_BAUDRATE_115200  "$PMTK251,115200*1F\r\n"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n"
#define INDENT_SPACES "  "

Serial gps(PC_10,PC_11);

gps_data gpsdata;
const int buffer_size = 255;
char gpsBuffer[buffer_size+1];
char rBuffer[255];
volatile int gpsIn=0;
volatile int rxSoftOut=0;
bool newGPSData=false;

void gps_init(void){

	gps.baud(115200);
	gps.attach(&gps_handler);
	wait_ms(1);
	gps.puts(PMTK_SET_NMEA_BAUDRATE_9600);
	//gps.puts(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	gps.puts(PMTK_SET_NMEA_BAUDRATE_9600);
	//gps.puts(PMTK_SET_NMEA_UPDATERATE_10HZ);
	gps.puts(PMTK_SET_NMEA_BAUDRATE_9600);
	gps.baud(9600);
	//gps.puts(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	gps.puts(PMTK_SET_NMEA_UPDATERATE_5HZ);
	gps.puts(PMTK_SET_NMEA_OUTPUT_ALLDATA);

}

void gps_parser(char* data){

	//printf("%s", data);
	switch (minmea_sentence_id(data, false)) {
		case MINMEA_SENTENCE_RMC: {
			struct minmea_sentence_rmc frame;
			if (minmea_parse_rmc(&frame, data)) {
				gpsdata.day=frame.date.day;
				gpsdata.month=frame.date.month;
				gpsdata.year=frame.date.year;
				gpsdata.hours=frame.time.hours;
				gpsdata.minutes=frame.time.minutes;
				gpsdata.seconds=frame.time.seconds;
				gpsdata.microseconds=frame.time.microseconds;
#ifdef DEBUG
				printf(INDENT_SPACES "$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n\r",
						frame.latitude.value, frame.latitude.scale,
						frame.longitude.value, frame.longitude.scale,
						frame.speed.value, frame.speed.scale);
				printf(INDENT_SPACES "$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n\r",
						minmea_rescale(&frame.latitude, 1000),
						minmea_rescale(&frame.longitude, 1000),
						minmea_rescale(&frame.speed, 1000));
				printf(INDENT_SPACES "$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n\r",
						minmea_tocoord(&frame.latitude),
						minmea_tocoord(&frame.longitude),
						minmea_tofloat(&frame.speed));
#endif
			}
			else {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxRMC sentence is not parsed\n\r");
#endif
			}
		} break;

		case MINMEA_SENTENCE_GGA: {
			struct minmea_sentence_gga frame;
			if (minmea_parse_gga(&frame, data)) {
				//gpsdata.altitude=minmea_tofloat(&frame.altitude);
				gpsdata.altitude=frame.altitude.value;
				gpsdata.lat=frame.latitude.value;
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGGA: fix quality: %d\n\r", frame.fix_quality);
#endif
			}
			else {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGGA sentence is not parsed\n\r");
#endif
			}
		} break;

		case MINMEA_SENTENCE_GST: {
			struct minmea_sentence_gst frame;
			if (minmea_parse_gst(&frame, data)) {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n\r",
						frame.latitude_error_deviation.value, frame.latitude_error_deviation.scale,
						frame.longitude_error_deviation.value, frame.longitude_error_deviation.scale,
						frame.altitude_error_deviation.value, frame.altitude_error_deviation.scale);
				printf(INDENT_SPACES "$xxGST fixed point latitude,longitude and altitude error deviation"
					   " scaled to one decimal place: (%d,%d,%d)\n\r",
						minmea_rescale(&frame.latitude_error_deviation, 10),
						minmea_rescale(&frame.longitude_error_deviation, 10),
						minmea_rescale(&frame.altitude_error_deviation, 10));
				printf(INDENT_SPACES "$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
						minmea_tofloat(&frame.latitude_error_deviation),
						minmea_tofloat(&frame.longitude_error_deviation),
						minmea_tofloat(&frame.altitude_error_deviation));
#endif
			}
			else {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGST sentence is not parsed\n\r");
#endif
			}
		} break;

		case MINMEA_SENTENCE_GSV: {
			struct minmea_sentence_gsv frame;
			if (minmea_parse_gsv(&frame, data)) {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGSV: message %d of %d\n\r", frame.msg_nr, frame.total_msgs);
				printf(INDENT_SPACES "$xxGSV: sattelites in view: %d\n\r", frame.total_sats);
				for (int i = 0; i < 4; i++)
					printf(INDENT_SPACES "$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n\r",
						frame.sats[i].nr,
						frame.sats[i].elevation,
						frame.sats[i].azimuth,
						frame.sats[i].snr);
#endif
			}
			else {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxGSV sentence is not parsed\n\r");
#endif
			}
		} break;

		case MINMEA_SENTENCE_VTG: {
		   struct minmea_sentence_vtg frame;
		   if (minmea_parse_vtg(&frame, data)) {
			   gpsdata.true_track = frame.true_track_degrees.value;
			   gpsdata.mag_track = frame.magnetic_track_degrees.value;
			   gpsdata.speed_kts = frame.speed_knots.value;
#ifdef DEBUG
				printf(INDENT_SPACES "$xxVTG: true track degrees = %f\n\r",
					   minmea_tofloat(&frame.true_track_degrees));
				printf(INDENT_SPACES "        magnetic track degrees = %f\n\r",
					   minmea_tofloat(&frame.magnetic_track_degrees));
				printf(INDENT_SPACES "        speed knots = %f\n\r",
						minmea_tofloat(&frame.speed_knots));
				printf(INDENT_SPACES "        speed kph = %f\n\r",
						minmea_tofloat(&frame.speed_kph));
#endif
		   }
		   else {
#ifdef DEBUG
				printf(INDENT_SPACES "$xxVTG sentence is not parsed\n\r");
#endif
		   }
		} break;

		case MINMEA_INVALID: {
#ifdef DEBUG
			printf(INDENT_SPACES "$xxxxx sentence is not valid\n\r");
#endif
		} break;
		default: {
#ifdef DEBUG
			printf(INDENT_SPACES "$xxxxx sentence is not parsed\n\r");
#endif
		} break;
	}
}

void gps_alt(void){
	printf("TIME:  %i:%i:%2i.%2i   ALT:%i\n\r", gpsdata.hours,gpsdata.minutes,gpsdata.seconds,gpsdata.microseconds,(int)gpsdata.altitude);
}

void gps_unix_time(void){

}

gps_data get_gps(void){
	return gpsdata;
}

void gps_handler(void){
	char data = gps.getc();
	if(data != '\n'){
		gpsBuffer[gpsIn] = data;
		gpsIn++;
	}else{
		gpsBuffer[gpsIn] = '\n';
		gpsBuffer[gpsIn+1] = '\0';
		gpsIn=0;
		newGPSData = true;
		gps_parser(gpsBuffer);
	}
}
