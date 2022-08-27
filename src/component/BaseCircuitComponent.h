//
// Created by xhy on 2021/2/23.
//

#ifndef MCBEREDSTONESYSTEM_BASECIRCUITCOMPONENT_H
#define MCBEREDSTONESYSTEM_BASECIRCUITCOMPONENT_H


#include "CircuitComponentList.h"
#include "CircuitTrackingInfo.h"

class CircuitSystem;

class CircuitSceneGraph;

/*
 * 红石原件的抽象接口类
 */
class BaseCircuitComponent {
public:
    CircuitComponentList mSources; //所有为该红石原件供能的信号源裂变
    bool mIgnoreFirstUpdate = false;
    bool mIsFirstTime = true;
    BlockPos mChunkPosition;
    int mStrength = 0;
    FACING mDirection = FACING::NOT_DEFINED;
    bool mAllowPowerUp = false;
    bool mAllowPowerDown = true;
    bool mRemoved = false;
public:
    bool mShouldEvaluate = true;
    bool mNeedsUpdate = false;

    //vtb+2
    virtual int getStrength() const {
        return this->mStrength;
    }

    //vtb+3
    FACING getDirection() const {
        return this->mDirection;
    }

    //vtb+4
    virtual void setStrength(int strength) {
        this->mStrength = strength;
    }

    //vtb+5
    void setDirection(FACING facing) {
        this->mDirection = facing;
    }

    //vtb+6
    virtual bool consumePowerAnyDirection() { return false; }

    //vtb+7
    virtual bool canConsumerPower() { return false; }

    //vtb+8
    virtual bool canStopPower() { return false; }

    //vtb+9
    virtual void setStopPower(bool stop) {}

    //vtb+10
    virtual ComponentTypeID getBaseType() {
        return CSSC;
    }

    //vtb+11
    virtual ComponentTypeID getInstanceType() {
        return CSSC;
    }

    //vtb+12
    bool removeSource(const BlockPos &pos, BaseCircuitComponent *component);

    //vtb+13
    virtual bool addSource(CircuitSceneGraph *graph, CircuitTrackingInfo &info, int damping, bool &directPowered) {}

    //vtb+14
    virtual bool allowConnection(CircuitSceneGraph *pGraph, CircuitTrackingInfo &info, bool &directPowered);

    //tvb+15
    virtual void checkLock(CircuitSystem &, BlockPos const &) {}

    //vtb+16
    virtual bool evaluate(CircuitSystem &, BlockPos const &) {}

    //vtb+17
    virtual void cacheValues(CircuitSystem &, const BlockPos &pos) {}

    //vtb+18
    virtual void updateDependencies(CircuitSceneGraph *graph, const BlockPos &pos) {}

    //vtb+19
    virtual bool allowIndirect() { return false; }

    //vtb+20
    virtual bool isHalfPulse() {
        return false;
    }

    //vtb+21
    bool hasSource(BaseCircuitComponent *component);


    //vtb+22
    virtual bool hasChildrenSource() { return false; }

    //vtb+23
    virtual bool isSecondaryPowered() { return false; }

    virtual bool hasDirectPower();

    void trackPowerSourceDuplicates(const CircuitTrackingInfo &info, int dampening, bool directlyPowered);

    bool canAllowPowerUp() const {
        return this->mAllowPowerUp;
    }

    bool canAllowPowerDown() const { return this->mAllowPowerDown; }


    void setRemoved() {
        this->mRemoved = true;
    }

    bool isRemoved() const {
        return this->mRemoved;
    }


    bool calculateValue(CircuitSystem *system);

    void clearFirstTimeFlag() { this->mIsFirstTime = false; }


    bool trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data);

};


#endif //MCBEREDSTONESYSTEM_BASECIRCUITCOMPONENT_H
