#include "timethread.h"

using std::chrono::system_clock;

timeThread::timeThread()
    :
  rawTime(system_clock::to_time_t(system_clock::now())),
  time(std::localtime(&rawTime))
{

}

void timeThread::run()
{
    while(isLooping){
    //qDebug() << timeLabel->text();

    rawTime = system_clock::to_time_t(system_clock::now());
    time = std::localtime(&rawTime);

    //qDebug() << "[timeThread]\t" << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec;
    emit updateUI(time);

    QThread::msleep(1000);
    }
}

void timeThread::forceTimeUpdate()
{
    rawTime = system_clock::to_time_t(system_clock::now());
    time = std::localtime(&rawTime);
    emit updateUI(time);
}

void timeThread::setLooping(bool isLooping)
{
    this->isLooping = isLooping;
}

bool timeThread::getLooping()
{
    return isLooping;
}


