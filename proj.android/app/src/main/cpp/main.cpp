#include "AppDelegate.h"
#include "BaseLevelScene.h"
#include "menuScene.h"
#include <jni.h>

void cocos_android_app_init(JNIEnv*) {
    static AppDelegate app;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_emmandesu_carrotfantasy_MainActivity_nativeBack(JNIEnv*, jclass) {
    auto director = cocos2d::Director::getInstance();
    auto scene = director->getRunningScene();
    if (auto level = dynamic_cast<BaseLevelScene*>(scene)) {
        level->menu_all(nullptr);
        return JNI_TRUE;
    }
    if (dynamic_cast<MenuScene*>(scene)) return JNI_FALSE;
    director->popScene();
    return JNI_TRUE;
}
