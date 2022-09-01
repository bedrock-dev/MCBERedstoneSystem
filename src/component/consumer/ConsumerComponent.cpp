//
// Created by xhy on 2021/3/22.
//

#include "ConsumerComponent.h"
#include "component/producer/ProducerComponent.h"

bool
ConsumerComponent::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {

    auto id = info.mNearest.mTypeID;
    auto prevDp = directPowered;
    directPowered = false; //先把dp强制设成false
    this->mPromotedToProducer = false;//这个？？？属性也重置
    if (this->mPropagatePower) {
        //当前原件能强充能
        if (id == CSPC) { //直连生产者
            auto *producer = dynamic_cast<ProducerComponent *>(info.mNearest.mComponent);
            directPowered = producer->doesAllowAttachments() && info.mCurrent.mDirection == producer->getDirection();
            this->mPromotedToProducer = directPowered;
        } else if (id == CSCA) {
            directPowered = info.mCurrent.mDirection == FACING::POS_Y;

        }
    } else {
        //不能强充能




    }


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
