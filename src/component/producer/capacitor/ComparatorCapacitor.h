//
// Created by xhy on 2021/4/4.
//

#ifndef MCBEREDSTONESYSTEM_COMPARATORCAPACITOR_H
#define MCBEREDSTONESYSTEM_COMPARATORCAPACITOR_H

#include "../ProducerComponent.h"

class ComparatorCapacitor : public CapacitorComponent {
//todo
    int mRearAnalogStrength = -1;
    int mSideAnalogStrengthRight = -1;
    int mSideAnalogStrengthLeft = -1;
    int mOldStrength = 0;
    int mMode = 0;
    int mRearStrength = 0;
    int mSideStrengths = 0;
    int mHasAnalogBeenSet = 0;
    CircuitComponentList mSideComponents;

    FACING getPoweroutDirection() {
        return this->getDirection();
    }

    inline int getOldStrength() const {
        return this->mOldStrength;
    }

    bool allowConnection(CircuitSceneGraph *graph, const CircuitTrackingInfo *info, bool *bDirectlyPowered) {
        return info->mCurrent.mDirection == this->getPoweroutDirection();
    }
    

};


#endif //MCBEREDSTONESYSTEM_COMPARATORCAPACITOR_H
