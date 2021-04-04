//
// Created by xhy on 2021/2/23.
//

#ifndef MCBEREDSTONESYSTEM_CIRCUITSYSTEM_H
#define MCBEREDSTONESYSTEM_CIRCUITSYSTEM_H

#include "CircuitSceneGraph.h"
#include <vector>
#include "mc/BlockSource.h"

/*
 * 大体上就是CircuitSceneGraph的一个封装
 */
class CircuitSystem {
    struct LevelChunkTracking {

    };
    bool mLockGraph = true;
    CircuitSceneGraph mSceneGraph;
    std::vector<LevelChunkTracking> mAddedLevelChunk;
    bool mHasBeenEvaluated = true;

    void _shouldEvaluate(BlockSource *source);

public:
    void update(BlockSource *region);

    void evaluate(BlockSource *region);


    void cacheValues();

    void evaluateComponents(bool bOnlyProducers);

    void checkLocks();


    BaseCircuitComponent *
    createComponent(const BlockPos &pos, FACING direction, std::unique_ptr<BaseCircuitComponent> newComponent);

    FACING getDirection(const BlockPos &pos);

    int getStrength(const BlockPos &pos);

    bool hasDirectPower(const BlockPos &pos);

    bool invalidatePos(const BlockPos &pos);

    bool isAvailableAt(const BlockPos &pos);

    inline bool lockGraph(bool bLock) { this->mLockGraph = bLock; }

    void preSetupPoweredBlocks(const ChunkPos &pos);


    /*
     * 部分方块被移除的回调函数里面会调用这个函数用于移除红石系统中的红石组件
     */
    void removeComponents(const BlockPos &pos);

    void setStrength(const BlockPos &pos, int strength);

    void updateDependencies(BlockSource *blockSource);
};


#endif //MCBEREDSTONESYSTEM_CIRCUITSYSTEM_H
