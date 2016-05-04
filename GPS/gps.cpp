#include "mbed.h"
#include "gps.h"
#include <string.h>
#include <strings.h>
#define PMTK_SET_NMEA_UPDATERATE_1HZ "$PMTK220,1000*1F\r\n"
#define PMTK_SET_NMEA_UPDATERATE_5HZ "$PMTK220,200*2C\r\n"
#define PMTK_SET_NMEA_UPDATERATE_10HZ "$PMTK220,100*2F\r\n"
#define PMTK_SET_NMEA_BAUDRATE_9600 "$PMTK251,9600*17\r\n"
#define PMTK_SET_NMEA_BAUDRATE_115200  "$PMTK251,115200*1F\r\n"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n"


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

void gga_parser(char* data){
	//TIME
	int hour = (data[7]-'0')*10;
	hour += (data[8]-'0');
	int minute = (data[9]-'0')*10;
	minute += (data[10]-'0');
	int second = (data[11]-'0')*10;
	second += (data[12]-'0');
	//LATITUDE
	double lat = (data[18]-'0')*10;
	lat +=(data[19]-'0');
	lat +=(data[20]-'0')*.1;
	lat +=(data[21]-'0')*.01;
	lat +=(data[23]-'0')*.001;
	lat +=(data[24]-'0')*.0001;
	lat +=(data[25]-'0')*.00001;
	lat +=(data[26]-'0')*.000001;
	//N or S
	char lat_hem = data[28];
	//LONGITUDE
	double lng = (data[30]-'0')*100;
	lng +=(data[31]-'0')*10;
	lng +=(data[32]-'0');
	lng +=(data[33]-'0')*.1;
	lng +=(data[34]-'0')*.01;
	lng +=(data[36]-'0')*.001;
	lng +=(data[37]-'0')*.0001;
	lng +=(data[38]-'0')*.00001;
	lng +=(data[39]-'0')*.000001;
	//E or W
	char lng_hem = data[41];
	//Fix Quality
	int qual = data[43];
	//Sats in view
	int num_sats = data[45];
	//Altitude(meters MSL)
	double alt = (data[52]-'0')*1000;
	alt += (data[53]-'0')*100;
	alt += (data[54]-'0')*10;
	alt += (data[55]-'0');
	alt += (data[57]-'0')*.1;

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
		gpsIn=0;
		newGPSData = true;
		char header[6] = {gpsBuffer[0],gpsBuffer[1],gpsBuffer[2],gpsBuffer[3],gpsBuffer[4],gpsBuffer[5]};
		if(header[3]=='G' && header[4]=='G' && header[5]=='A'){
			gga_parser(gpsBuffer);
		}
		if(header[3]=='R'&&header[4]=='M'&&header[5]=='C'){
			rmc_parser(gpsBuffer);
		}
		if(header[3]=='V'&&header[4]=='T'&&header[5]=='G'){
//			gps_print();
		}
	}
}
