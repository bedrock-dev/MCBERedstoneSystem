//
// Created by xhy on 2021/2/23.
//

#include "BaseCircuitComponent.h"
#include "../CircuitSystem.h"

/**
 * 计算当前原件信号强度的基本算法
 * @param system
 * @return
 */
bool BaseCircuitComponent::calculateValue(CircuitSystem *system) {
    int newStrength = 0;
    //遍历所有红石原件
    //计算信号源强度 - 电阻 并取最大值
    for (auto &source: this->mSources.mComponents) {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;
        //如果信号源是半脉冲信号，就强制用15作为该信号源的强度
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
 * 这个函数是被信号源搜索到并准备添加新的信号源的时候执行对当前信号源的相关信息进行修正
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

            /*
             * 这里处理的目的是
             * 如果一个信号源通过多条线连到同一个消费者，这里有个问题就是
             * 由于搜索算法使用的BFS,那么**电阻更大的电路一定最后搜索到**，
             * 如果不加以处理就只能用电阻更大的值，这里显然不合理，因此才
             * 有这一段特判：
             * 每搜索完需要更新两个值：
             *  1. 电阻
             *  2. 是否是直接激活
             * 在不同的情况下对这俩值的设定有不同的策略
             */
            //如果新搜索到的信号源在此之前已经是信号源了
            if (s.mDirectlyPowered || !directlyPowered) {
                //B. 其他情况
                // a. 激活情况不变 直接还是直接 非直接还是得直接
                // 使用电阻更小的情况，但是是否直接激活仍然用新的
                // 从直接激活 -> 非直接激活（这种情况好像不存在的样子）
                //
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
                //更新及活泼方式
                s.mDirectlyPowered = false;
                //旧的信号源强度不足以支撑这个原件(提供的能量为0)，才使用 就青强制使用新旧电阻中更更小的一个
                if (info.mPower.mComponent && info.mPower.mComponent->getStrength() - info.mDampening <= 0) {
                    s.mDampening = std::max(std::min(info.mDampening - 1, s.mDampening), 0);
                    dampChanged = true;
                } else {
                    //如果新的可以支撑就用新的
                    s.mDampening = std::max(info.mDampening - 1, 0);
                    dampChanged = true;
                }
            }

            return dampChanged;
        }
    }

    //新增加的信号源不在原有的信号源表内，直接把相关数据加入即可
    auto newDamp = std::max(0, dampening - 1);
    CircuitComponentList::Item it(info.mPower.mComponent, newDamp, info.mPower.mPos);
    it.mDirectlyPowered = directlyPowered;
    it.mData = data;
    this->mSources.push_back(it);
    return true;
}

/*
 * 允许搜索的过程中元件A能搜索到原件B
 */
bool BaseCircuitComponent::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info,
                                           bool &directedPowered) {//NOLINT
    return true;
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


