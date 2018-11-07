#include <iostream>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <string>
#include <thread>
#include "djicam.h"
#include "video_to_picture/video_to_picture.h"
#include "video_to_picture/ConvertUtil.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

#define LOCAL_HOST_INET_4_ADDR "127.0.0.1"
#define INET_UDP_BUFFER_LEN 1024

#define DJI_CAM_HEIGHT 720
#define DJI_CAM_WIDTH 1280

#define FRAME_SIZE              (DJI_CAM_WIDTH*DJI_CAM_HEIGHT*3/2)  /*format NV12*/
#define GET_PICTURE_TIME_INTERVAL 5 //sec.

static unsigned char buffer[FRAME_SIZE+8] = {0};

static unsigned int nframe = 0;

static time_t last_get_picture_time = 0;

void error_notify();
void not_root_notify();
bool check_is_root();
void writeToFile();
void socket_thread_method(int port_number);
void frame_clip_thread();
void udp_msg_sender(int, struct sockaddr*, string);


// 运行方式 sudo ./xxx -p [port_number] or --port [port_number]
int main(int argc, char **argv) {
    if(check_is_root()) {
        if(argc == 3) {
            string arg1 = string(argv[1]);
            if(arg1.find("-p") != std::string::npos ||
               arg1.find("--port") != std::string::npos) {
                int port_number = 0;
                try {
                    port_number = std::stoi(argv[2]);
                }catch (exception &e) {}
                if(port_number > 0 && port_number < 65536) {
                    thread * thread_socket = new thread(socket_thread_method, port_number);
                    thread_socket->detach();

                    thread * thread_read = new thread(frame_clip_thread);
                    thread_read->join();

                    delete thread_socket;
                    delete thread_read;
                }else {
                    error_notify();
                    cout << endl << "[port_number] must big than 1 and small than 65536" << endl;
                    cout << "your input:" << argv[2] << endl;
                }
            }else {
                error_notify();
            }
        }else {
            error_notify();
        }
    } else {
        not_root_notify();
    }
    return 0;
}

void socket_thread_method(int port_number) {
    cout << "init socket thread" << endl;
    cout << "listen port: " << port_number << endl;
    // 实际上这只是个client 负责把数据发到机器的端口上就可以了
    int udp_client_fd;
    struct sockaddr_in server_addr;
    udp_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_client_fd < 0){
        cout << "can not allocate fd" << endl;
        throw exception();
    }
    memset(&server_addr,0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST_INET_4_ADDR);
    server_addr.sin_port = htons(port_number);
    while (true) {
        // implement a sender

        //todo read identified data and pack it to json data

        string str = "udp test:" + std::to_string(time(NULL));
        //send
        udp_msg_sender(udp_client_fd, (struct sockaddr*)&server_addr,str);
        sleep(2);
    }
}

void udp_msg_sender(int fd, struct sockaddr* dst, string data) {
    int buffer_len = data.size();
    const char * send_buffer = data.c_str();
    sendto(fd,send_buffer,buffer_len,0,dst,(socklen_t)sizeof(*dst));
    cout << "you sent:" << data << endl;
}

void frame_clip_thread() {
    while (1) {
        sleep(1);
        cout << "test2:" << endl;
    }
}

//int main(int argc, char **argv) {
//    bool isRoot = check_is_root();
//    if(isRoot) {
//        int cam_mode = TRANSFER_MODE;
//        cam_mode |= GETBUFFER_MODE;
//        int ret = manifold_cam_init(cam_mode);
//        if(ret == -1) {
//            cout << "The camera cannot init" << endl;
//            return -1;
//        }
//        while (1) {
//            if(cam_mode & GETBUFFER_MODE) {
//                ret = manifold_cam_read(buffer, &nframe, CAM_NON_BLOCK);
//                if(ret > 0){
//                    time_t now_time;
//                    time(&now_time);
//                    if(now_time - last_get_picture_time >= GET_PICTURE_TIME_INTERVAL) {
//                        cout << "frame_size =" << ret << ", nframe = " << nframe << endl;
//                        writeToFile();
//                        last_get_picture_time = now_time;
//                    }
//                }else if(0 == ret){
////                printf("no new frame, ret = %d\n",ret);
//                }
//                else
//                    break;
//            }
//            if(manifold_cam_exit())
//                break;
//            usleep(10000);
//        }
//        while (!manifold_cam_exit()) {
//            sleep(1);
//        }
//    }else {
//        cout << "Please run it as root! " << endl;
//    }
//    return 0;
//}

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

bool check_is_root() {
    bool isRoot = true;
    if(0 != getuid()){
        isRoot = false;
    }
    return isRoot;
}

void error_notify() {
    cout << "Input Error, Example :" << endl;
    cout << "sudo ./xxx -p [port_number]" << endl;
    cout << "or" << endl;
    cout << "sudo ./xxx --port [port_number]" << endl;
}

void not_root_notify() {
    cout << "Please run it as root " << endl;
}