//
// Created by xhy on 2021/3/31.
//

#include "PoweredBlockComponent.h"

bool PoweredBlockComponent::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping,
                                      bool &directPowered) {
    //todo
    return false;
}
bool PoweredBlockComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    auto typeID = info.mCurrent.mTypeID;
    if (typeID == CSTR) {
        return this->mPromotedToProducer;
    } else {
        return typeID != CSPB;
    }
}

//充能方块的红石信号是获取的时候实时计算的,没有缓存
int PoweredBlockComponent::getStrength() const {
    int newStrength = 0;
    for (auto &source:this->mSources.mComponents) {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;
        if (currentStrength < 0)currentStrength = 0;
        if (newStrength < currentStrength)
            newStrength = currentStrength;
    }
    return newStrength;
}
