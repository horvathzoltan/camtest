#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>
#include "sqlhelper.h"

class Settings
{
public:
    SQLHelper::SQLSettings _sql_settings;
    //int _project = 99;

    Settings(const SQLHelper::SQLSettings&a);
};

#endif // SETTINGS_H
