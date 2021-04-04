//
// Created by xhy on 2021/3/16.
//

#include "RedstoneTorchCapacitor.h"

bool RedstoneTorchCapacitor::_canIncrementSelfPower() {
    return this->mCanReigniteFromBurnout ? false ? this->mSelfPowerCount <= 32;
}
