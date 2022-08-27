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

/*
 * 这个函数是被信号源搜索到并准备添加新的信号源的时候执行,面对新的信号源已经在当前的更新列表时的操作
 */
bool BaseCircuitComponent::trackPowerSource(const CircuitTrackingInfo &info,  //NOLINT
                                            int dampening, bool directlyPowered, int data) {
    bool dampChanged = false;
    for (auto &s: this->mSources) {
        if (s.mPos == info.mPower.mPos) {
            /* old new
             *  1   0
             *  1   1
             *  0   0
             */
            //如果新搜索到的信号源在此之前已经是信号源了
            if (s.mDirectlyPowered || !directlyPowered) {
                //B. 其他情况
                if (s.mDampening <= info.mDampening) {
                    //如果旧的电阻更小，保持不变
                    dampChanged = false;
                } else {
                    //新搜搜索到的电阻更小就更新
                    s.mDampening = std::max(info.mDampening - 1, 0); //
                    dampChanged = true;
                }
            } else {
                //A. 从非直接激活变成直接激活的情况
                //信号源强度不足以支撑这个原件(提供的能量为0)，就青强制使用新旧电阻中更更小的一个
                if (info.mPower.mComponent && info.mPower.mComponent->getStrength() - info.mDampening <= 0) {
                    s.mDampening = std::max(std::min(info.mDampening - 1, s.mDampening), 0);
                    dampChanged = true;
                } else {
                    //可以支撑就用新的
                    s.mDampening = std::max(info.mDampening - 1, 0);
                    dampChanged = true;
                }
            }

            return dampChanged;
        }
    }

    //新增加的信号源不在原有的信号源表内
    auto newDamp = std::max(0, dampening - 1);
    CircuitComponentList::Item it(info.mPower.mComponent, newDamp, info.mPower.mPos);
    this->mSources.push_back(it);
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


