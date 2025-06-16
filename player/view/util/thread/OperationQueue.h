#pragma once

#include <iostream>
#include <chrono>


#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

#include <future>

namespace vleap {
namespace async {
// 定义一个任务类型，这里使用std::function来存储可调用对象
using TaskBlock = std::function<void(void)>;
using CompletionBlock = std::function<void(void)>;
class Operation {
public:
    void setTaskBlock(std::function<void(void)> block);
    void setCompletionBlock(std::function<void(void)> callback);

    void addDependency(Operation *op);
    void removeDependency(Operation *op);

public:
    TaskBlock taskBlock_ = nullptr;
    CompletionBlock completion_ = nullptr;

private:
    std::vector<Operation *> dependencies;
};

class OperationQueue {
public:
    OperationQueue();
    ~OperationQueue();
    OperationQueue(const OperationQueue&) = delete;
    OperationQueue& operator=(const OperationQueue&) = delete;

    void setMaxConcurrentOperationCount(size_t number);
    bool startWorker();

    void addTaskWithBlock(Operation op);
    void addBarrierBlock(Operation op);

    void cancelAllTask();
    void waitForDone();

    int operationCount();
    bool isSuspended();

private:
    // 任务队列和同步机制
    std::queue<Operation> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condVar;
    // 用于停止工作线程的信号
    std::atomic<bool> stop = false;

private:
    static void LoopEv(void *opaque);

    void taskWorker(OperationQueue *worker);
    std::vector<std::thread> threadList_;
    size_t threadCount_ = 0;
};

}
} // namespace avmedia::async
