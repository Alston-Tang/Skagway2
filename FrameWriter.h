//
// Created by tang on 7/5/19.
//

#ifndef SKAGWAY2_FRAMEWRITER_H
#define SKAGWAY2_FRAMEWRITER_H

#include <thread>

#include "Camera.h"

namespace Skagway2 {

class FrameWriter {
public:
    FrameWriter(std::string path, FrameQueue &down_queue);
    void start_streaming();
    void end_streaming();

private:
    std::atomic<bool> stop = false;
    std::unique_ptr<std::thread> thread = nullptr;
    std::string path;
    FrameQueue &down_queue;
};

}

#endif //SKAGWAY2_FRAMEWRITER_H
