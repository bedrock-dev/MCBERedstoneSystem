//
// Created by xhy on 2021/4/4.
//

#include "RepeaterCapacitor.h"

void RepeaterCapacitor::delayPulse(RepeaterCapacitor::Status status) {
    //todo
}

bool RepeaterCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos) {
    auto oldPowered = this->mPowered;
    if (this->mLocked) { //NOLINT
        return false;
    }
    this->delayPulse(this->mNextPower);
    if (this->mInsertAt > 0) {
        auto v5 = this->mPulse ? this->mPulseCount == 2 : false;
        if (this->mPulseCount > this->mInsertAt && !v5) {
            this->alternatePulse();
        } else {
            this->extendPulse();
        }
    }

    auto powered = true;

    this->mPowered = this->mOnStates[0] != ON ? ON_LOCKED : ON;
    return oldPowered != this->mPowered;

}

void RepeaterCapacitor::alternatePulse() {
    auto currentStatus = OFF;
    if (this->mInsertAt > 0) {
        currentStatus = this->mOnStates[0];
        if (currentStatus == OFF || currentStatus == ON) {
            auto newStatus = this->mNextPulse ? ON_LOCKED : OFF_LOCKED;
            this->mNextPulse = ~this->mNextPulse;
            for (int i = 0; i < this->mInsertAt + 1; ++i) {
                this->mOnStates[i] = newStatus;
            }
        }
    }
}

void RepeaterCapacitor::extendPulse() {

}

bool RepeaterCapacitor::addSource(CircuitSceneGraph *graph, const CircuitTrackingInfo *info, int *dampening,
                                  bool *bDirectlyPowered) {
    //这两个参数不知道写反了没，因为不清楚中继器的默认方向是咋样的
    auto forward = this->getDirection() == info->mCurrent.mDirection; //信号到来的方向和中继器方向是否相反
    auto backward = invFacing(this->getDirection()) == info->mCurrent.mDirection; //信号是不是从输入端来的
    if (forward)return false; //从反方向来的直接返回false

    //如果是充能方块且(充能方块没有信号源 or 充能方块是非直接激活）直接返回
    //类似这种电路  [红石][铁块][中继器]三个连一起，红石没法搜索到中继器
    if (info->mNearest.mTypeID == CSPB && (!info->mNearest.mComponent->hasDirectPower() || !bDirectlyPowered))
        return false;
    auto type = info->mNearest.mComponent->getInstanceType();
    if (backward || type == MCRR || type == MCCR) {
        //如果上一个原件是比较器或者中继器就两边也能输入（只有中继器和比较器能锁存中继器）
        //如果不是，只能从输入端输入
        this->trackPowerSource(*info, *dampening, *bDirectlyPowered, backward);
    }

    return true;
}
