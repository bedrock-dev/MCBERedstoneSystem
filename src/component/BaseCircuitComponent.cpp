//
// Created by xhy on 2021/2/23.
//

#include "BaseCircuitComponent.h"
#include "../CircuitSystem.h"

bool BaseCircuitComponent::calculateValue(CircuitSystem *system) {
    int newStrength = 0;
    for (auto &source: this->mSources.mComponents) {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;
        if (source.mComponent->isHalfPulse())
            currentStrength = 15 - source.mDampening;
        if (newStrength < currentStrength)
            newStrength = currentStrength;
    }

    bool hasChanged = this->mStrength != newStrength;
    this->mStrength = newStrength;
    return hasChanged || this->mIsFirstTime && this->mStrength == 0;
}

bool BaseCircuitComponent::trackPowerSource(const CircuitTrackingInfo &info,  //NOLINT
                                            int dampening, bool directlyPowered, int data) {


    return true;
}

bool BaseCircuitComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info,
                                           bool &directedPowered) {//NOLINT
    return false;
}

void
BaseCircuitComponent::trackPowerSourceDuplicates(const CircuitTrackingInfo &info, int dampening, bool directlyPowered) {


}

/**
 * 为当前原件删除一个信号源
 * @param pos
 * @param component
 */
bool BaseCircuitComponent::removeSource(const BlockPos &pos, BaseCircuitComponent *component) {
    return this->mSources.removeSource(pos, component);
}

/*
 * `component`是否是当前原件的一个信号源
 */
bool BaseCircuitComponent::hasSource(BaseCircuitComponent *component) { //NOLINT
    //todo
    if (this->mSources.size() <= 0)return false;
    for (auto &s: mSources.mComponents) {
        if (s.mComponent == component)return true;
        //信号源的信号源也算(前提是新仓元有子信号源)
        if (s.mComponent->hasChildrenSource() && s.mComponent->hasSource(component))return true;
    }
    return false;
}

/**
 * 是否有一个直接激活的信号源
 * @return
 */
bool BaseCircuitComponent::hasDirectPower() {
    for (auto &s: this->mSources) { //NOLINT
        if (s.mDirectlyPowered)return true;
    }
    return false;
}


