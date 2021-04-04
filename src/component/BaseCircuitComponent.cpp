//
// Created by xhy on 2021/2/23.
//

#include "BaseCircuitComponent.h"
#include "../CircuitSystem.h"

bool BaseCircuitComponent::calculateValue(CircuitSystem *system) {
    int newStrength = 0;
    for (auto &source:this->mSources.mComponents) {
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

void BaseCircuitComponent::removeSource(const BlockPos &pos, BaseCircuitComponent *component) {
    //todo

}

bool BaseCircuitComponent::hasSource(BaseCircuitComponent *component) { //NOLINT
    //todo
    return true;
}

bool BaseCircuitComponent::hasDirectPower() {
    //todo
    return false;
}


