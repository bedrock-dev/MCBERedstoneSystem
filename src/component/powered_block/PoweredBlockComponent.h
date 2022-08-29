//
// Created by xhy on 2021/3/31.
//

#ifndef MCBEREDSTONESYSTEM_POWEREDBLOCKCOMPONENT_H
#define MCBEREDSTONESYSTEM_POWEREDBLOCKCOMPONENT_H

#include "component/BaseCircuitComponent.h"

class PoweredBlockComponent : public BaseCircuitComponent {
    bool mPromotedToProducer = false;
    bool mAllowAsPowerSource = true;
    bool mIsFirstTime = false;
public:
    bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) override;

    bool allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) override;


    bool canConsumerPower() override { return true; }

    bool evaluate(CircuitSystem &, const BlockPos &) override {
        return false;
    }

    ComponentTypeID getBaseType() override { return CSPB; }

    ComponentTypeID getInstanceType() override { return CSPB; }

    int getStrength() const override;

    bool hasChildrenSource() override { return true; }

    bool isAllowAsPowerSource() const {
        return this->mAllowAsPowerSource;
    };

    bool isPromotedToProducer() const { return this->mPromotedToProducer; }
};


#endif //MCBEREDSTONESYSTEM_POWEREDBLOCKCOMPONENT_H
