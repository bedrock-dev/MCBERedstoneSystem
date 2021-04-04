//
// Created by xhy on 2021/2/23.
//

#ifndef MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H
#define MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H

#include "mc/BlockPos.h"

class BaseCircuitComponent;

class CircuitComponentList {
public:
    struct Item {
        BaseCircuitComponent *mComponent = nullptr;
        int mDampening = 0;
        BlockPos mPos;
        FACING mDirection = FACING::NOT_DEFINED;
        bool mDirectlyPowered = false;
        int mData = 0;

        Item() = default;

        Item(BaseCircuitComponent *component, int dampening, BlockPos &pos) : mComponent(component),
                                                                              mDampening(dampening), mPos(pos) {}
    };

    std::vector<Item> mComponents;

    auto end() {
        return mComponents.end();
    }

    auto begin() {
        return mComponents.begin();
    }

    void push_back(Item &item) {
        this->mComponents.push_back(item);
    }

    auto size() const {
        return this->mComponents.size();
    }

    void removeSource(const BlockPos &posSource, const BaseCircuitComponent *component) {
        //todo
    }

};


#endif //MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H
