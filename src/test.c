//
//  test.c
//  
//
//  Created by scurlyhair on 2019/1/24.
//

#include "test.h"

int main() {
    printf("start running!\n");
    
    int32 iyear = 1990;
    int32 imonth = 02;
    int32 iday = 23;
    int32 ihour = 17;
    int32 imin = 00;
    double dsec = 00;
    double d_timezone = +8;
    int32 iyear_out;
    int32 imonth_out;
    int32 iday_out;
    int32 ihour_out;
    int32 imin_out;
    double dsec_out;
    
    // convert Local time to UTC time
    swe_utc_time_zone(iyear, imonth, iday, ihour, imin, dsec, d_timezone, &iyear_out, &imonth_out, &iday_out, &ihour_out, &imin_out, &dsec_out);
    
    printf("UIC时间为: %d年 %d月 %d日 %d:%d:%f\n", iyear_out, imonth, iday_out, ihour_out, imin_out, dsec_out);
    
    printf("end!\n");
}
