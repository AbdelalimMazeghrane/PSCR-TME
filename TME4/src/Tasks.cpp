#include "Tasks.h"
#include "util/ImageUtils.h"
#include "util/thread_timer.h"
#include <thread>
#include <sstream>

namespace pr {

void treatImage(FileQueue& fileQueue, const std::filesystem::path& outputFolder) {
    // measure CPU time in this thread
    pr::thread_timer timer;
    
    while (true) {
        std::filesystem::path file = fileQueue.pop();
        if (file == pr::FILE_POISON) break; // poison pill
        QImage original = pr::loadImage(file);
        if (!original.isNull()) {
            QImage resized = pr::resizeImage(original);
            std::filesystem::path outputFile = outputFolder / file.filename();
            pr::saveImage(resized, outputFile);
        }
    }

    // trace
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (treatImage): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}



void reader(FileQueue& fileQueue, ImageTaskQueue& imageQueue) {
    pr::thread_timer timer;
    while (true) {
        std::filesystem::path file = fileQueue.pop();
        if (file == pr::FILE_POISON) {
            imageQueue.push(pr::TASK_POISON);
            break;
        }

        QImage img = pr::loadImage(file);

        if (!img.isNull()) {
            
            pr::TaskData* task=new pr::TaskData{img, file};
            imageQueue.push(task);
        }
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (reader): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}

void resizer(ImageTaskQueue& imageQueue, ImageTaskQueue& resizedQueue) {
    pr::thread_timer timer;
    while (true) {
        pr::TaskData* td=imageQueue.pop();
        if (td == pr::TASK_POISON) {
            resizedQueue.push(pr::TASK_POISON);
            delete td;
            break;
        }

        if(!td->image.isNull()){
            QImage rimg=pr::resizeImage(td->image);
            TaskData* task=new pr::TaskData{rimg, td->filename};
            resizedQueue.push(task);
        }
        delete td;
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (resizer): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}

void saver(ImageTaskQueue& resizedQueue, const std::filesystem::path& outputFolder) {
    pr::thread_timer timer;
    while (true) {
        pr::TaskData* td=resizedQueue.pop();
        if(td != pr::TASK_POISON){
            delete td;
            break;
        }
        pr::saveImage(td->image,outputFolder / td->filename);
        delete td;
    }
    std::stringstream ss;
    ss << "Thread " << std::this_thread::get_id() << " (saver): " << timer << " ms CPU" << std::endl;
    std::cout << ss.str();
}



}  //namespace pr