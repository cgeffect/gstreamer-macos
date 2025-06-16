#include "QTaskQueueThread.h"

namespace vleap {
// TaskQueueThread::TaskQueueThread() {
//     // 初始化互斥锁和条件变量
//     pthread_mutex_init(&mutex, nullptr);
//     pthread_cond_init(&cond, nullptr);
// }

QTaskQueueThread::QTaskQueueThread(QObject *parent) :
    QThread(parent), m_abort(false), m_paused(false) {
}
QTaskQueueThread::~QTaskQueueThread() {
    {
        QMutexLocker locker(&m_mutex);
        m_abort = true;
        m_cond.wakeOne();
    }
    wait();
}

// 添加任务到队列
void QTaskQueueThread::addTask(const Task &task) {
    QMutexLocker locker(&m_mutex);
    m_taskQueue.enqueue(task);
    m_cond.wakeOne();
}

void QTaskQueueThread::run() {
    forever {
        QMutexLocker locker(&m_mutex);
        while (m_taskQueue.isEmpty() && !m_abort) {
            m_cond.wait(&m_mutex);
        }
        if (m_abort) {
            return;
        }
        if (m_paused) {
            m_cond.wait(&m_mutex);
        }

        Task task = m_taskQueue.dequeue();
        locker.unlock();

        // 执行任务
        task();
    }
}
} // namespace vleap