//
// Created by tang on 7/5/19.
//

#include <libusb.h>
#include <folly/ScopeGuard.h>
#include <folly/container/F14Map.h>
#include <glog/logging.h>
#include <libuvc/libuvc.h>
#include <experimental/filesystem>


#include "Camera.h"
#include "FrameWriter.h"

typedef std::pair<uint16_t, uint16_t> DeviceId;

std::vector<DeviceId> get_user_devices () {
    return {{0x0c45, 0x671d}, {0x058f, 0x0362}};
}

std::string format_device_id(DeviceId &id) {
    char buf[14];
    sprintf(buf, "0x%04x:0x%04x", id.first, id.second);
    return std::string(buf);
}

enum class USBEventType {
    DEVICE_LEFT,
};

struct USBEventMessage {
    USBEventType type;
    DeviceId deviceId;
};

struct CameraInfo {
    std::unique_ptr<Skagway2::Camera> camera = nullptr;
    std::unique_ptr<Skagway2::FrameWriter> frame_writer = nullptr;
};

int main() {
    libusb_context *usb_ctx = nullptr;
    uvc_context_t *uvc_ctx = nullptr;
    SCOPE_EXIT {
        if (usb_ctx) {
            libusb_exit(usb_ctx);
        }
        if (uvc_ctx) {
            uvc_exit(uvc_ctx);
        }
    };
    int usb_res = libusb_init(&usb_ctx);
    if (usb_res != LIBUSB_SUCCESS) {
        LOG(FATAL) << "cannot initialize libusb context";
    }
    uvc_error_t uvc_res = uvc_init(&uvc_ctx, usb_ctx);
    if (uvc_res < 0) {
        uvc_perror(uvc_res, "uvc_init");
        LOG(FATAL) << "cannot initialize libuvc context";
    }

    auto user_devices = get_user_devices();
    folly::F14FastMap<DeviceId, CameraInfo> cameras;
    for (auto &device_id : user_devices) {
        try {
            auto cur_camera = std::make_unique<Skagway2::Camera>(uvc_ctx, device_id.first, device_id.second);
            if (cameras.count(device_id)) {
                LOG(ERROR) << "duplicated device id " << format_device_id(device_id);
            } else {
                cameras[device_id].camera = std::move(cur_camera);
                auto cur_frame_writer = std::make_unique<Skagway2::FrameWriter>(
                        "/home/tang/test/" + format_device_id(device_id) + "/",
                        cameras[device_id].camera->get_up_queue());
                cameras[device_id].frame_writer = std::move(cur_frame_writer);
                cameras[device_id].camera->start_streaming();
                cameras[device_id].frame_writer->start_streaming();

            }
        } catch (const Skagway2::CameraError &error) {
            LOG(ERROR) << "cannot create device " << format_device_id(device_id);
            LOG(ERROR) << error.what();
        }
    }

    std::thread event_thread([usb_ctx](){
        for (int i = 0; i < 10000; i++) {
            libusb_handle_events_completed(usb_ctx, nullptr);
        }
    });

    event_thread.join();


}

