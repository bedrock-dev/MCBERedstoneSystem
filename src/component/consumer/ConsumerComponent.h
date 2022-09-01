//
// Created by xhy on 2021/3/22.
//

#ifndef MCBEREDSTONESYSTEM_CONSUMERCOMPONENT_H
#define MCBEREDSTONESYSTEM_CONSUMERCOMPONENT_H

#include "component/BaseCircuitComponent.h"

class ConsumerComponent : public BaseCircuitComponent {

    bool mSecondaryPowered = false;
    bool mPropagatePower = false; //是否能强充能后面的方块
    bool mPromotedToProducer = false;
    bool mAcceptHalfPulse = false; //是否接受半脉冲
public:
    

    bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) override;

    bool allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) override;

    bool canConsumerPower() override { return true; }

    bool evaluate(CircuitSystem &, const BlockPos &) override;

    ComponentTypeID getBaseType() override { return CSCC; }

    ComponentTypeID getInstanceType() override { return CSCC; }

    bool isPromotedToProducer() const { return this->mPromotedToProducer; }

    bool isSecondaryPowered() override { return this->mSecondaryPowered; }

    void setAcceptHalfPulse(bool accept) { this->mAcceptHalfPulse = accept; }

    void setPropagatePower(bool propagatePower) { this->mPropagatePower = propagatePower; }

};


#endif //MCBEREDSTONESYSTEM_CONSUMERCOMPONENT_H
