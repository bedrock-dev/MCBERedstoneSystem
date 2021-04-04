//
// Created by xhy on 2021/2/23.
//

#include "CircuitTrackingInfo.h"
#include "BaseCircuitComponent.h"


void initEntry(CircuitTrackingInfo::Entry &entry, BaseCircuitComponent *component, const BlockPos &pos) {
    entry.mComponent = component;
    entry.mDirection = component->getDirection();
    entry.mPos = pos;
    entry.mTypeID = component->getBaseType();
}

CircuitTrackingInfo::CircuitTrackingInfo(BaseCircuitComponent *component, const BlockPos &pos, int dampening) {

    initEntry(this->mCurrent, component, pos);
    initEntry(this->mPower, component, pos);
    initEntry(this->mNearest, component, pos);
    initEntry(this->m2ndNearest, component, pos);
    this->mDampening = dampening;
    this->mDirectlyPowered = true;
    this->mData = 0;
}
