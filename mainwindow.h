#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "comm_def.h"
/* Parameter initial */
void Cam_Param_Init(void);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Data_init(void);

private slots:
    void on_cam0_bin_inv_clicked();

    void on_cam0_bin_th_dec_clicked();

    void on_cam0_bin_th_inc_clicked();

    void on_cam0_bin_threshold_slider_valueChanged(int value);

    void on_cam0_ero_rnd_dec_clicked();

    void on_cam0_ero_rnd_inc_clicked();

    void on_cam0_ero_round_slider_valueChanged(int value);

    void on_cam0_dia_rnd_dec_clicked();

    void on_cam0_dia_rnd_inc_clicked();

    void on_cam0_dia_round_slider_valueChanged(int value);

    void on_cam0_ab_area_high_dec_clicked();

    void on_cam0_ab_area_high_inc_clicked();

    void on_cam0_abnormal_area_high_slider_valueChanged(int value);

    void on_cam0_ab_area_low_dec_clicked();

    void on_cam0_ab_area_low_inc_clicked();

    void on_cam0_abnormal_area_low_slider_valueChanged(int value);

    void on_cam0_src_pic_open_bt_clicked();

    void on_cam0_cfg_file_reload_clicked();

    void on_cam0_cfg_local_save_clicked();

    void on_cam1_bin_inv_clicked();

    void on_cam1_bin_th_dec_clicked();

    void on_cam1_bin_th_inc_clicked();

    void on_cam1_bin_threshold_slider_valueChanged(int value);

    void on_cam1_ero_rnd_dec_clicked();

    void on_cam1_ero_rnd_inc_clicked();

    void on_cam1_ero_round_slider_valueChanged(int value);

    void on_cam1_dia_rnd_dec_clicked();

    void on_cam1_dia_rnd_inc_clicked();

    void on_cam1_dia_round_slider_valueChanged(int value);

    void on_cam1_ab_area_high_dec_clicked();

    void on_cam1_ab_area_high_inc_clicked();

    void on_cam1_abnormal_area_high_slider_valueChanged(int value);

    void on_cam1_ab_area_low_dec_clicked();

    void on_cam1_ab_area_low_inc_clicked();

    void on_cam1_abnormal_area_low_slider_valueChanged(int value);

    void on_cam1_src_pic_open_bt_clicked();

    void on_cam1_cfg_file_reload_clicked();

    void on_cam1_cfg_local_save_clicked();

    void on_cam2_bin_inv_clicked();

    void on_cam2_bin_th_dec_clicked();

    void on_cam2_bin_th_inc_clicked();

    void on_cam2_bin_threshold_slider_valueChanged(int value);

    void on_cam2_ero_rnd_dec_clicked();

    void on_cam2_ero_rnd_inc_clicked();

    void on_cam2_ero_round_slider_valueChanged(int value);

    void on_cam2_dia_rnd_dec_clicked();

    void on_cam2_dia_rnd_inc_clicked();

    void on_cam2_dia_round_slider_valueChanged(int value);

    void on_cam2_ab_area_high_dec_clicked();

    void on_cam2_ab_area_high_inc_clicked();

    void on_cam2_abnormal_area_high_slider_valueChanged(int value);

    void on_cam2_ab_area_low_dec_clicked();

    void on_cam2_ab_area_low_inc_clicked();

    void on_cam2_abnormal_area_low_slider_valueChanged(int value);

    void on_cam2_src_pic_open_bt_clicked();

    void on_cam2_cfg_file_reload_clicked();

    void on_cam2_cfg_local_save_clicked();

    void on_cam3_bin_inv_clicked();

    void on_cam3_bin_th_dec_clicked();

    void on_cam3_bin_th_inc_clicked();

    void on_cam3_bin_threshold_slider_valueChanged(int value);

    void on_cam3_ero_rnd_dec_clicked();

    void on_cam3_ero_rnd_inc_clicked();

    void on_cam3_ero_round_slider_valueChanged(int value);

    void on_cam3_dia_rnd_dec_clicked();

    void on_cam3_dia_rnd_inc_clicked();

    void on_cam3_dia_round_slider_valueChanged(int value);

    void on_cam3_ab_area_high_dec_clicked();

    void on_cam3_ab_area_high_inc_clicked();

    void on_cam3_abnormal_area_high_slider_valueChanged(int value);

    void on_cam3_ab_area_low_dec_clicked();

    void on_cam3_ab_area_low_inc_clicked();

    void on_cam3_abnormal_area_low_slider_valueChanged(int value);

    void on_cam3_src_pic_open_bt_clicked();

    void on_cam3_cfg_file_reload_clicked();

    void on_cam3_cfg_local_save_clicked();

    void on_cam0_abnormal_area_high_value_returnPressed();
    void on_cam0_abnormal_area_low_value_returnPressed();
    void on_cam1_abnormal_area_high_value_returnPressed();
    void on_cam1_abnormal_area_low_value_returnPressed();
    void on_cam2_abnormal_area_high_value_returnPressed();
    void on_cam2_abnormal_area_low_value_returnPressed();
    void on_cam3_abnormal_area_high_value_returnPressed();
    void on_cam3_abnormal_area_low_value_returnPressed();
    void on_cam0_check_area_p1_clicked();
    void on_cam0_check_area_p2_clicked();
    void on_cam0_check_area_p3_clicked();
    void on_cam0_check_area_p4_clicked();
    void on_cam1_check_area_p1_clicked();
    void on_cam1_check_area_p2_clicked();
    void on_cam1_check_area_p3_clicked();
    void on_cam1_check_area_p4_clicked();
    void on_cam2_check_area_p1_clicked();
    void on_cam2_check_area_p2_clicked();
    void on_cam2_check_area_p3_clicked();
    void on_cam2_check_area_p4_clicked();
    void on_cam3_check_area_p1_clicked();
    void on_cam3_check_area_p2_clicked();
    void on_cam3_check_area_p3_clicked();
    void on_cam3_check_area_p4_clicked();
    void on_cam0_check_area_point_up_clicked();
    void on_cam0_check_area_point_down_clicked();
    void on_cam0_check_area_point_left_clicked();
    void on_cam0_check_area_point_right_clicked();
    void on_cam1_check_area_point_up_clicked();
    void on_cam1_check_area_point_down_clicked();
    void on_cam1_check_area_point_left_clicked();
    void on_cam1_check_area_point_right_clicked();
    void on_cam2_check_area_point_up_clicked();
    void on_cam2_check_area_point_down_clicked();
    void on_cam2_check_area_point_left_clicked();
    void on_cam2_check_area_point_right_clicked();
    void on_cam3_check_area_point_up_clicked();
    void on_cam3_check_area_point_down_clicked();
    void on_cam3_check_area_point_left_clicked();
    void on_cam3_check_area_point_right_clicked();
    void on_cam0_check_area_point_hslider_valueChanged(int value);
    void on_cam0_check_area_point_vslider_valueChanged(int value);
    void on_cam1_check_area_point_hslider_valueChanged(int value);
    void on_cam1_check_area_point_vslider_valueChanged(int value);
    void on_cam2_check_area_point_hslider_valueChanged(int value);
    void on_cam2_check_area_point_vslider_valueChanged(int value);
    void on_cam3_check_area_point_hslider_valueChanged(int value);
    void on_cam3_check_area_point_vslider_valueChanged(int value);

    void on_cam_gpio_output_pluse_width_valueChanged(int value);

    void on_cam_gpio_pluse_width_returnPressed();

    void on_cam0_color_channel_clicked();
    void on_cam0_r_channel_clicked();
    void on_cam0_g_channel_clicked();
    void on_cam0_b_channel_clicked();
    void on_cam1_color_channel_clicked();
    void on_cam1_r_channel_clicked();
    void on_cam1_g_channel_clicked();
    void on_cam1_b_channel_clicked();
    void on_cam2_color_channel_clicked();
    void on_cam2_r_channel_clicked();
    void on_cam2_g_channel_clicked();
    void on_cam2_b_channel_clicked();
    void on_cam3_color_channel_clicked();
    void on_cam3_r_channel_clicked();
    void on_cam3_g_channel_clicked();
    void on_cam3_b_channel_clicked();

    void on_cam0_otus_binary_clicked();
    void on_cam1_otus_binary_clicked();
    void on_cam2_otus_binary_clicked();
    void on_cam3_otus_binary_clicked();

    void on_actioncam0_cfg_open_triggered();
    void on_actioncam0_cfg_save_triggered();
    void on_actioncam1_cfg_open_triggered();
    void on_actioncam1_cfg_save_triggered();
    void on_actioncam2_cfg_open_triggered();
    void on_actioncam2_cfg_save_triggered();
    void on_actioncam3_cfg_open_triggered();
    void on_actioncam3_cfg_save_triggered();
private:
    Ui::MainWindow *ui;
    QImage ScaleImage[CAM_NUM_MAX];
    CFG_PARAM cfg_param[CAM_NUM_MAX];
    int PicLoadFlag[CAM_NUM_MAX];
    int AreaSelectActivePoint[CAM_NUM_MAX];
    int Cam_Config_File_Load(unsigned int cam_id);
    void ShowPorcessedPic(unsigned int cam_id, QImage *qBinImage);
    void ShowSourcePic(unsigned int cam_id, QImage *qBinImage);
    void ResultSet(unsigned int cam_id, const QString *result);
    QString SourePicPathGet(unsigned int cam_id);
    int Pic_Process(unsigned int cam_id);
    void Cam0_Ui_Init(void);
    void Cam1_Ui_Init(void);
    void Cam2_Ui_Init(void);
    void Cam3_Ui_Init(void);
    void Cam_Param_Init(void);
    void handleResults(const QString &s);
};

#endif // MAINWINDOW_H
