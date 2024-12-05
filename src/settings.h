#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    float azimuth = 0.0f;
    float elevation = 0.0f;
    float fov = 45.0f;
    float time = 0.0f;
    float T = 2.0f;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
