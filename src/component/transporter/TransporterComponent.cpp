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
    if (info.mCurrent.mDirection == FACING::POS_Y) { return false; }//从正上方来的原件绝对禁止连接

    if (info.mCurrent.mDirection == FACING::NEG_Y ||
        info.mCurrent.mComponent->consumePowerAnyDirection()) { return true; } //如果六个方向都接受连接，直接返回true

    auto type = info.mCurrent.mTypeID;
    if (type == CSTR) {

        auto dy = info.mCurrent.mPos.y - info.mNearest.mPos.y;
        /*
          * [current]
          *           [ nearest]
          */
        if (dy >= 0) {
            //从下往下传输上方的方块组织信号传输
            if (dy > 0  /**/) {
                return false;
            } else {



            }

        }


    }


    return false;
}


//找能提供最大能量的信号源
void TransporterComponent::cacheValues(CircuitSystem &, const BlockPos &pos) {
    int currentStrength = 0, newStrength = 0;
    for (auto &item: this->mSources.mComponents) {
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
