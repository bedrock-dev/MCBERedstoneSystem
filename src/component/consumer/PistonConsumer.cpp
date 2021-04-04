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
    //todo
    return false;
}

bool PistonConsumer::allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) {
    return true;
}
