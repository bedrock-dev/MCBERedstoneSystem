//
// Created by xhy on 2021/4/4.
//

#include "ComparatorCapacitor.h"

bool
ComparatorCapacitor::addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {
    auto searchDir = info.mCurrent.mDirection;
    if (searchDir == FACING::NEG_Y || searchDir == FACING::POS_Y) return false; //不能从上面和下面搜索到
    if (searchDir == this->getDirection()) return false; //从比较器的输出端搜索，直接返回
    auto type = info.mNearest.mTypeID;
    if (searchDir == invFacing(this->getDirection())) { //如果是从比较器的侧面输入
        //不是红石 中继器 比较器就返回 (比较器侧面只允许这三种进行连接)
        if (type != CSTR && type != MCRR && type != MCCR)return false;
    }

    //同比较器[红石块][充能方块][比较器]无法激活
    if (type == CSPB && !info.mNearest.mComponent->hasDirectPower()) {
        return false;
    }
    return this->trackPowerSourceDuplicates(info, damping, directPowered);
}
