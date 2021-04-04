//
// Created by xhy on 2021/3/16.
//

#include "ProducerComponent.h"

void ProducerComponent::allowAttachments(bool attached) {
    this->mAttachedAllowed = attached;
}

bool
ProducerComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool bDirectlyPowered) {
    if (this->mAttachedAllowed) return true;
    return this->getBaseType() != CSPB;
}


ComponentTypeID ProducerComponent::getBaseType() {
    return CSPC;
}

ComponentTypeID ProducerComponent::getInstanceType() {
    return CSPC;
}


void ProducerComponent::setStrength(int strength) {
    this->mNextStrength = strength;
    if (this->mIsFirstTime)
        this->mStrength = mNextStrength;
}


bool ProducerComponent::evaluate(CircuitSystem &, const BlockPos &) {
    this->mStrength = this->mNextStrength;
    return false;
}

ComponentTypeID CapacitorComponent::getBaseType() {
    return CSCA;
}

ComponentTypeID CapacitorComponent::getInstanceType() {
    return CSCA;
}
