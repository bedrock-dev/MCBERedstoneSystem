//
// Created by xhy on 2021/3/14.
//

#include "CircuitSceneGraph.h"
#include <queue>
#include <cassert>
#include "component/powered_block/PoweredBlockComponent.h"

namespace {


    /**
     * 把新的消费者加入队列
     * 这个递归的细节真的不想看了，告辞
     * @param graph 电路图
     * @param powerAssociationMap  当前发起搜索的红石信号源目前拥有的消费者
     * @param newComponent  新的消费者
     * @param info  递归信息
     * @param newPos
     * @param face
     * @param queue
     */
    void addToFillQueue(CircuitSceneGraph *graph, //电路图对象
                        CircuitComponentList &powerAssociationMap, //正在进行搜索的信号源所支撑的原件表
                        BaseCircuitComponent *newComponent,  //正在被搜索的原件
                        CircuitTrackingInfo &info, //track info
                        const BlockPos &newPos, //被搜索的原件位置
                        FACING face, //nearest -> 被搜索原件的方向
                        std::queue<CircuitTrackingInfo> &queue //队列
    ) {
        auto newInfo = info;
        //把当前被搜索的原件放到current位置
        initEntry(newInfo.mCurrent, newComponent, newPos);

        auto newDampening = info.mDampening;
        auto bDirectlyPowered = info.mDirectlyPowered;

        //提前就更新映射表的图？
        powerAssociationMap.mComponents.emplace_back(newComponent, 0, newPos);


        //如果nearest允许连接
        if (info.mNearest.mComponent->allowConnection(graph, newInfo, bDirectlyPowered)) {
            //把信号源加入找到的消费者的信号源列表中里面
            //尝试添加成功
            if (newComponent->addSource(graph, newInfo, newDampening, bDirectlyPowered)) {
                //更新track信息
                //2nd <- near
                //near <- cur
                newInfo.m2ndNearest = info.mNearest;
                initEntry(newInfo.mNearest, newComponent, newPos);
                newInfo.mDampening = newDampening;
                newInfo.mDirectlyPowered = bDirectlyPowered;
                //队列中插入新的的信息
                queue.push(newInfo);
            }
        }
    }

    /**
     * 搜索消费者
     * BFS递归过程
     * @param bs   句柄
     * @param powerAssociationMap  当前消费者列表
     * @param graph  全局电路图对象
     * @param trackInfo //递归信息
     * @param facing  //朝向
     * @param pos  //位置
     * @param stack  //队列
     */
    void searchForRelationshipAt(BlockSource *bs, CircuitComponentList &powerAssociationMap, CircuitSceneGraph *graph,
                                 CircuitTrackingInfo &trackInfo, FACING facing,
                                 const BlockPos &pos, //IDA中没有这个参数，但是加上更合理
                                 std::queue<CircuitTrackingInfo> &queue) {
        auto newPos = pos + facingToBlockPos(facing);


        auto newComponent = graph->getBaseComponent(newPos);
        //新方块加入队列
        if (pos != trackInfo.mNearest.mPos && pos != trackInfo.m2ndNearest.mPos) {
            addToFillQueue(graph, powerAssociationMap, newComponent, trackInfo, newPos, facing, queue);
        }
    }

}

/*
 * 玩家放下红石相关方块后会触发这个函数
 * @BlockPos 方块的坐标
 * @component 红石组件 这个是由@CircuitSysttem 传递过来的参数
 * @return 无
 *
 * 这个函数只有部分情况会直接把新的组件加入pendingAdd列表中
 * - 目前不知道这么写的意义是什么
 * - 如果有这个方块且是充能方块不加入
 * - 其他情况都加入
 */
void CircuitSceneGraph::add(const BlockPos &blockPos, std::unique_ptr<BaseCircuitComponent> component) {
    auto iter = this->mPendingAdds.find(blockPos);
    auto &pending = iter->second;
    //如果当前的add列表有这个方块且这个组件的类型是充能方块
    if (iter != mPendingAdds.end() && iter->second.mComponent->getBaseType() == CSPB) {
        if (pending.mComponent->canAllowPowerUp()) {
            return;
        } else {
            pending.mComponent.reset(component.get());
        }
        //没有这个方块或者有但不是充能方块
    } else {
        PendingEntry pendingEntry;
        pendingEntry.mComponent.reset(component.get());
        pendingEntry.mPos = blockPos;
        this->mPendingAdds[blockPos] = std::move(pendingEntry);
    }

    if (component->canAllowPowerUp()) {
        pending.mComponent.reset(nullptr);
        pending.mComponent = std::move(component);
    }
}

/**
 * 从全局元件表中获取特定位置的原件
 * @param pos 位置
 * @return  原件的指针
 */
BaseCircuitComponent *CircuitSceneGraph::getBaseComponent(const BlockPos &pos) {
    auto iter = this->mAllComponents.find(pos);
    if (iter == mAllComponents.end() || iter->second->isRemoved()) {
        return nullptr;
    }
    return iter->second.get();
}

/**
 * 从全局元件表获取指定位置指定类型的原件
 * @param pos 位置
 * @param id 类型
 * @return 返回原该原件的指针
 */
BaseCircuitComponent *CircuitSceneGraph::getComponent(const BlockPos &pos, ComponentTypeID id) {
    auto comp = this->getBaseComponent(pos);
    return comp->getBaseType() == id ? comp : nullptr;
}

CircuitComponentList &CircuitSceneGraph::getComponents_FastIterationAcrossActive() {
    return this->mActiveComponents;
}

auto CircuitSceneGraph::getComponents_FastLookupByChunkPos() -> decltype(this->mActiveComponentsPerChunk) {
    return this->mActiveComponentsPerChunk;
}

BaseCircuitComponent *CircuitSceneGraph::getFromPendingAdd(const BlockPos &p) {
    auto iter = this->mPendingAdds.find(p);
    return iter != mPendingAdds.end() ? iter->second.mComponent.get() : nullptr;
}

auto CircuitSceneGraph::getFromPendingAdd(const BlockPos &p, ComponentTypeID id) {
    auto comp = this->getFromPendingAdd(p);
    return comp->getBaseType() == id ? comp : nullptr;
}

void CircuitSceneGraph::preSetupPoweredBlocks(const ChunkPos &pos) {
    BlockPos blockPos(pos.x << 4, 0, pos.z << 4);
    auto chunkPosIter = this->mComponentsToReEvaluate.find(blockPos);
    if (chunkPosIter != mComponentsToReEvaluate.end()) {
        auto &blocks = chunkPosIter->second;
        for (auto &block: blocks) {
            auto comp = this->getBaseComponent(block);
            this->scheduleRelationshipUpdate(block, comp);
        }
        this->mComponentsToReEvaluate.erase(chunkPosIter);
    }
}

/*
 * 更新原件component的电路连接状态
 * @pos 原件位置
 * @component 原件自身（这个元件只能是生产者或者电容器，不可能是消费者）
 * 这个函数的作用就是从自身出发，寻找自己能够提供能量的红石原件，
 */
void CircuitSceneGraph::scheduleRelationshipUpdate(const BlockPos &pos, BaseCircuitComponent *component) {
    if (!component) return;
    auto typeID = component->getBaseType();
    //只有生产者和电容器才能作为信号源来提供能量
    if (typeID == CSCA || typeID == CSPB) {
        PendingEntry entry = {std::unique_ptr<BaseCircuitComponent>(component), pos, component};
        //加入待更新列表
        this->mPendingUpdates.insert({pos, std::move(entry)});
    }
}

void CircuitSceneGraph::update(BlockSource *pSource) {
    this->processPendingRemoves();
    this->processPendingAdds();
    this->processPendingUpdates(pSource);
}

void CircuitSceneGraph::remove(const BlockPos &pos, BaseCircuitComponent *component) {
    //从等待加入的列表中移除，防止被重新加入
    this->mPendingAdds.erase(pos);
    if (component) {
        //标记为已经移除
        component->setRemoved();
        //加入待移除列表
        this->mPendingRemoves.emplace_back(component, pos, component);
    }
}


//把pend ing列表中的红石组件正式加入到电路中
void CircuitSceneGraph::processPendingAdds() {
    for (auto iter = this->mPendingAdds.begin();; iter = this->mPendingAdds.erase(iter)) {
        //遍历pendingAdd列表,找到一个删去一个
        if (iter == mPendingAdds.end())break;
        //下面是具体行为
        //取出一个entry
        auto &pendingToAdd = iter->second;
        //pending中存的组件对象
        //在全局元件表中寻找这个等待加入的原件
        auto componentGroupIter = this->mAllComponents.find(pendingToAdd.mPos);

        bool flagV19 = false;
        bool needProcess = true;
        if (componentGroupIter != mAllComponents.end() && componentGroupIter->second) {
            //如果在全局元件表中找到了这个等待加入的原件
            //如果找到的这个已存在的原件不是空指针
            if (componentGroupIter->second->getBaseType() == CSPB) {
                flagV19 = pendingToAdd.mComponent->getBaseType() != CSPB;
            }
        }

        //如果没找到自然需要处理，因此needProcess默认是true
        //如果找到了也有一种需要处理的情况：
        // 就是当前原件是充能方块且等待加入的不是充能方块
        if (componentGroupIter != mAllComponents.end()) {
            needProcess = flagV19;
        }
        if (!needProcess)return;
        //下面开始正式地处理
        if (flagV19) {
            //这里是对充能方块的特判的情况
            auto allIter = this->mAllComponents.begin();
            while (allIter != this->mAllComponents.end()) {
                //遍历当前所有组件，如果有个组件的信号源是充能方块就把这个方块移除
                if (allIter->second->hasSource(componentGroupIter->second.get())) {
                    allIter->second->removeSource(componentGroupIter->first, componentGroupIter->second.get());
                }
                ++allIter;
            }
            this->mAllComponents.erase(componentGroupIter);
        }
        //下面是所有方块都需要的情况，正式把pending列表中的组件加入红石电路中
        //把pd原件加入全局红石组件表
        auto component = pendingToAdd.mComponent.get();
        this->mAllComponents.insert({pendingToAdd.mPos, std::move(pendingToAdd.mComponent)});
        auto typeID = component->getBaseType();
        //如果新加入的组件不是充能方块
        if (typeID != CSPB) {
            //如果不是充能方块就加入活跃元件表和活跃区块元件表
            this->mActiveComponents.mComponents.emplace_back(component, 0, pendingToAdd.mPos);
            this->mActiveComponentsPerChunk[component->mChunkPosition].mComponents.emplace_back(component, 0,
                                                                                                pendingToAdd.mPos);
        }
        //如果新加入的方块是生产者
        if (typeID == CSPC) {
            //重新构建电路，更新信号源和寻找依赖
            this->scheduleRelationshipUpdate(pendingToAdd.mPos, component);
        } else if (typeID == CSCA) {
            //重新构建电路，更新信号源和寻找依赖
            this->scheduleRelationshipUpdate(pendingToAdd.mPos, component);
            for (int c1 = -1; c1 <= 1; c1 += 2) {
                for (int c2 = -1; c2 <= 1; c2 += 2) {
                    //获取四个角的方块
                    //如果四角是电容器这四个角的方块也更新下
                    auto pos = pendingToAdd.mPos + BlockPos(c1, 0, c2);
                    auto possibleComponent = this->getBaseComponent(pos);
                    if (possibleComponent && possibleComponent->getBaseType() == CSCA) {
                        this->scheduleRelationshipUpdate(pos, possibleComponent);
                    }
                }
            }
        }

        for (int y = -1; y < 1; y++) {
            for (int face = 0; face < 6; face++) {
                //获取竖直方向上的两个方块的六面所有方块
                auto pos = pendingToAdd.mPos + BlockPos(0, y, 0) + facingToBlockPos((FACING) face);
                auto possibleComponent = this->getBaseComponent(pos);
                if (possibleComponent) {
                    auto tID = possibleComponent->getBaseType();
                    if (tID != CSPC && tID != CSCA) {
                        for (auto item: possibleComponent->mSources) {
                            this->scheduleRelationshipUpdate(item.mPos, item.mComponent);
                        }
                    } else {
                        this->scheduleRelationshipUpdate(pos, possibleComponent);
                    }
                }
            }
        }
    }
}

//从pending列表中取出等待移除的组件，将其从红石电路中移除
void CircuitSceneGraph::processPendingRemoves() {
    //遍历等待移除的列表，进行真正的移除操作
    for (auto &compEntry: this->mPendingRemoves) {
        this->removeComponent(compEntry.mPos);
    }
    this->mPendingRemoves.clear();
}

/*
 * 集中处理需要更新信号源的原件(仅包括生产者和电容器,这个列表不会包含消费者)
 * @bs BlockSource
 */
void CircuitSceneGraph::processPendingUpdates(BlockSource *bs) {
    if (this->mPendingUpdates.empty())return;
    //移待更新方块的消费者
    this->removeStaleRelationships();
    auto iter = this->mPendingUpdates.begin();
    //遍历整个待更新列表，寻找信号源-消费者关系
    while (iter != this->mPendingUpdates.end()) {
        if (iter->second.mRawComponentPtr) {
            this->findRelationships(iter->second.mPos, iter->second.mRawComponentPtr, bs);
        }
        iter = this->mPendingUpdates.erase(iter);
    }

    //遍历所有原件并更新依赖
    for (auto &component: this->mAllComponents) {
        //更新依赖
        //这个函数只有电容器才具有实际行为(包括红石火把,比较器和中继器)
        component.second->updateDependencies(this, component.first);
    }
}

//移除稳定的关系
void CircuitSceneGraph::removeStaleRelationships() {
    for (auto &pendingItem: this->mPendingUpdates) {
        auto updatePos = pendingItem.second.mPos;
        //在全局电源关系图中找到该方块充当信号源的关系图
        auto powerAssociationIter = this->mPowerAssociationMap.find(updatePos);
        if (powerAssociationIter != this->mPowerAssociationMap.end()) {
            //如果找到了
            auto relationships = powerAssociationIter->second;
            for (auto reIter = relationships.begin(); reIter != relationships.end(); ++reIter) {
                //遍历它们的每一个消费者
                auto iter = this->mAllComponents.find(reIter->mPos);
                if (iter != mAllComponents.end()) {
                    //消费者移除指向这个待更新方块的信号源
                    iter->second->removeSource(updatePos, pendingItem.second.mRawComponentPtr);
                    relationships.mComponents.erase(reIter);
                } else {
                    ++reIter;
                }
            }
        }
    }
}


/*
 * 这个函数是真的给生产者寻找自己能提供能量的消费者的函数
 * @pos，生产者的位置
 * @producerTarget 生产者本身(包括电容器)
 */
void
CircuitSceneGraph::findRelationships(const BlockPos &pos, BaseCircuitComponent *producerTarget, BlockSource *pSource) {
//在这里更新电路连接

    auto powerAssociationIter = this->mPowerAssociationMap.find(pos);
    //信号源列表没有这个信号源就加入
    if (powerAssociationIter == mPowerAssociationMap.end()) {
        this->mPowerAssociationMap.insert({pos, CircuitComponentList()});
    }
//    //在信号源列表中找到这个信号源，前面刚插入，这里不可能没有，因为mc是单线程的
//    auto sourceIter = this->mPowerAssociationMap.find(pos);
//    assert(sourceIter != mPowerAssociationMap.end());


    std::queue<CircuitTrackingInfo> queue;
    //创建一个初始的信息,距离是0,Entry内的4个entry都是当前生产者
    //info内的信息 (power->...->2ndNear->near->cur)
    CircuitTrackingInfo startInfo(producerTarget, pos, 0);


    //开始做BFS
    //队列中加入这个信号源
    queue.push(startInfo);
    do {

        //典型的BFS结构
        auto fillTrack = queue.front();
        queue.pop();
        //取出队列首部元素
        auto targetPos = fillTrack.mNearest.mPos;

        auto currentComponent = fillTrack.mNearest.mComponent; //这里暂时不知道为啥要用Nearest
        if (currentComponent) {
            int damping = fillTrack.mDampening;
            auto dir = fillTrack.mNearest.mComponent->getDirection();
            //所有操作都要在damping在15格以内
            if (damping <= 15) {


                /*这里是搜索充能方块并加入更新表*/
                if (currentComponent->getBaseType() != CSPB) { //如果不是充能方块
                    for (auto facing = 0; facing < 6; facing++) {
                        //遍历六面的方块位置
                        auto newPos = targetPos + facingToBlockPos(static_cast<FACING>(facing));

                        //如果当前电路中没有这个红石原件,这就是说明搜索到充能方块了
                        if (!this->getBaseComponent(newPos) && pSource) {
                            //猜测意思是区块已经加载，就更新充能方块信息
                            if (pSource->hasChunksAt(newPos, 0)) {
                                this->addIfPoweredBlockAt(pSource, newPos);
                            } else {
                                //如果区块没加载，就暂存等后面加载
                                ChunkPos chunkPos = newPos.toChunkPos();
                                this->addPositionToReEvaluate(chunkPos, targetPos);
                            }
                        }
                    }
                }



                //这里是实际的电路构建连接
                for (auto face = 0; face < 6; face++) {
                    searchForRelationshipAt(pSource, powerAssociationIter->second, this, fillTrack,
                                            static_cast<FACING>(face), targetPos, queue);
                }

                //搜索上下方向的关系?
                this->searchForRelationshipsAboveAndBelow(powerAssociationIter->second,
                                                          currentComponent,
                                                          pos,
                                                          startInfo,
                                                          queue
                );
            }
        }

    } while (!queue.empty());

}


/**
 * 尝试把方块作为充能方块加入红石电路
 * @param blockSource  方块源
 * @param pos  坐标
 */
BaseCircuitComponent *CircuitSceneGraph::addIfPoweredBlockAt(BlockSource *blockSource, const BlockPos &pos) {
    //check block property and solid and so on
    auto powerBlockComponent = std::make_unique<PoweredBlockComponent>();
    this->mAllComponents.insert({pos, std::move(powerBlockComponent)});
    //set allow as power source
    //if property & 0x800000>0 set allow power up
    //??why stack memory
    return powerBlockComponent.get();
}

/**
 * 把位置加到重新计算列表
 * @param chunkPos(区块坐标)
 * @param pos (具体位置)
 */
void CircuitSceneGraph::addPositionToReEvaluate(const ChunkPos &chunkPos, const BlockPos &pos) {
    BlockPos p = {chunkPos.x << 4, 0, chunkPos.z << 4};
    this->mComponentsToReEvaluate[p].push_back(pos);
}

//把原件从电路中移除的实际操作
//从各种表中移除原件并更新电路连接
void CircuitSceneGraph::removeComponent(const BlockPos &pos) {
    //全局元件表中查找这个原件，找不到直接返回
    auto componentGroupIter = this->mAllComponents.find(pos);
    if (componentGroupIter == mAllComponents.end())return;
    //找到了进行具体操作
    //从全局元件表中移除
    this->mAllComponents.erase(pos);
    auto component = componentGroupIter->second.get();
    //如果不是充能方块就把这个方块从信号激活图中移除
    if (component->getBaseType() != CSPB) {
        this->mPowerAssociationMap.erase(pos);
    }

    //从区块活跃元件表中移除
    for (auto &item: this->mActiveComponentsPerChunk) {
        item.second.removeSource(pos, component);
    }

    auto sources = component->mSources;
    for (auto &item: sources.mComponents) {
        auto updateComponent = item.mComponent;
        //重新计算这个原件的所有连接的原件
        this->scheduleRelationshipUpdate(item.mPos, item.mComponent);
    }

    //再次遍历全局元件表，删除相关依赖
    for (auto &allComp: this->mAllComponents) {
        auto updateComponent = allComp.second.get();
        updateComponent->removeSource(pos, component);
        auto iter = updateComponent->mSources.mComponents.begin();
        while (iter != updateComponent->mSources.mComponents.end()) {
            if (iter->mPos == pos || iter->mComponent == component) {
                this->scheduleRelationshipUpdate(allComp.first, updateComponent);
            } else {
                ++iter;
            }
        }

    }

    //更新六个方向的所有红石组件以及它们的信号源
    for (int face = 0; face < 6; ++face) {
        auto position = pos + facingToBlockPos((FACING) face);
        auto updateComponent = this->getBaseComponent(position);
        if (updateComponent) {
            this->scheduleRelationshipUpdate(pos, updateComponent);
            for (auto &item: updateComponent->mSources.mComponents) {
                this->scheduleRelationshipUpdate(item.mPos, item.mComponent);
            }
        }
    }

    this->mPendingUpdates.erase(pos);
    componentGroupIter->second.reset(nullptr);
}

void CircuitSceneGraph::searchForRelationshipsAboveAndBelow(CircuitComponentList &powerAssociationMap,
                                                            BaseCircuitComponent *currentComponent,
                                                            const BlockPos &targetPos,
                                                            CircuitTrackingInfo &info,
                                                            std::queue<CircuitTrackingInfo> &stack) {
    auto typeID = currentComponent->getBaseType();
    if (typeID == CSPB) {
        auto otherPos = targetPos + facingToBlockPos(POS_Y);
        //获取上方的充能方块
        auto up = this->getComponent(otherPos, CSPB);
        if (!up || up->canAllowPowerUp()) {
            for (auto facing = 2; facing <= 5; facing++) {
                checkTransporterComponent(powerAssociationMap, info, stack, otherPos, static_cast<FACING>(facing),
                                          false);
            }
        }
        otherPos = targetPos + facingToBlockPos(NEG_Y);
        auto down = this->getComponent(otherPos, CSPB);
        if (!down || down->canAllowPowerUp()) {
            for (auto facing = 2; facing <= 5; facing++) {
                checkTransporterComponent(powerAssociationMap, info, stack, otherPos, static_cast<FACING>(facing),
                                          false);
            }
        }
    } else if (typeID != CSPC || currentComponent->getDirection() != NEG_Y) {
        auto otherPos = targetPos + facingToBlockPos(POS_Y);
        for (auto facing = 2; facing <= 5; facing++) {
            checkBaseRailTransporterComponent(powerAssociationMap, info, stack, otherPos, static_cast<FACING>(facing),
                                              false);
        }
        auto down = this->getComponent(otherPos, CSPB);
        for (auto facing = 2; facing <= 5; facing++) {
            checkBaseRailTransporterComponent(powerAssociationMap, info, stack, otherPos, static_cast<FACING>(facing),
                                              false);
        }
    } else {
        auto topPos = targetPos + facingToBlockPos(POS_Y);
        auto downPos = targetPos + facingToBlockPos(NEG_Y);
        auto topComp = getComponent(topPos, CSPB);
        auto downComp = getComponent(downPos, CSPB);
        if (topComp) { //todo
            for (auto facing = 2; facing <= 5; facing++) {
                checkBaseRailTransporterComponent(powerAssociationMap, info, stack,
                                                  const_cast<BlockPos &>(targetPos),
                                                  static_cast<FACING>(facing),
                                                  false);
            }
        }
    }
}


//寻找前后左右四个方向的红石线加入队列
void CircuitSceneGraph::checkTransporterComponent(CircuitComponentList &powerAssociationMap,
                                                  CircuitTrackingInfo &info,
                                                  std::queue<CircuitTrackingInfo> &queue,
                                                  BlockPos &pos, FACING dir,
                                                  bool goingDown) {
    auto position = pos + facingToBlockPos(dir);
    auto component = this->getComponent(position, CSTR);
    addToFillQueue(this, powerAssociationMap, component, info, position, dir, queue);
}

void CircuitSceneGraph::checkBaseRailTransporterComponent(CircuitComponentList &powerAssociationMap,
                                                          CircuitTrackingInfo &info,
                                                          std::queue<CircuitTrackingInfo> &queue, BlockPos &pos,
                                                          FACING dir, bool goingDown) {
    auto position = pos + facingToBlockPos(dir);
    auto component = this->getComponent(position, MCPR);
    addToFillQueue(this, powerAssociationMap, component, info, position, dir, queue);
}







