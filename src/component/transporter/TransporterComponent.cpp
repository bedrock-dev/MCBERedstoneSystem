//
// Created by xhy on 2021/3/22.
//

#include "TransporterComponent.h"
#include "component/powered_block/PoweredBlockComponent.h"
#include "component/consumer/ConsumerComponent.h"

bool
TransporterComponent::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {
    auto type = info.mNearest.mTypeID;
    if (type == CSPB && !directPowered)return false; //弱充能检查

    if (type == CSPC || type == CSTR || type == CSCA) {
        directPowered = true; //生产者 红石线 能量源
    } else if (type == CSPB &&
               dynamic_cast<PoweredBlockComponent *>(info.mNearest.mComponent)->isPromotedToProducer()) {
        directPowered = true;
    } else if (type && dynamic_cast<ConsumerComponent *>(info.mNearest.mComponent)->isPromotedToProducer()) {
        directPowered = true;
    } else {
        auto dir = info.mCurrent.mDirection;
        if (dir != NEG_Y && dir != invFacing(info.mNearest.mDirection)) {
            //只有这一种情况是非直接激活
            //连接没有被提升为生产者的充能方块or消费者,且两次输入的方向不同，这里还没找到情况，因此暂时不知道
            directPowered = info.mCurrent.mDirection == info.mNearest.mDirection;
        }
    }

    return this->trackPowerSource(info, ++damping, directPowered, 0);
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
