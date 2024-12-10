#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

enum class WeatherType {
    CLEAR,
    // OVERCAST,
    SNOW,
    RAIN
};

enum class MountainType {
    SNOW_MOUNTAIN,
    ROCK_MOUNTAIN,
    GRASS_MOUNTAIN
};

struct Settings {
    float fov;
    float time;
    WeatherType weather;
    MountainType mountain;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
