#ifndef SETTINGS_H
#define SETTINGS_H

#include "string"

struct settings {

    bool printAuthorInEntry = true;
    bool printTimeInEntry = true;
    bool printLineInEntry = true;

    bool askBeforeClose = true;
    bool useUnicodeEncoding = true;

    bool useDefaultFilePath = true;
    std::string defaultEntryPath = "";

};
typedef struct settings Settings;



#endif // SETTINGS_H
