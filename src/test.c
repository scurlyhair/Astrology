//
//  test.c
//  
//
//  Created by scurlyhair on 2019/1/24.
//

#include "test.h"

int main() {
    
    /*
     * 设置资源文件路径
     */
    // 获取当前路径
    char currentDir[100];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        perror("获取当前路径错误.");
        return -1;
    }
    char* swephDir = "/sweph";
    char* path;
    if((path = malloc(strlen(currentDir)+strlen(swephDir)+1)) != NULL){
        path[0] = '\0';   // ensures the memory is an empty string
        strcat(path,currentDir);
        strcat(path,swephDir);
        printf("资源路径: %s\n", path);
    } else {
        printf("路径拼接错误.");
        return -1;
    }
    
    // 设置资源文件路径
    swe_set_ephe_path(path);
    
    
    
    /*
     * 配置个人信息
     */
    int birth_year = 1990, birth_month = 02, birth_day = 23, birth_hour = 17, birth_min = 01;
    double birth_sec = 00;
    double birth_lat = 37.343903; // 纬度
    double birth_long = 111.767829; // 经度
    printf("出生日期: %04d.%02d.%02d at %02d:%02d:%.0f\n", birth_year, birth_month, birth_day, birth_hour, birth_min, birth_sec);
    printf("出生经纬度: long '%f', lat '%f'\n", birth_long, birth_lat);

    
    /*
     * 将本地时间转换为世界时间
     */
    double timezone = +8;
    int u_year, u_month, u_day, u_hour, u_min;
    double u_sec;
    
    swe_utc_time_zone(birth_year, birth_month, birth_day, birth_hour, birth_min, birth_sec, timezone, &u_year, &u_month, &u_day, &u_hour, &u_min, &u_sec);
    printf("世界时间: %04d.%02d.%02d at %02d:%02d:%.0f\n", u_year, u_month, u_day, u_hour, u_min, u_sec);

    
    /*
     * 世界时转换为儒略日
     */
    double et_jd, ut_jd;
    int greg_flag = SE_GREG_CAL;
    double times[2];
    char serr[256];
    
    swe_utc_to_jd (u_year, u_month, u_day, u_hour, u_min, u_sec, greg_flag, times, serr);
    et_jd = times[0];  /* this is ET (TT) */
    ut_jd = times[1];  /* this is UT (UT1) */
    printf("世界时儒略日数: %f\n", et_jd);
    printf("历书时儒略日数: %f\n", ut_jd);
    
    
    
    /*
     * 计算星体位置
     */
    char planet_name[100];
    double planet_params[6];
    int planet_number = SE_SUN;
    long iflag = 0; // If the speed of the body is required, set iflag = SEFLG_SPEED
    printf("星体名   \tlongitude\tlatitude\tdistance\n");
    // 计算星体位置
    swe_calc_ut(ut_jd, planet_number, iflag, planet_params, serr);
    // 获取星体名字
    swe_get_planet_name(planet_number, planet_name);
    printf("%10s\t%11.7f\t%10.7f\t%10.7f\n", planet_name, planet_params[0], planet_params[1],  planet_params[2]);
    
    
    
    /*
     * 计算宫位分布
     */
    char *houseMethod = "W"; // W: 整宫制。一宫30度，从上升点落入星座的0度开始第一宫，因此宫位和星座是完全重合的。
    int hsys = houseMethod[0];
    double cusps[13], ascmc[10];
    // 计算宫位
    swe_houses(ut_jd,birth_lat,birth_long,hsys,cusps,ascmc);
    int i;
    for (i = 1; i < 13; i++) {
        printf("第%d宫起点: %.0f°\n", i, cusps[i]);
    }
    printf("ASC: %.2f°\n", ascmc[SE_ASC]); // 上升点
    printf("MC: %.2f°\n", ascmc[SE_MC]); // 顶点
    printf("ARMC: %.2f°\n", ascmc[SE_ARMC]); // 恒星时间的度数
    
    /*
     * 计算星体在星盘的宫位
     */
    double eps = 23.437404; // 黄道倾角
    double xpin[2] = {planet_params[0], planet_params[1]};
    double house_pos, mund_pos;
    int house_index; // 第几宫
    
    // The function returns a value between 1.0 and 12.999999, indicating in which house a planet is and how far from its cusp it is.
    house_pos = swe_house_pos(ascmc[SE_ARMC], birth_lat, eps, hsys, xpin, serr);
    house_index = house_pos;
    mund_pos = (house_pos - house_index) * 30.0;
    printf("%s 在 %f\n", planet_name, house_pos);
    printf("%s 在第 %d 宫\n", planet_name, house_index);
    printf("%s 的宫位度数: %.2f°\n",planet_name, mund_pos);
    
    
    
    swe_close();
    return 1;
}


/**
 #define SE_ECL_NUT                              -1
 
 #define SE_SUN                                     0
 
 #define SE_MOON                                  1
 
 #define SE_MERCURY                             2
 
 #define SE_VENUS                                 3
 
 #define SE_MARS                                   4
 
 #define SE_JUPITER   木星                            5
 
 #define SE_SATURN     土星                          6
 
 #define SE_URANUS                               7
 
 #define SE_NEPTUNE                              8
 
 #define SE_PLUTO                                  9
 
 #define SE_MEAN_NODE                        10
 
 #define SE_TRUE_NODE                         11
 
 #define SE_MEAN_APOG                         12
 
 #define SE_OSCU_APOG                         13
 
 #define SE_EARTH                                  14
 
 #define SE_CHIRON                               15
 
 #define SE_PHOLUS                               16
 
 #define SE_CERES                                  17
 
 #define SE_PALLAS                                18
 
 #define SE_JUNO                                   19
 
 #define SE_VESTA                                  20
 
 #define SE_INTP_APOG                          21
 
 #define SE_INTP_PERG                          22
 
 
 
 #define SE_NPLANETS                            23
 
 #define SE_FICT_OFFSET                        40
 
 #define SE_NFICT_ELEM                        15
 
 #define SE_AST_OFFSET                        10000
 **/
