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
