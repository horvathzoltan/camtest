#ifndef RASPICAMTYPES_H
#define RASPICAMTYPES_H

#include <QString>


/**Auto white balance types
     */
enum RASPICAM_AWB {
    RASPICAM_AWB_OFF,
    RASPICAM_AWB_AUTO,
    RASPICAM_AWB_SUNLIGHT,
    RASPICAM_AWB_CLOUDY,
    RASPICAM_AWB_SHADE,
    RASPICAM_AWB_TUNGSTEN,
    RASPICAM_AWB_FLUORESCENT,
    RASPICAM_AWB_INCANDESCENT,
    RASPICAM_AWB_FLASH,
    RASPICAM_AWB_HORIZON
}  ;

static QString WbToString(int i){
    switch(static_cast<RASPICAM_AWB>(i)){
    case RASPICAM_AWB_OFF: return QStringLiteral("OFF");
    case RASPICAM_AWB_AUTO: return QStringLiteral("AUTO");
    case RASPICAM_AWB_SUNLIGHT: return QStringLiteral("SUNLIGHT");
    case RASPICAM_AWB_CLOUDY: return QStringLiteral("CLOUDY");
    case RASPICAM_AWB_SHADE: return QStringLiteral("SHADE");
    case RASPICAM_AWB_TUNGSTEN: return QStringLiteral("TUNGSTEN");
    case RASPICAM_AWB_FLUORESCENT: return QStringLiteral("FLUORESCENT");
    case RASPICAM_AWB_INCANDESCENT: return QStringLiteral("INCANDESCENT");
    case RASPICAM_AWB_FLASH: return QStringLiteral("FLASH");
    case RASPICAM_AWB_HORIZON: return QStringLiteral("HORIZON");
    default: return QStringLiteral("unknown");
    }
}

#endif // RASPICAMTYPES_H
