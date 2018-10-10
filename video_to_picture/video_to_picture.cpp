//
// Created by zhouxi on 7/10/2018.
//

#include "video_to_picture.h"

#include <unistd.h>
#include <fstream>
#include "ConvertUtil.h"

bool isFileExist(string path);

int convert_nv12_to_bmp(int width_pixel, int height_pixel, string nv12_path, string save_path){
    int status = convert_nv_12_to_bmp_status_failed;
    int file_exist = isFileExist(nv12_path);
    if(true == file_exist) {
        // set buffer for nv12
        int file_size = width_pixel * height_pixel * 3 / 2;
        char * buffer = new char[file_size];
        // read file
        ifstream in_stream;
        in_stream.open(nv12_path.c_str(),std::ifstream::binary);
        bool file_is_open = in_stream.is_open();
        if(!file_is_open) {
            status = convert_nv_12_to_bmp_status_file_not_exist_or_not_permission;
        }else {
            in_stream.read(buffer,file_size);
            in_stream.close();
            cout << "input file size: " << strlen(buffer) << endl;
            // define a out buffer
            int out_buffer_size = width_pixel * height_pixel * 3 + Bmp_FileHeader_Size + Bmp_Info_Size;
            char * out_buffer = new char[out_buffer_size];
            status = convert_nv12_to_bmp_buffer(width_pixel,height_pixel,buffer,out_buffer);
            if(status == convert_nv_12_to_bmp_status_success){
                ofstream out_stream;
                out_stream.open(save_path, std::ofstream::binary);
                out_stream.write(out_buffer,out_buffer_size);
                out_stream.close();
                delete [] out_buffer;
                out_buffer = NULL;
            }
            delete [] buffer;
            buffer = NULL;
        }
    } else {
        status = convert_nv_12_to_bmp_status_file_not_exist_or_not_permission;
    }
    return status;
}

char * memcat(char *dest, size_t dest_len, const char *src, size_t src_len) {
    memcpy(dest+dest_len, src, src_len);
    return dest;
}

int convert_nv12_to_bmp_buffer(int width_pixel, int height_pixel, char * in_buffer, char * out_buffer){
    int status = convert_nv_12_to_bmp_status_failed;
    if(in_buffer != NULL && out_buffer != NULL) {
        int file_size = width_pixel * height_pixel * 3 / 2;
        char * yv12buffer = (char *) malloc(file_size +1);
        memcpy(yv12buffer,in_buffer,file_size);
        // convert NV12 to YV12
        convertNV12ToYV12((uint8_t *)in_buffer,(uint8_t *)yv12buffer,width_pixel,height_pixel);
        // convert YV12 to BGR
        int bgr_size = width_pixel * height_pixel * 3;
        char * bgrbuffer = (char *) malloc(bgr_size + 1);
        bool isSuccessToConvertBGR = convertYV12ToBGR24_Table((uint8_t *)yv12buffer,(uint8_t *)bgrbuffer,width_pixel,height_pixel);
        if(isSuccessToConvertBGR) {
            delete [] yv12buffer;
            yv12buffer = NULL;
            char * bmp_header_buffer = (char *)malloc(Bmp_FileHeader_Size + Bmp_FileHeader_Size + bgr_size + 100);// fix a problem with linux buffer overflow
            memcat(bmp_header_buffer,Bmp_FileHeader_Size + Bmp_Info_Size,bgrbuffer,bgr_size);
            createBMPHeader((uint8_t *)bmp_header_buffer,width_pixel,height_pixel,bgr_size);
            memcpy(out_buffer,bmp_header_buffer,Bmp_Info_Size+Bmp_FileHeader_Size+bgr_size);
            cout << "output file size: " << Bmp_Info_Size+Bmp_FileHeader_Size+bgr_size << endl;
            delete [] bmp_header_buffer;
            delete [] bgrbuffer;
            bmp_header_buffer = NULL;
            bgrbuffer = NULL;
            status = convert_nv_12_to_bmp_status_success;
        } else{
            status = convert_nv_12_to_bmp_status_failed;
        }
    }else {
        status = convert_nv_12_to_bmp_status_file_not_exist_or_not_permission;
    }
    return status;
}

int convert_dji_camera_nv12_to_bmp(string nv12_path, string save_path) {
    int status = convert_nv12_to_bmp(1280,720,nv12_path,save_path);
    return status;
}

/**
 * check is file exists
 * @param path
 * @return
 */
bool isFileExist(string path) {
    bool status = false;
    int access_result = access(path.c_str(),F_OK);
    if(0 == access_result) {
        status = true;
    }
    return status;
}

