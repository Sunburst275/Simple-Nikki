#ifndef PROFILE_H
#define PROFILE_H

#include "string"


struct profile {
    static constexpr int MAX_NAMELENGTH_PROFILE = 100;

    std::string name;
    bool isDefault;
};
typedef struct profile Profile;


#endif // PROFILE_H
