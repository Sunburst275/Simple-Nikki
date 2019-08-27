#ifndef TIMETHREAD_H
#define TIMETHREAD_H

#include "QtCore"
#include "QLabel"
#include "QDebug"

// For time
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

class timeThread : public QThread
{
    Q_OBJECT

public:
    timeThread();

    void run();

    void forceTimeUpdate();
    void setLooping(bool isLooping);
    bool getLooping();

private:
    std::time_t rawTime;
    std::tm *time = nullptr;

    bool isLooping = false;

signals:
    void updateUI(const std::tm* pTime);

};

#endif // TIMETHREAD_H
