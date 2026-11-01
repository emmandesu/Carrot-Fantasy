#pragma once

#include "base/CCDirector.h"
#include "base/CCValue.h"
#include "platform/CCFileUtils.h"
#include "2d/CCDrawNode.h"
#include "2d/CCLabel.h"
#include "2d/CCMenuItem.h"
#include "2d/CCSprite.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include <algorithm>
#include <cmath>

// English text and replacement artwork metadata live in an editable XML plist.
// Header-only so existing Cocos projects need no extra source/linker registration.
namespace EnglishUI {
inline const cocos2d::ValueMap& catalog() {
    static const auto data = cocos2d::FileUtils::getInstance()->getValueMapFromFile(
        "localization/en.xml");
    return data;
}

inline std::string text(const std::string& key) {
    const auto strings = catalog().find("strings");
    if (strings != catalog().end()) {
        const auto& values = strings->second.asValueMap();
        const auto found = values.find(key);
        if (found != values.end()) return found->second.asString();
    }
    CCLOG("Missing English localization key: %s", key.c_str());
    return key;
}

inline const cocos2d::ValueMap* asset(const std::string& path) {
    const auto found = catalog().find("assets");
    if (found == catalog().end()) return nullptr;
    const auto& assets = found->second.asValueMap();
    const auto entry = assets.find(path);
    return entry == assets.end() ? nullptr : &entry->second.asValueMap();
}

inline cocos2d::Label* label(const std::string& key, float fontSize,
                             const cocos2d::Size& bounds) {
    auto result = cocos2d::Label::createWithTTF(text(key), "fonts/arial.ttf",
        fontSize, bounds, cocos2d::TextHAlignment::CENTER,
        cocos2d::TextVAlignment::CENTER);
    if (result) {
        result->setOverflow(cocos2d::Label::Overflow::SHRINK);
        result->setTextColor(cocos2d::Color4B(255, 252, 230, 255));
    }
    return result;
}

inline void addPlate(cocos2d::Node* parent, const cocos2d::ValueMap& entry,
                     const cocos2d::Size& size, bool selected = false) {
    using namespace cocos2d;
    // Opaque geometry replaces all Chinese lettering, including button shadows.
    // Geometry and TTF labels remain crisp at the existing scene scales.
    auto plate = DrawNode::create();
    const float radius = std::min(size.height * 0.20f, size.width * 0.08f);
    Vec2 points[24];
    for (int corner = 0; corner < 4; ++corner) {
        const Vec2 center(corner == 0 || corner == 3 ? size.width-radius : radius,
                          corner < 2 ? size.height-radius : radius);
        for (int i = 0; i < 6; ++i) {
            const float angle = (corner * 90.0f + i * 18.0f) * 0.01745329252f;
            points[corner*6+i] = center + Vec2(std::cos(angle), std::sin(angle))*radius;
        }
    }
    plate->drawPolygon(points, 24,
        selected ? Color4F(0.12f, 0.36f, 0.20f, 1) : Color4F(0.18f, 0.47f, 0.25f, 1),
        std::max(1.0f, size.height * 0.025f), Color4F(0.70f, 0.88f, 0.43f, 1));
    parent->addChild(plate);
    const float density = Director::getInstance()->getContentScaleFactor();
    const float font = entry.at("font").asFloat() / density;
    const auto body = entry.find("body");
    const bool hasBody = body != entry.end();
    const float padding = size.width * 0.05f;
    auto title = label(entry.at("key").asString(), font,
        Size(size.width-padding*2, size.height*(hasBody ? 0.28f : 0.88f)));
    if (title) {
        title->setPosition(size.width/2, size.height*(hasBody ? 0.78f : 0.5f));
        parent->addChild(title, 1);
    }
    if (hasBody) {
        auto description = label(body->second.asString(), font*0.68f,
            Size(size.width-padding*2, size.height*0.52f));
        if (description) {
            const auto bodyY = entry.find("body_y");
            description->setPosition(size.width/2, size.height *
                (bodyY == entry.end() ? 0.37f : bodyY->second.asFloat()));
            parent->addChild(description, 1);
        }
    }
}

inline cocos2d::Sprite* sprite(const std::string& path, bool selected = false) {
    using namespace cocos2d;
    const auto entry = asset(path);
    if (!entry) return Sprite::create(path);
    const float density = Director::getInstance()->getContentScaleFactor();
    const Size size(entry->at("width").asFloat()/density,
                    entry->at("height").asFloat()/density);
    const bool header = entry->at("kind").asString() == "header";
    auto result = header ? Sprite::create(path) : Sprite::create();
    if (!result) return nullptr;
    if (header) {
        result->setContentSize(size);
        // Theme cards have Chinese titles/subtitles in their upper quarter.
        // Retain their landscape artwork; cover the entire original text area.
        auto heading = Node::create();
        heading->setPosition(0, size.height*0.72f);
        result->addChild(heading);
        addPlate(heading, *entry, Size(size.width, size.height*0.28f), selected);
    } else {
        // Give the invisible host a nonzero rect; avoid a zero-size stretch.
        result->setTextureRect(Rect(0, 0, size.width, size.height));
        result->setCascadeOpacityEnabled(false);
        result->setOpacity(0);
        addPlate(result, *entry, size, selected);
    }
    return result;
}

inline cocos2d::MenuItemImage* menuItem(const std::string& normal,
    const std::string& selected,
    const cocos2d::ccMenuCallback& callback = cocos2d::ccMenuCallback()) {
    auto result = cocos2d::MenuItemImage::create();
    result->setNormalImage(sprite(normal));
    result->setSelectedImage(sprite(selected, true));
    result->setCallback(callback);
    return result;
}

// The visible English plate follows Button's state hooks, not a touch listener.
// Scene-specific listeners remain free to handle New Game and Load Game actions.
class TextButton : public cocos2d::ui::Button {
public:
    static TextButton* create(const std::string& path) {
        auto result = new (std::nothrow) TextButton();
        if (result && result->init()) {
            result->autorelease();
            result->_visual = sprite(path);
            result->ignoreContentAdaptWithSize(false);
            result->setContentSize(result->_visual->getContentSize());
            result->_visual->setPosition(result->getContentSize().width/2,
                                         result->getContentSize().height/2);
            result->addChild(result->_visual);
            return result;
        }
        CC_SAFE_DELETE(result);
        return nullptr;
    }
protected:
    void onPressStateChangedToNormal() override {
        Button::onPressStateChangedToNormal();
        if (_visual) _visual->setScale(1.0f);
    }
    void onPressStateChangedToPressed() override {
        Button::onPressStateChangedToPressed();
        if (_visual) _visual->setScale(0.95f);
    }
private:
    cocos2d::Sprite* _visual = nullptr;
};

inline cocos2d::ui::Button* button(const std::string& normal,
                                  const std::string& selected = "") {
    if (!asset(normal)) return cocos2d::ui::Button::create(normal, selected);
    return TextButton::create(normal);
}

inline cocos2d::ui::ImageView* imageView(const std::string& path) {
    auto result = cocos2d::ui::ImageView::create(path);
    if (result && asset(path)) {
        // Retain ImageView's original intrinsic size and PageView behavior.
        result->getVirtualRenderer()->setVisible(false);
        auto visual = sprite(path);
        visual->setPosition(result->getContentSize().width/2,
                            result->getContentSize().height/2);
        result->addChild(visual);
    }
    return result;
}
} // namespace EnglishUI
