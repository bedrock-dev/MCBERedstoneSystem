//
// Created by xhy on 2021/3/22.
//

#ifndef MCBEREDSTONESYSTEM_TRANSPORTERCOMPONENT_H
#define MCBEREDSTONESYSTEM_TRANSPORTERCOMPONENT_H

#include "component/BaseCircuitComponent.h"


/*
 * 这里是红石粉的逻辑实现
 */
class TransporterComponent : public BaseCircuitComponent {
private:
    int mNextStrength = 0;
public:
    //todo
    bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) override;

    //todo
    bool allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered) override;

    void cacheValues(CircuitSystem &, const BlockPos &pos) override;

    bool canConsumerPower() override { return true; }

    bool evaluate(CircuitSystem &, const BlockPos &) override;

    ComponentTypeID getBaseType() override { return CSTR; }

    ComponentTypeID getInstanceType() override { return CSTR; }


};


#endif //MCBEREDSTONESYSTEM_TRANSPORTERCOMPONENT_H
