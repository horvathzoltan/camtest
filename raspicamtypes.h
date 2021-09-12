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
};

enum RASPICAM_EXPOSURE {
    RASPICAM_EXPOSURE_OFF,
    RASPICAM_EXPOSURE_AUTO,
    RASPICAM_EXPOSURE_NIGHT,
    RASPICAM_EXPOSURE_NIGHTPREVIEW,
    RASPICAM_EXPOSURE_BACKLIGHT,
    RASPICAM_EXPOSURE_SPOTLIGHT,
    RASPICAM_EXPOSURE_SPORTS,
    RASPICAM_EXPOSURE_SNOW,
    RASPICAM_EXPOSURE_BEACH,
    RASPICAM_EXPOSURE_VERYLONG,
    RASPICAM_EXPOSURE_FIXEDFPS,
    RASPICAM_EXPOSURE_ANTISHAKE,
    RASPICAM_EXPOSURE_FIREWORKS
};


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
    };
}

static QString ExtToString(int i){
    switch(static_cast<RASPICAM_EXPOSURE>(i)){
    case RASPICAM_EXPOSURE_OFF: return QStringLiteral("OFF");
    case RASPICAM_EXPOSURE_AUTO: return QStringLiteral("AUTO");
    case RASPICAM_EXPOSURE_NIGHT: return QStringLiteral("NIGHT");
    case RASPICAM_EXPOSURE_NIGHTPREVIEW: return QStringLiteral("NIGHTPREVIEW");
    case RASPICAM_EXPOSURE_BACKLIGHT: return QStringLiteral("BACKLIGHT");
    case RASPICAM_EXPOSURE_SPOTLIGHT: return QStringLiteral("SPOTLIGHT");
    case RASPICAM_EXPOSURE_SPORTS: return QStringLiteral("SPORTS");
    case RASPICAM_EXPOSURE_SNOW: return QStringLiteral("SNOW");
    case RASPICAM_EXPOSURE_BEACH: return QStringLiteral("BEACH");
    case RASPICAM_EXPOSURE_VERYLONG: return QStringLiteral("VERYLONG");
    case RASPICAM_EXPOSURE_FIXEDFPS: return QStringLiteral("FIXEDFPS");
    case RASPICAM_EXPOSURE_ANTISHAKE: return QStringLiteral("ANTISHAKE");
    case RASPICAM_EXPOSURE_FIREWORKS: return QStringLiteral("FIREWORKS");
    default: return QStringLiteral("unknown");
    };
}



#endif // RASPICAMTYPES_H
