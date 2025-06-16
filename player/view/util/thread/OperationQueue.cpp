
#include "OperationQueue.h"

namespace vleap {
namespace async {

#pragma mark Operation
void Operation::setTaskBlock(std::function<void(void)> block) {
    taskBlock_ = block;
}
void Operation::setCompletionBlock(std::function<void(void)> callback) {
    completion_ = callback;
};

void Operation::addDependency(Operation *op) {
    dependencies.push_back(op);
}

void Operation::removeDependency(Operation *op) {
    for (auto it = dependencies.begin(); it != dependencies.end(); ++it) {
        if (*it == op) {
            dependencies.erase(it);
        }
    }
    std::cerr << "Operation not found in dependencies." << std::endl;
}

// - (BOOL)isReady {
//     if (!self.isFinished) {
//         for (NSOperation *dependency in self.dependencies) {
//             if (!dependency.isFinished) {
//                 return NO;
//             }
//         }
//         return YES;
//     }
//     return NO;
// }

#pragma mark OperationQueue
// 工作线程函数, 类的静态成员函数, 可以访问类的私有成员变量
void OperationQueue::LoopEv(void *opaque) {
    OperationQueue *operater = static_cast<OperationQueue *>(opaque);
    operater->taskWorker(operater);
}

void OperationQueue::taskWorker(OperationQueue *worker) {
    while (true) {
        std::unique_lock<std::mutex> lock(worker->queueMutex);
        worker->condVar.wait(lock, [&] {
            return !worker->taskQueue.empty() || worker->stop;
        });

        if (worker->stop && worker->taskQueue.empty()) {
            break; // 如果停止信号为真且任务队列为空，则退出循环
        }

        // 取出并执行任务
        Operation task = std::move(worker->taskQueue.front());
        worker->taskQueue.pop();
        lock.unlock(); // 在执行任务前解锁，以避免持有锁的时间过长

        task.taskBlock_(); // 执行任务
        if (task.completion_) {
            task.completion_();
        }
    }
}

bool OperationQueue::startWorker() {
    // 启动指定数量的工作线程
    for (size_t i = 0; i < threadCount_; ++i) {
        // std::thread t(OperationQueue::LoopEv, this);
        threadList_.emplace_back(OperationQueue::LoopEv, this);
    }
    return true;
}

OperationQueue::OperationQueue() {
}

void OperationQueue::setMaxConcurrentOperationCount(size_t number) {
    threadCount_ = number;
}

void OperationQueue::addTaskWithBlock(Operation task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    taskQueue.push(std::move(task));
    // printf("taskQueue push: %zu\n", taskQueue.size());
    lock.unlock();
    condVar.notify_one(); // 通知一个等待的线程有新任务到来
}

void OperationQueue::addBarrierBlock(Operation task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    waitForDone();

    taskQueue.push(std::move(task));
    condVar.notify_one(); // 通知一个等待的线程有新任务到来

    waitForDone();
    lock.unlock();
}

bool OperationQueue::isSuspended() {
    return false;
}

int OperationQueue::operationCount() {
    size_t size = 0;
    std::unique_lock<std::mutex> lock(queueMutex);
    size = taskQueue.size();
    lock.unlock();
    return (int)size;
}

void OperationQueue::waitForDone() {
    // 等待所有任务完成
    // 使用 std::async 来创建一个异步任务作为 stopper 线程
    std::future<void> stopperFuture_ = std::async(std::launch::deferred, [&]() {
        // 等待一段时间或满足某个条件后，设置停止信号并通知所有线程
        std::this_thread::sleep_for(std::chrono::seconds(10)); // 示例：等待10秒
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condVar.notify_all(); // 通知所有等待的线程

        // 等待所有工作线程结束
        for (auto &t : threadList_) {
            if (t.joinable()) {
                t.join();
            }
        }
    });

    stopperFuture_.wait();
}

void OperationQueue::cancelAllTask() {
    std::unique_lock<std::mutex> lock(queueMutex);
    printf("onSliderMoved: %zu\n", taskQueue.size());
    while (taskQueue.size() != 0) {
        taskQueue.pop();
    }
    lock.unlock();
}

OperationQueue::~OperationQueue() {
}
}
} // namespace avmedia::async
