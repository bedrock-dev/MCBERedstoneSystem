//
// Created by xhy on 2021/3/27.
//

#include "PistonConsumer.h"

//在y轴方向上活塞方块的特殊值和激活方向相反
//因此才来这么一个函数
void PistonConsumer::setBlockPowerFace(FACING facing) {
    if (facing == NEG_Y) {
        this->mBlockedFace = POS_Y;
    } else {
        if (facing == POS_Y) {
            this->mBlockedFace = NEG_Y;
        } else {
            this->mBlockedFace = facing;
        }
    }
}

bool PistonConsumer::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {
    if (this->mBlockedFace == info.mCurrent.mDirection)return false; //从活塞推出面搜索到信号源，直接返回false
    auto mTypeId = info.mNearest.mTypeID;
    //这里应该是电容器的背面无法激活活塞
    //比如中继器的背面对着活塞
    if (mTypeId == CSCA && info.mCurrent.mDirection == info.mNearest.mComponent->getDirection()) {
        return false;
    }

    //这边是充能方块无法[非直接]激活活塞,实测所有的激活方法针对活塞都是直接激活，没有非直接的情况
    if (mTypeId == CSPB && !directPowered) {
        return false;
    }

    this->trackPowerSource(info, damping, directPowered, info.mCurrent.mDirection == invFacing(this->getDirection()));
    return false;
}

bool PistonConsumer::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    return true;
}
