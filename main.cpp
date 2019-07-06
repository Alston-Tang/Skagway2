#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <libuvc/libuvc.h>
#include <zconf.h>

#include "Camera.h"

#include <sys/types.h>
#include <sys/syscall.h>
#include <thread>


typedef std::pair<uint16_t, uint16_t> DeviceId;

std::vector<DeviceId> get_user_devices () {
    return {{0x0c45, 0x671d}};
}

std::string format_device_id(DeviceId &id) {
    char buf[14];
    sprintf(buf, "0x%04x:0x%04x", id.first, id.second);
    return std::string(buf);
}

int main() {
    printf("%s\n", format_device_id(get_user_devices()[0]).c_str());
}

/*

void cb(uvc_frame_t *frame, void *ptr) {
    pid_t x = syscall(__NR_gettid);
    printf("%d\n", x);
    printf("frame_cb\n");
}

static int count = 0;
int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) {
    static libusb_device_handle *handle = NULL;
    struct libusb_device_descriptor desc;
    int rc;
    pid_t x = syscall(__NR_gettid);
    printf("%d\n", x);
    (void)libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
        rc = libusb_open(dev, &handle);
        if (LIBUSB_SUCCESS != rc) {
            printf("Could not open USB device\n");
        }
    } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
        printf("LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT\n");
        if (handle) {
            libusb_close(handle);
            handle = NULL;
        }
    } else {
        printf("Unhandled event %d\n", event);
    }
    count++;
    return 0;
}
int main (void) {
    libusb_hotplug_callback_handle handle;
    int rc;
    libusb_context *ctx;
    libusb_init(&ctx);
    rc = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_NO_FLAGS, 0x058f, 0x0362,
                                          LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL,
                                          &handle);
    if (LIBUSB_SUCCESS != rc) {
        printf("Error creating a hotplug callback\n");
        libusb_exit(NULL);
        return EXIT_FAILURE;
    }
    uvc_context_t *uvc_ctx;
    uvc_device_t *dev;
    uvc_device_handle_t *devh;
    uvc_stream_ctrl_t ctrl;
    uvc_error_t res;
    res = uvc_init(&uvc_ctx, ctx);
    if (res < 0) {
        uvc_perror(res, "uvc_init");
        return res;
    }
    puts("UVC initialized");
    res = uvc_find_device(
            uvc_ctx, &dev,
            0x058f, 0x0362, NULL);
    if (res < 0) {
        uvc_perror(res, "uvc_find_device");
    } else {
        puts("Device found");

        res = uvc_open(dev, &devh);
        if (res < 0) {
            uvc_perror(res, "uvc_open");
        } else {
            puts("Device opened");

            uvc_print_diag(devh, stderr);
            res = uvc_get_stream_ctrl_format_size(
                    devh, &ctrl,
                    UVC_FRAME_FORMAT_YUYV,
                    640, 480, 30
            );
            uvc_print_stream_ctrl(&ctrl, stderr);
            if (res < 0) {
                uvc_perror(res, "get_mode");
            } else {
                res = uvc_start_streaming(devh, &ctrl, cb, NULL, 0);
                if (res < 0) {
                    uvc_perror(res, "start_streaming");
                } else {
                    puts("Streaming...");

                    uvc_set_ae_mode(devh, 1);
                }
            }
        }
    }
    while (count < 2) {
        pid_t x = syscall(__NR_gettid);
        printf("%d\n", x);
        libusb_handle_events_completed(NULL, NULL);
        usleep(10000);
    }        usleep(10000);
    libusb_hotplug_deregister_callback(NULL, handle);
    libusb_exit(NULL);
    return 0;
}

*/
