//
// Created by tang on 7/4/19.
//

#include <libuvc/libuvc.h>
#include <folly/MPMCQueue.h>

#include <memory>

#ifndef SKAGWAY2_CAMERA_H
#define SKAGWAY2_CAMERA_H

namespace Skagway2 {

struct Frame;

typedef folly::MPMCQueue<std::shared_ptr<Frame>> FrameQueue;

// A cpp wrapper of uvc_frame
struct Frame {
    explicit Frame(uvc_frame_t *uvc_frame);
    Frame(const Frame &frame) = delete;
    Frame(const Frame &&frame) = delete;

    ~Frame();
    uvc_frame_t *uvc_frame;
};

class CameraError : public std::runtime_error {
public:
    explicit CameraError(const char* msg) : std::runtime_error(msg) {}
};

class Camera {
public:
    explicit Camera(
            uvc_context_t *ctx,
            int vid,
            int pid,
            uvc_frame_format format = UVC_FRAME_FORMAT_MJPEG,
            int height = 720,
            int width = 1280,
            int fps = 30);
    ~Camera();
    void start_streaming();
    void stop_streaming();

    FrameQueue& get_up_queue();


    uvc_context_t *ctx{};
    int vid;
    int pid;
    uvc_frame_format format;
    int height;
    int width;
    int fps;

    uvc_device_t *dev = nullptr;
    uvc_device_handle_t *devh = nullptr;
    uvc_stream_ctrl_t ctrl;

    FrameQueue up_queue;
    std::chrono::milliseconds queue_write_timeout;

    void init();
};

}


#endif //SKAGWAY2_CAMERA_H
