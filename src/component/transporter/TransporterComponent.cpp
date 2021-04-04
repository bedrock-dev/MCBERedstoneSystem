//
// Created by xhy on 2021/3/22.
//

#include "TransporterComponent.h"

bool
TransporterComponent::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {
    //todo
    return false;
}

bool TransporterComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    //todo
    return false;
}


//找能提供最大能量的信号源
void TransporterComponent::cacheValues(CircuitSystem &, const BlockPos &pos) {
    int currentStrength = 0, newStrength = 0;
    for (auto &item:this->mSources.mComponents) {
        currentStrength = item.mComponent->getStrength() - item.mDampening;
        if (currentStrength < 0) {
            currentStrength = 0;
        }
        if (newStrength < currentStrength) {
            newStrength = currentStrength;
        }
    }
    this->mNextStrength = newStrength;
}

//更新信号
bool TransporterComponent::evaluate(CircuitSystem &, const BlockPos &) {
    bool changed = this->mStrength != this->mNextStrength;
    this->mStrength = this->mNextStrength;
    return changed;
}
