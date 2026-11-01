#include "Carrot.h"
#include"music.h"
#define schedule_selector(_SELECTOR) static_cast<SEL_SCHEDULE>(&_SELECTOR)
USING_NS_CC;
extern Music a;
Carrot* Carrot::create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    Carrot* ret = new (std::nothrow) Carrot();
    if (ret && ret->init(initialHP, carrotPos, hpPos)) {
        ret->autorelease();  // 交给内存管理
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}

bool Carrot::init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos) {
    if (!Node::init()) { // 确保父类的init()调用成功
        return false;
    }

    // 初始化血量
    hp = initialHP;

    // 载入精灵帧缓存
    auto frameCache = SpriteFrameCache::getInstance();
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist", "CarrotGuardRes/Carrots.png");
    frameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist", "CarrotGuardRes/Health.png");

    // 初始化萝卜图片
    carrotSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", (hp+1)/2));
    carrotSprite->setScale(1.5);
    carrotSprite->setPosition(carrotPos);
    this->addChild(carrotSprite, 2); // 将萝卜图片添加为Carrot的子节点

    // 初始化血条图片
    hpSprite = Sprite::createWithSpriteFrameName(StringUtils::format("Health_%d.png", std::max(1, hp)));
    hpSprite->setScale(1.5);
    hpSprite->setPosition(hpPos);
    this->addChild(hpSprite, 2); // 将血条图片添加为Carrot的子节点

    return true;
}

void Carrot::changeHP(int change) {
    hp = std::max(0, std::min(10, hp + change));
    updateHPDisplay();
}

void Carrot::updateHPDisplay() {
    carrotSprite->setSpriteFrame(StringUtils::format("Carrot_%d.png", std::max(1, (hp + 1) / 2)));
    hpSprite->setSpriteFrame(StringUtils::format("Health_%d.png", std::max(1, hp)));
}





void Carrot::getDamage(int damage) {
    if(hp<=0)
        return;
    a.carrotSound();
    changeHP(0-damage);
    CCLOG("getDamage...");
}
void Carrot::getRecover()
{
    if (hp== 10)
        return;
    changeHP(1);
    CCLOG("getRecover...");
}
void Carrot::gameOver() {
    CCLOG("game over");
}

int Carrot::getHP() const {
    return hp;
}
