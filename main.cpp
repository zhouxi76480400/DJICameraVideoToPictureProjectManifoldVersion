#include <iostream>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <string>
#include "djicam.h"
#include "video_to_picture/video_to_picture.h"
#include "video_to_picture/ConvertUtil.h"

using namespace std;

#define DJI_CAM_HEIGHT 720
#define DJI_CAM_WIDTH 1280

#define FRAME_SIZE              (DJI_CAM_WIDTH*DJI_CAM_HEIGHT*3/2)  /*format NV12*/
#define GET_PICTURE_TIME_INTERVAL 5 //sec.

static unsigned char buffer[FRAME_SIZE+8] = {0};

static unsigned int nframe = 0;

static time_t last_get_picture_time = 0;

bool check_is_root();
void writeToFile();

int main(int argc, char **argv) {
    bool isRoot = check_is_root();
    if(isRoot) {
        int cam_mode = TRANSFER_MODE;
        cam_mode |= GETBUFFER_MODE;
        int ret = manifold_cam_init(cam_mode);
        if(ret == -1) {
            cout << "The camera cannot init" << endl;
            return -1;
        }
        while (1) {
            if(cam_mode & GETBUFFER_MODE) {
                ret = manifold_cam_read(buffer, &nframe, CAM_NON_BLOCK);
                if(ret > 0){
                    time_t now_time;
                    time(&now_time);
                    if(now_time - last_get_picture_time >= GET_PICTURE_TIME_INTERVAL) {
                        cout << "frame_size =" << ret << ", nframe = " << nframe << endl;
                        writeToFile();
                        last_get_picture_time = now_time;
                    }
                }else if(0 == ret){
//                printf("no new frame, ret = %d\n",ret);
                }
                else
                    break;
            }
            if(manifold_cam_exit())
                break;
            usleep(10000);
        }
        while (!manifold_cam_exit()) {
            sleep(1);
        }
    }else {
        cout << "Please run it as root! " << endl;
    }
    return 0;
}

bool check_is_root() {
    bool isRoot = true;
    if(0 != getuid()){
        isRoot = false;
    }
    return isRoot;
}

void writeToFile() {
    int bmp_size = Bmp_FileHeader_Size + Bmp_Info_Size + DJI_CAM_HEIGHT * DJI_CAM_WIDTH * 3;
    char * bmpBuffer = new char[bmp_size];
    int convert_status = convert_nv12_to_bmp_buffer(DJI_CAM_WIDTH,DJI_CAM_HEIGHT,(char *)buffer,bmpBuffer);
    cout << "convert_status:" << convert_status << endl;
    if(convert_status == convert_nv_12_to_bmp_status_success){
        ofstream out_stream;
        string savePath = "/home/ubuntu/cam_out/" + to_string(last_get_picture_time) +".bmp";
        out_stream.open(savePath, std::ofstream::binary);
        out_stream.write(bmpBuffer,bmp_size);
        out_stream.close();
    }
    delete [] bmpBuffer;
    bmpBuffer = NULL;
}