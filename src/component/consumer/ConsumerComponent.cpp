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
    this->mPromotedToProducer = false;//重置强充能状态
    if (this->mPropagatePower) {
        //当前原件能强充能
        if (id == CSPC) {
            //直连生产者
            auto *producer = dynamic_cast<ProducerComponent *>(info.mNearest.mComponent);
            //该信号源要能强充能充能方块or消费者且连接方向没问题
            //第一个条件红石块和阳光传感器不满足
            //第二个条件拉杆的附着方向不对也不满足
            directPowered = producer->doesAllowAttachments() && info.mCurrent.mDirection == producer->getDirection();
            //直接激活就是被强充能了
            this->mPromotedToProducer = directPowered;
        } else if (id == CSCA) {
            directPowered = info.mCurrent.mDirection == FACING::POS_Y;
            //TODO 电容器
            auto dirPower = info.mNearest.mComponent->getDirection();
//            if (dirPower != FACING::NOT_DEFINED) {
//                auto capacitor = dynamic_cast<CapacitorComponent *>(info.mNearest.mComponent);
//
//            }
        } else if (id == CSTR) {
            directPowered = true;
        } else {
            if (id == CSPB && !prevDp) {
                return false;
            }
        }
    } else if (id == CSPB && !prevDp) {
        //不能强充能
        return false;
    }
    return this->trackPowerSource(info, damping, directPowered, id == CSSC);
}

bool ConsumerComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    //todo
    auto id = info.mCurrent.mTypeID;

    if (id != CSCA) { //不是电容器
        if (id == CSTR && this->mPropagatePower) { //是红石线且能被强充能
            return this->mPromotedToProducer; //还要看是否被强充能了
        }
        return info.mDirectlyPowered; //没懂
    }
    //是电容器的情况
    //TODO: 没看懂
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
