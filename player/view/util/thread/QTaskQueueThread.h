#pragma once

#include <QCoreApplication>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <iostream>

namespace vleap {

// 定义任务类型，这里简单用一个函数指针表示
using Task = std::function<void()>;

// 任务队列类
class QTaskQueueThread : public QThread {
    Q_OBJECT
public:
    QTaskQueueThread(QObject *parent = nullptr);
    virtual ~QTaskQueueThread() override;

    // 添加任务到队列
    void addTask(const Task &task);

protected:
    virtual void run() override;

private:
    QQueue<Task> m_taskQueue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_abort;
    bool m_paused;
};

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);

//     TaskQueue taskQueue;
//     taskQueue.start();

//     // 添加一些任务
//     for (int i = 0; i < 5; ++i) {
//         int taskId = i;
//         taskQueue.addTask([taskId]() {
//             std::cout << "Executing task " << taskId << " in thread " << QThread::currentThreadId() << std::endl;
//             // 模拟任务执行耗时
//             QThread::sleep(1);
//         });
//     }

//     // 等待任务队列完成（这里简单等待一段时间）
//     QThread::sleep(6);

//     return a.exec();
// }
} // namespace vleap
