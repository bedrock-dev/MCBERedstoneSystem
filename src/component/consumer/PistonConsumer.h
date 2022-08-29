//
// Created by xhy on 2021/3/27.
//

#ifndef MCBEREDSTONESYSTEM_PISTONCONSUMER_H
#define MCBEREDSTONESYSTEM_PISTONCONSUMER_H

#include "ConsumerComponent.h"

class PistonConsumer : public ConsumerComponent {
    FACING mBlockedFace = FACING::NOT_DEFINED;

    bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) override;

    bool allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) override;

    ComponentTypeID getInstanceType() override {
        return MCPI;
    }

    void setBlockPowerFace(FACING facing);

};


#endif //MCBEREDSTONESYSTEM_PISTONCONSUMER_H
