//
// Created by xhy on 2021/3/22.
//

#include "ConsumerComponent.h"

bool
ConsumerComponent::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {
//    this->mPromotedToProducer = false;
//    auto typeID = info.mNearest.mTypeID;
//    auto promotedToProducer = false;
//    if (this->mPropagatePower) {
//        switch (typeID) {
//            case CSPC:
//                //todo
//                break;
//            case CSCA:
//                //todo
//                break;
//            case CSTR:
//                //todo
//            default:
//                if (typeID == CSPB && !directPowered)
//                    return false;
//                break;
//        }
//    } else if (typeID == CSPB && !directPowered) {
//        return false;
//    }
//
//    if (this->trackPowerSource(info, damping, directPowered, typeID == CSCC)) {
//        return directPowered;
//    } else {
//        return false;
//    }
    //todo
    return false;
}

bool ConsumerComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    //todo
    return false;
}


bool ConsumerComponent::evaluate(CircuitSystem &, const BlockPos &) {
    int newStrength = 0;
    for (auto &source: this->mSources.mComponents) {
        int currentStrength = source.mComponent->getStrength() - source.mDampening;
        if (source.mComponent->isHalfPulse() && !this->mAcceptHalfPulse) {
            currentStrength = 15 - source.mDampening;
        }
        //半脉冲特判定
        if (currentStrength < 0)currentStrength = 0;
        if (newStrength < currentStrength) {
            this->mSecondaryPowered = source.mData == 1;
            newStrength = currentStrength;
        }
    }
    auto hasChanged = this->mStrength != newStrength;
    this->mStrength = newStrength;
    return hasChanged;
}
