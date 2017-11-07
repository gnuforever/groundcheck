#ifndef COMM_DEF_H
#define COMM_DEF_H

#define ERR_CAM_ID          0x1
#define ERR_CFG_FILE_NONE   0x2
#define ERR_NO_PIC_LOAD     0x3

#define CAM_NUM_MAX  4
typedef struct {
    int invert_flag;   //二值化是否反向
    int bin_threshold; //二值化阈值
    int otus_flag;     //大津律阈值使能标识
    int ero_round;     //腐蚀次数
    int dia_round;     //膨胀次数
    int abnormal_area_low; //异常面积下限
    int abnormal_area_high;//异常面积上限
    int pinout_ms;      //管脚输出时间
    int channel;        //0:color,1:r,2:g,3:b
    CvPoint checkarea[4]; //0:左上角，1：右上角，2：右下角，3：左下角
}CFG_PARAM;
#define CAM0_CFG_FILE "/cam0_cfg.txt"
#define CAM1_CFG_FILE "/cam1_cfg.txt"
#define CAM2_CFG_FILE "/cam2_cfg.txt"
#define CAM3_CFG_FILE "/cam3_cfg.txt"

typedef struct {
    unsigned cam_id;
    char cfg_file_name[16];
}CAM_CFG_FILE;

#endif // COMM_DEF_H
