//
// Created by zhouxi on 7/10/2018.
//

#ifndef DJICAMERAVIDEOTOPICTUREPROJECT_VIDEO_TO_PICTURE_H
#define DJICAMERAVIDEOTOPICTUREPROJECT_VIDEO_TO_PICTURE_H

#include <iostream>
#include <string>

using namespace std;

#define convert_nv_12_to_bmp_status_success  0

#define convert_nv_12_to_bmp_status_failed  1

#define convert_nv_12_to_bmp_status_file_not_exist_or_not_permission 2;

/**
 * convert method
 * @param nv12_path
 * @param save_path
 * @return
 */
int convert_nv12_to_bmp(int with_pixel, int height_pixel, string nv12_path, string save_path);

/**
 * convert method with buffer
 * @param width_pixel
 * @param height_pixel
 * @param in_buffer
 * @param out_buffer
 * @return
 */
int convert_nv12_to_bmp_buffer(int width_pixel, int height_pixel, char * in_buffer, char * out_buffer);

/**
 * only for dji z3 x3
 * @param nv12_path
 * @param save_path
 * @return
 */
int convert_dji_camera_nv12_to_bmp(string nv12_path, string save_path);


#endif //DJICAMERAVIDEOTOPICTUREPROJECT_VIDEO_TO_PICTURE_H
