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
//        printf("资源路径: %s\n", path);
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
    double birth_sec = 3.0;
    double birth_lat = 37.343903; // 纬度 北纬+ 南纬-
    double birth_long = 111.767829; // 经度 东经+ 西经-
    printf("时间: %04d.%02d.%02d %02d:%02d:%02.0f\n", birth_year, birth_month, birth_day, birth_hour, birth_min, birth_sec);
    printf("经度: %f, 纬度: %f\n\n", birth_long, birth_lat);

    
    /*
     * 将本地时间转换为世界时间
     */
    double timezone = +8; // 东部时区+ 西部时区-
    int u_year, u_month, u_day, u_hour, u_min;
    double u_sec;
    
    swe_utc_time_zone(birth_year, birth_month, birth_day, birth_hour, birth_min, birth_sec, timezone, &u_year, &u_month, &u_day, &u_hour, &u_min, &u_sec);
//    printf("世界时间: %04d.%02d.%02d at %02d:%02d:%.0f\n", u_year, u_month, u_day, u_hour, u_min, u_sec);

    
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
//    printf("世界时儒略日数: %f\n", et_jd);
//    printf("历书时儒略日数: %f\n\n", ut_jd);
    
    
    
    
    /*
     * 计算宫位分布
     */
    char *houseMethod = "W"; // W: 整宫制。一宫30度，从上升点落入星座的0度开始第一宫，因此宫位和星座是完全重合的。
    int hsys = houseMethod[0];
    double cusps[13], ascmc[10];
    char *lifehouses[12] = {"命宫","财帛宫","兄弟宫","田宅宫","男女宫","奴仆宫","妻妾宫","疾厄宫","迁移宫","官禄宫","福德宫","相貌宫"};
    // 计算宫位
    swe_houses(ut_jd,birth_lat,birth_long,hsys,cusps,ascmc);
    
    char t0_title_house[100];
    char t0_content_degree[100];
    char t0_content_lifehouse[150];
    char t0_house_str[10];
    char t0_degree_str[10];
    char t0_lifehouse_str[20];

    for (int i = 1; i <= 12; i++) {
        sprintf(t0_house_str, "\t第%d宫", i);// 宫位
        strcat(t0_title_house, t0_house_str);

        sprintf(t0_degree_str, "\t%.0f°", cusps[i]);// 宫头度数
        strcat(t0_content_degree, t0_degree_str);

        sprintf(t0_lifehouse_str, "\t%s", lifehouses[i - 1]); // 命理十二宫
        strcat(t0_content_lifehouse, t0_lifehouse_str);
    }
    printf("宫位  |%s\n", t0_title_house);
    printf("宫头  |%s\n", t0_content_degree);
    printf("命理宫|%s\n\n", t0_content_lifehouse);
    
//    printf("上升点(ASC): %.2f°\n\n", ascmc[SE_ASC]); // 上升点
//    printf("顶点(MC): %.2f°\n", ascmc[SE_MC]); // 顶点
//    printf("ARMC: %.2f°\n\n", ascmc[SE_ARMC]); // 恒星时间的度数
    
    
    
    /*
     * 计算星体位置
     */
    long planet_numbers[10] = {SE_SUN,SE_MOON,SE_MERCURY,SE_MARS,SE_JUPITER,SE_VENUS,SE_SATURN,SE_MEAN_NODE,SE_AST_OFFSET + 1181,SE_FICT_OFFSET_1 + 1};
    char *planet_names[10] = {"太阳","月亮","水星","火星","木星","金星","土星","罗睺","月孛","紫气"};
    
    double planet_params[6], xpin[2];;
    double eps = 23.437404; // 黄道倾角
    double house_pos, house_degree, planet_long, planet_lat;
    double armc = ascmc[SE_ARMC];
    int planet_number, house_index;
    long iflag = 0; // If the speed of the body is required, set iflag = SEFLG_SPEED
    
    char t1_title_planet[100];
    char t1_content_house[100];
    char t1_content_degree[100];

    char t1_planet_name_str[15];
    char t1_house_str[10];
    char t1_degree_str[10];

    for (int i = 0; i < 10; i++) {
        planet_number = planet_numbers[i];
        // 计算星体位置
        swe_calc_ut(ut_jd, planet_number, iflag, planet_params, serr);
        planet_long = planet_params[0];
        planet_lat = planet_params[1];
        xpin[0] = planet_long;
        xpin[1] = planet_lat;

        // The function returns a value between 1.0 and 12.999999, indicating in which house a planet is and how far from its cusp it is.
        house_pos = swe_house_pos(armc, birth_lat, eps, hsys, xpin, serr);
        house_index = house_pos;
        house_degree = (house_pos - house_index) * 30.0;
//        printf("%s\n", serr);
        
        sprintf(t1_planet_name_str, "\t%s", planet_names[i]);// 星体
        strcat(t1_title_planet, t1_planet_name_str);
        
        sprintf(t1_house_str, "\t第%d宫", house_index);// 宫位
        strcat(t1_content_house, t1_house_str);
        
        sprintf(t1_degree_str, "\t%.2f°", house_degree);// 所在宫位的度数
        strcat(t1_content_degree, t1_degree_str);
    }
    
    printf("星体|%s\n", t1_title_planet);
    printf("宫位|%s\n", t1_content_house);
    printf("度数|%s\n\n", t1_content_degree);
    
    
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
