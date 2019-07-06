//
// Created by tang on 7/4/19.
//

#include "Camera.h"

#include "glog/logging.h"

namespace Skagway2 {

void streaming_cb(uvc_frame_t *frame, void *ptr) {
    uvc_error_t res;
    frame->library_owns_data = 0;
    auto *camera = (Camera*)ptr;
    auto *new_frame = uvc_allocate_frame(frame->data_bytes);
    res = uvc_duplicate_frame(frame, new_frame);
    if (res < 0) {
        LOG(FATAL) << "cannot duplicate frame";
    }
    std::shared_ptr<Frame> frameWrapped = std::make_shared<Frame>(new_frame);
    camera->up_queue.tryWriteUntil(std::chrono::steady_clock::now() + camera->queue_write_timeout, frameWrapped);
}

Frame::Frame(uvc_frame_t *uvc_frame) {
    this->uvc_frame = uvc_frame;
}

Frame::~Frame() {
    uvc_free_frame(this->uvc_frame);
}

Camera::Camera(
        uvc_context_t *ctx,
        int vid, int pid,
        uvc_frame_format format,
        int height,
        int width,
        int fps) : up_queue(900) {
    this->ctx = ctx;
    this->vid = vid;
    this->pid = pid;
    this->format = format;
    this->height = height;
    this->width = width;
    this->fps = fps;
    this->queue_write_timeout = std::chrono::milliseconds(1000 / this->fps / 2);

    this->init();
}

FrameQueue &Camera::get_up_queue() {
    return this->up_queue;
}

void Camera::init() {
    uvc_error_t res;
    res = uvc_find_device(this->ctx, &this->dev, this->vid, this->pid, nullptr);
    if (res < 0) {
        throw CameraError(uvc_strerror(res));
    }
    res = uvc_open(this->dev, &this->devh);
    if (res < 0) {
        throw CameraError(uvc_strerror(res));
    }
    res = uvc_get_stream_ctrl_format_size(
            this->devh,
            &this->ctrl,
            this->format,
            this->width,
            this->height,
            this->fps);
    if (res < 0) {
        throw CameraError(uvc_strerror(res));
    }
}

void Camera::start_streaming() {
    uvc_error_t res;
    res = uvc_start_streaming(this->devh, &this->ctrl, streaming_cb, (void*)this, 0);
    if (res < 0) {
        throw CameraError(uvc_strerror(res));
    }
    uvc_set_ae_mode(devh, 2);
}

void Camera::stop_streaming() {
    uvc_stop_streaming(this->devh);
}


Camera::~Camera() {
    if (this->devh) {
        uvc_stop_streaming(this->devh);
        uvc_close(this->devh);
    }
    if (this->dev) {
        uvc_unref_device(this->dev);
    }
}

}
