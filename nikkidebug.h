#ifndef NIKKIDEBUG_H
#define NIKKIDEBUG_H

#include "sstream"

class NikkiDebug : public std::stringstream
{
public:
    NikkiDebug();
};

//  Standard message:
/*
    nikkiDebugStream << "[" << "callingWindow" << "]"
                     << "[" << "callingFunction()" << "]" << ":\n"
                     << "messageBlub: "
                     << message2 ;
*/

#endif // NIKKIDEBUG_H
