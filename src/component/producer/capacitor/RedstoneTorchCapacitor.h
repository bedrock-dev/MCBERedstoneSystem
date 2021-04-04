//
// Created by xhy on 2021/3/16.
//

#ifndef MCBEREDSTONESYSTEM_REDSTONETORCHCAPACITOR_H
#define MCBEREDSTONESYSTEM_REDSTONETORCHCAPACITOR_H

#include "component/producer/ProducerComponent.h"

class RedstoneTorchCapacitor : public CapacitorComponent {

    //todo
    struct State {
        bool mOn;
        bool mHalfFrame;
    };
    RedstoneTorchCapacitor *nextOrder{};
    int mSelfPowerCount = 0;
    bool mCanReigniteFromBurnout = false;
    State mState[2]{};
public:
    bool _canIncrementSelfPower();

    void addSource();
};


#endif //MCBEREDSTONESYSTEM_REDSTONETORCHCAPACITOR_H
