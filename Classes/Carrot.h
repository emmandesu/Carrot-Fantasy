#pragma once

//#ifdef CARROT_H
//#define CARROT_H
#ifndef CARROT_H
#define CARROT_H

#include "cocos2d.h"

class Carrot : public cocos2d::Node  // 继承Node
{
public:
    // 使用create静态方法进行对象创建
    static Carrot* create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);
    virtual bool init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);
    void changeHP(int change); // 改变血量
    void getDamage(int damage);          // 受伤
    void getRecover();         //治疗
    int getHP() const;         // 获取当前的萝卜血量
    int hp;                         // 血量
private:
    cocos2d::Sprite* carrotSprite;  // 萝卜的图片
    cocos2d::Sprite* hpSprite;      // 血条的图片
    void updateHPDisplay();         // 更新血条和萝卜的图片显示
    void gameOver();                // 游戏结束
};

#endif

//#endif CARROT_H
