//
// Created by xhy on 2021/4/4.
//

#ifndef MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H
#define MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H

#include "../ProducerComponent.h"

class RepeaterCapacitor : public CapacitorComponent {

    //todo
    int mInsertAt = 0;
    bool mPowered = false;
    bool mNextPower = false;
    bool mLocked = true;
    int mPulseCount = 0;
    int mPulse = 0;
    int mNextPulse = 0;
    int mOnStates[5]{};

    RepeaterCapacitor() {
        for (int i = 0; i < 4; ++i)
            mOnStates[i] = 2;
    }

    //addSource和AllowConnection不写了


};


#endif //MCBEREDSTONESYSTEM_REPEATERCAPACITOR_H
