//
// Created by xhy on 2021/2/23.
//

#ifndef MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H
#define MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H

#include "mc/BlockPos.h"

class BaseCircuitComponent;


/**
 * 信号源表,其实就是对std::vector<Item>的一个简单封装
 */
class CircuitComponentList {
public:
    /*
     * 信号源表内的一个表项
     */
    struct Item {
        BaseCircuitComponent *mComponent = nullptr; //指向信号源的指针
        int mDampening = 0; //和信号源的距离
        BlockPos mPos{}; //信号源的位置
        FACING mDirection = FACING::NOT_DEFINED; //信号源的方向
        bool mDirectlyPowered = false; //是否被信号源直接激活
        int mData = 0; //额外数据值，用途未知

        Item() = default;

        Item(BaseCircuitComponent *component, int dampening, const BlockPos &pos) : mComponent(component),
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

    /**
     * 从信号源表中移除特定位置的信号源,O(n)
     * @param posSource 位置
     * @param component 无效参数
     * @return 是否成功移除
     */
    bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *component) {
        bool found = false;
        auto it = this->mComponents.begin();
        for (; it != this->mComponents.end();) {
            if (it->mPos == posSource) {
                it = this->mComponents.erase(it);
                found = true;
            } else {
                ++it;
            }
        }

        return found;
    }


    //往信号源表尾部添加一个新的信号源
    void add(BaseCircuitComponent *component, int damping, const BlockPos &pos) {
        Item item(component, damping, pos);
        this->mComponents.push_back(item);
    }
};


#endif //MCBEREDSTONESYSTEM_CIRCUITCOMPONENTLIST_H
