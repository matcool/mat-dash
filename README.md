# MAT dash

mat dash

templates gone insane

Thx to pie for some help on the templates

this project is very basic, as its only temporary while lilac is in the works

## Usage

```cpp
#include <matdash.hpp>
#include <matdash/minhook.hpp>
#include <gd.h>

using namespace cocos2d;

class MenuLayerMod : public gd::MenuLayer {
public:
    // here the name cant be `init` as that'd make it a virtual
    // which doesnt work with the current code
    bool init_() {
        if (!orig<&MenuLayerMod::init_>(this)) return false;

        auto label = CCLabelBMFont::create("Hello world!", "bigFont.fnt");
        label->setPosition(ccp(200, 200));
        addChild(label);

        return true;
    }
};

void MenuLayer_onNewgrounds(gd::MenuLayer* self, CCObject* sender) {
    std::cout << "cool!" << std::endl;
    orig<&MenuLayer_onNewgrounds>(self, sender);
}

bool GJDropDownLayer_init(gd::GJDropDownLayer* self, const char* title, float height) {
    return orig<&GJDropDownLayer_init>(self, "my own title", height * 0.5f);
}

void PlayLayer_update(gd::PlayLayer* self, float dt) {
    orig<&PlayLayer_update, Thiscall>(self, dt * 0.5f);
}

void mod_main() {
    add_hook<&MenuLayerMod::init_>(gd::base + 0x1907b0);
    add_hook<&MenuLayer_onNewgrounds>(gd::base + 0x191e90);
    add_hook<&GJDropDownLayer_init>(gd::base + 0x113530);
    // Note the `Thiscall`, this is because PlayLayer::update is not
    // optimized, which is what MAT dash defaults to
    add_hook<&PlayLayer_update, Thiscall>(gd::base + 0x2029c0);
}

#include <matdash/boilerplate.hpp>
```

# Installation

If youre using cmake then you can
```cmake
include(mat-dash/include.cmake)
```