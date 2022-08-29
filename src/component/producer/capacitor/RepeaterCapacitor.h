//
// Created by xhy on 2021/4/4.
//

#ifndef MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H
#define MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H

#include "../ProducerComponent.h"


int clamp(int x, int min, int max) {
    if (x < min)return min;
    if (x > max)return max;
    return x;
}

//中继器
//看不懂，比我想的要复杂一些，不知道写的啥玩意

class RepeaterCapacitor : public CapacitorComponent {

    enum Status {
        OFF = 0,
        ON = 1,
        OFF_LOCKED = 2,
        ON_LOCKED = 3
    };

    //todo
    int mInsertAt = 0; //挡位(0,1,2,3)
    bool mPowered = false; //有无信号
    Status mNextPower = OFF;
    bool mLocked = true;
    int mPulseCount = 0;
    int mPulse = 0;
    int mNextPulse = 0;
    Status mOnStates[5]{};
    CircuitComponentList mSideComponents;

    RepeaterCapacitor() {
        for (int i = 0; i < 4; ++i)
            mOnStates[i] = OFF_LOCKED;
    }

    inline FACING getPoweroutDirection() {
        return this->getDirection();
    }

    bool allowConnection(CircuitSceneGraph *graph, const CircuitTrackingInfo *info, bool *bDirectlyPowered) {
        return info->mCurrent.mDirection == this->getPoweroutDirection();
    }


    bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) override;

    void setStrength(int strength) override {
        this->mPowered = strength != 0;
        for (int i = 0; i < this->mInsertAt + 1; ++i) {
            this->mOnStates[i] = this->mPowered ? ON_LOCKED : OFF_LOCKED;
        }
        for (int i = this->mInsertAt + 1; i <= 4; ++i) {
            this->mOnStates[i] = OFF;
        }
    }


    int getStrength() const override {
        return this->mPowered ? 15 : 0;
    }

    void setDelay(int delay) {
        this->mInsertAt = clamp(delay, 0, 3);
    }

    //addSource和AllowConnection不写了
    bool evaluate(CircuitSystem &system, const BlockPos &pos) override;


    void delayPulse(Status status);

    void alternatePulse();

    void extendPulse();
};


#endif //MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H
