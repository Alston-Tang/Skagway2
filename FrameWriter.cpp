#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <experimental/filesystem>

//
// Created by tang on 7/5/19.
//

#include "FrameWriter.h"

namespace fs = std::experimental::filesystem;

Skagway2::FrameWriter::FrameWriter(std::string path, Skagway2::FrameQueue &down_queue) : path(std::move(path)), down_queue(down_queue) {
    if (!fs::exists(this->path)) {
        fs::create_directory(this->path);
    }
}

void Skagway2::FrameWriter::start_streaming() {
    thread = std::make_unique<std::thread>([this](){
        int count = 0;
        this->stop = false;
        while (!this->stop) {
            std::shared_ptr<Frame> cur_frame;
            bool res = down_queue.tryReadUntil(std::chrono::steady_clock::now() + std::chrono::milliseconds(30), cur_frame);
            if (!res) {
                continue;
            }
            std::string file_path = this->path + std::to_string(count) + ".jpg";
            int fd = open(file_path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            if (fd < 0) {
                perror("open");
            } else {
                ssize_t  bytes_write = write(fd, cur_frame->uvc_frame->data, cur_frame->uvc_frame->data_bytes);
                if (bytes_write != cur_frame->uvc_frame->data_bytes) {
                    LOG(ERROR) << "incomplete frame";
                }
            }
            count++;
        }
    });
}

void Skagway2::FrameWriter::end_streaming() {
    if (this->thread) {
        this->stop = true;
        this->thread->join();
    }
    this->thread = nullptr;
}
