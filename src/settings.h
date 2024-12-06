#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

enum class WeatherType {
    CLEAR,
    // OVERCAST,
    SNOW,
    RAIN
};

struct Settings {
    float fov;
    float time;
    WeatherType weather;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
