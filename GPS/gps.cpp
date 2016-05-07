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

const int buffer_size = 255;
char gpsBuffer[buffer_size+1];
char rBuffer[255];
volatile int gpsIn=0;
volatile int rxSoftOut=0;
bool newGPSData=false;

int gps_datardy(void){
	if(newGPSData){
		return 1;
	}else{
		return 0;
	}
}

char* gps_data(void){
	if(newGPSData){
			newGPSData = false;
			return gpsBuffer;
	}
}

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
	gps.puts(PMTK_SET_NMEA_UPDATERATE_1HZ);
	gps.puts(PMTK_SET_NMEA_OUTPUT_ALLDATA);


}

void gps_print(void){
	if(newGPSData){
		newGPSData = false;
		printf("%s",gpsBuffer);
	}
}

void gps_parser(char* data){

	//printf("%s", data);
	switch (minmea_sentence_id(data, false)) {
		case MINMEA_SENTENCE_RMC: {
			struct minmea_sentence_rmc frame;
			if (minmea_parse_rmc(&frame, data)) {
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
				printf(INDENT_SPACES "$xxGGA: fix quality: %d\n\r", frame.fix_quality);
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
				printf(INDENT_SPACES "$xxGSV: message %d of %d\n\r", frame.msg_nr, frame.total_msgs);
				printf(INDENT_SPACES "$xxGSV: sattelites in view: %d\n\r", frame.total_sats);
				for (int i = 0; i < 4; i++)
					printf(INDENT_SPACES "$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n\r",
						frame.sats[i].nr,
						frame.sats[i].elevation,
						frame.sats[i].azimuth,
						frame.sats[i].snr);
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
				printf(INDENT_SPACES "$xxVTG: true track degrees = %f\n\r",
					   minmea_tofloat(&frame.true_track_degrees));
				printf(INDENT_SPACES "        magnetic track degrees = %f\n\r",
					   minmea_tofloat(&frame.magnetic_track_degrees));
				printf(INDENT_SPACES "        speed knots = %f\n\r",
						minmea_tofloat(&frame.speed_knots));
				printf(INDENT_SPACES "        speed kph = %f\n\r",
						minmea_tofloat(&frame.speed_kph));
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

//	//TIME
//	int hour = (data[7]-'0')*10;
//	hour += (data[8]-'0');
//	int minute = (data[9]-'0')*10;
//	minute += (data[10]-'0');
//	int second = (data[11]-'0')*10;
//	second += (data[12]-'0');
//	//LATITUDE
//	double lat = (data[18]-'0')*10;
//	lat +=(data[19]-'0');
//	lat +=(data[20]-'0')*.1;
//	lat +=(data[21]-'0')*.01;
//	lat +=(data[23]-'0')*.001;
//	lat +=(data[24]-'0')*.0001;
//	lat +=(data[25]-'0')*.00001;
//	lat +=(data[26]-'0')*.000001;
//	//N or S
//	char lat_hem = data[28];
//	//LONGITUDE
//	double lng = (data[30]-'0')*100;
//	lng +=(data[31]-'0')*10;
//	lng +=(data[32]-'0');
//	lng +=(data[33]-'0')*.1;
//	lng +=(data[34]-'0')*.01;
//	lng +=(data[36]-'0')*.001;
//	lng +=(data[37]-'0')*.0001;
//	lng +=(data[38]-'0')*.00001;
//	lng +=(data[39]-'0')*.000001;
//	//E or W
//	char lng_hem = data[41];
//	//Fix Quality
//	int qual = data[43];
//	//Sats in view
//	int num_sats = data[45];
//	//Altitude(meters MSL)
//	double alt = (data[52]-'0')*1000;
//	alt += (data[53]-'0')*100;
//	alt += (data[54]-'0')*10;
//	alt += (data[55]-'0');
//	alt += (data[57]-'0')*.1;

	//printf("str: %s\r",data);
	//printf("parse: %d:%d:%d--%i,%c %i,%c--%i--%c%c%c%c\r",hour,minute,second,long(lat*1000000),lat_hem,long(lng*1000000),lng_hem,long(alt*10),data[52],data[53],data[54],data[55]);
}

void rmc_parser(char *data){
	//TIME
	int hour = (data[7]-'0')*10;
	hour += (data[8]-'0');
	int minute = (data[9]-'0')*10;
	minute += (data[10]-'0');
	int second = (data[11]-'0')*10;
	second += (data[12]-'0');
	//Reciever Warning
	char R_warn = data[14];
	//LATITUDE
	double lat = (data[16]-'0')*10;
	lat +=(data[17]-'0');
	lat +=(data[18]-'0')*.1;
	lat +=(data[19]-'0')*.01;
	lat +=(data[21]-'0')*.001;
	lat +=(data[22]-'0')*.0001;
	lat +=(data[23]-'0')*.00001;
	lat +=(data[24]-'0')*.000001;
	//N or S
	char lat_hem = data[26];
	//LONGITUDE
	double lng = (data[28]-'0')*100;
	lng +=(data[29]-'0')*10;
	lng +=(data[30]-'0');
	lng +=(data[31]-'0')*.1;
	lng +=(data[32]-'0')*.01;
	lng +=(data[34]-'0')*.001;
	lng +=(data[35]-'0')*.0001;
	lng +=(data[36]-'0')*.00001;
	lng +=(data[37]-'0')*.000001;
	//E or W
	char lng_hem = data[39];



	printf("str: %s\n\r",data);
	printf("parse: %d:%d:%d--%i,%c  %i,%c\r",hour,minute,second,long(lat*1000000),lat_hem,long(lng*1000000),lng_hem);


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
		//printf(gpsBuffer);
	}
}
