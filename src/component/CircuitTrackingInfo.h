//
// Created by xhy on 2021/2/23.
//

#ifndef MCBEREDSTONESYSTEM_CIRCUITTRACKINGINFO_H
#define MCBEREDSTONESYSTEM_CIRCUITTRACKINGINFO_H

#include "mc/BlockPos.h"

class BaseCircuitComponent;

enum ComponentTypeID : uint64_t {
    CSSC = 1129534275,
    CSPB = 1129533506, //充能方块
    CSPC, //生产者
    CSCA,
    CSTR, //传输者
    CSCC,
    MCPI, //活塞
    MCPR //铁轨
};


class CircuitTrackingInfo {
public:
    struct Entry {
        BaseCircuitComponent *mComponent = nullptr;
        BlockPos mPos;
        FACING mDirection = NEG_Y;
        ComponentTypeID mTypeID = CSSC;
    };
    Entry mCurrent;
    Entry mPower;
    Entry mNearest;
    Entry m2ndNearest;
    bool mDirectlyPowered = true;
    int mData = 0;
    int mDampening;


    CircuitTrackingInfo(BaseCircuitComponent *component, const BlockPos &pos, int dampening);

};

void
initEntry(CircuitTrackingInfo::Entry &entry, BaseCircuitComponent *component,
          const BlockPos &pos);

#endif //MCBEREDSTONESYSTEM_CIRCUITTRACKINGINFO_H
