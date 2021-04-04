//
// Created by xhy on 2021/3/14.
//

#ifndef MCBEREDSTONESYSTEM_CIRCUITSCENEGRAPH_H
#define MCBEREDSTONESYSTEM_CIRCUITSCENEGRAPH_H

#include <unordered_map>
#include "mc/BlockPos.h"
#include <memory>
#include <mc/BlockSource.h>
#include "component/BaseCircuitComponent.h"
#include <queue>

/*
 *这个类是整个红石系统构建的核心
 * 维护了所有的红石原件以及它们之间的相互关系
 * 以及何时对红石原件进行增删改以及进行信号源搜索
 */
class CircuitSceneGraph {
    //存了组件智能指针，裸指针和位置的entry
    struct PendingEntry {
        std::unique_ptr<BaseCircuitComponent> mComponent;
        BlockPos mPos;
        BaseCircuitComponent *mRawComponentPtr;
    };
    //全局红石组件表
    /*
     *{
     * pos1 --> {component1}
     * pos2-->{component2}
     * ...
     * pos n-->{component_n}
     * }
     */
    std::unordered_map<BlockPos, std::unique_ptr<BaseCircuitComponent>> mAllComponents;
    //活跃红石组件表，记载了所有的非充能方块的原件
    /*
     * {cmp1,cpm2,...cmp_n}
     */
    CircuitComponentList mActiveComponents;
    //活跃的区块级别的元件表，记载了每个区块的所有非充能方块原件
    std::unordered_map<BlockPos, CircuitComponentList> mActiveComponentsPerChunk;

    //全局信号源-<消费者列表>
    /*
     * {
     * source_1-->{consumer_11,consumer_12,...}
     * ...
     * source_n-->{consumer_n1,....consumer_nm}
     * }
     */
    std::unordered_map<BlockPos, CircuitComponentList> mPowerAssociationMap;
    //新加入电路的红石原件的一个buffer
    std::unordered_map<BlockPos, PendingEntry> mPendingAdds;
    //等待更新(这里的更新指的是进行信号源的重新搜索)的红石原件的buffer
    std::unordered_map<BlockPos, PendingEntry> mPendingUpdates;
    std::unordered_map<BlockPos, std::vector<BlockPos>> mComponentsToReEvaluate;

    //等待移除的列表
    std::vector<PendingEntry> mPendingRemoves;

public:

    void add(const BlockPos &blockPos, std::unique_ptr<BaseCircuitComponent> component);

    BaseCircuitComponent *getComponent(const BlockPos &pos, ComponentTypeID id);

    BaseCircuitComponent *getBaseComponent(const BlockPos &pos);

    auto getFromPendingAdd(const BlockPos &p, ComponentTypeID typeId);


    CircuitComponentList &getComponents_FastIterationAcrossActive(void);

    auto getComponents_FastLookupByChunkPos() -> decltype(this->mActiveComponentsPerChunk);

    BaseCircuitComponent *getFromPendingAdd(const BlockPos &p);


    inline bool isPendingAdd(const BlockPos &p) {
        return this->mPendingAdds.find(p) != mPendingAdds.end();
    }

    void preSetupPoweredBlocks(const ChunkPos &pos);

    void scheduleRelationshipUpdate(const BlockPos &pos, BaseCircuitComponent *component);

    void update(BlockSource *region);

    void remove(const BlockPos &pos, BaseCircuitComponent *pComponent);


    void removeComponent(const BlockPos &pos);

    void processPendingAdds();

    void processPendingRemoves();

    void processPendingUpdates(BlockSource *bs);

    void removeStaleRelationships();

    void findRelationships(const BlockPos &pos, BaseCircuitComponent *pComponent, BlockSource *pSource);

    BaseCircuitComponent *addIfPoweredBlockAt(BlockSource *blockSource, const BlockPos &pos);

    void addPositionToReEvaluate(const ChunkPos &chunkPos, const BlockPos &pos);

    void searchForRelationshipsAboveAndBelow(
            CircuitComponentList &powerAssociationMap,
            BaseCircuitComponent *currentComponent,
            const BlockPos &targetPos,
            CircuitTrackingInfo &info,
            std::queue<CircuitTrackingInfo> &stack
    );

    void
    checkTransporterComponent(CircuitComponentList &powerAssociationMap,
                              CircuitTrackingInfo &info,
                              std::queue<CircuitTrackingInfo> &queue,
                              BlockPos &pos,
                              FACING dir,
                              bool goingDown);

    void
    checkBaseRailTransporterComponent(CircuitComponentList &powerAssociationMap,
                                      CircuitTrackingInfo &info,
                                      std::queue<CircuitTrackingInfo> &queue,
                                      BlockPos &pos,
                                      FACING dir,
                                      bool goingDown);

};


#endif //MCBEREDSTONESYSTEM_CIRCUITSCENEGRAPH_H
