/**************************************************
** Author      : 徐建文
** CreateTime  : 2019-06-27 09:02:00
** ModifyTime  : 2019-06-27 18:02:17
** Email       : Vincent_xjw@163.com
** Description : RunGuard
***************************************************/

#ifndef RunGuard_H
#define RunGuard_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class RunGuard
{
public:
    RunGuard( const QString& key );
    ~RunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY( RunGuard )
};

#endif
