//
// Created by xhy on 2021/2/23.
//

#include "CircuitSystem.h"

/*
 * 给外部(Dimension对象)调用的接口
 */
void CircuitSystem::update(BlockSource *region) {
    this->mSceneGraph.update(region);
    this->mHasBeenEvaluated = false;
}

/*
 * 更新整个红石电路信号的总接口
 */
void CircuitSystem::evaluate(BlockSource *region) {
    this->_shouldEvaluate(region);
    this->cacheValues();
    this->evaluateComponents(true);
    this->evaluateComponents(false);
    this->checkLocks();
    this->mHasBeenEvaluated = true;
}

/*
 * 确定某个原件在下个红石刻是否需要更新,
 * 如果需要更新就把mShouldEvaluate这个值设为1
 */
void CircuitSystem::_shouldEvaluate(BlockSource *source) {
    if (!source)return;
    auto components = this->mSceneGraph.getComponents_FastLookupByChunkPos();
    for (auto &item:components) {
        auto shouldEvaluate = source->areChunksFullyLoaded(item.first, 32);
        for (auto &comp:item.second) {
            if (comp.mComponent) {
                comp.mComponent->mShouldEvaluate = shouldEvaluate && !comp.mComponent->isRemoved();
            }
        }
    }
}

/*
 * 电路中所有的红石原件计算新值并缓存的过程
 * 不是所有原件都会cacheValue,得看它们的具体实现
 */
void CircuitSystem::cacheValues() {
    auto compMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto &compItem:compMap) {
        auto component = compItem.mComponent;
        if (component->mShouldEvaluate) {
            component->cacheValues(*this, compItem.mPos);
        }
    }
}

/*
 * 锁检查
 * 只对红石中继器有效
 */
void CircuitSystem::checkLocks() {
    auto compMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto &compItem:compMap) {
        auto component = compItem.mComponent;
        if (component->mShouldEvaluate) {
            component->checkLock(*this, compItem.mPos);
        }
    }
}

/*
 * 更新实际信号值的过程
 * 或者叫同步新值和旧值的过程
 * 每个红石刻生产者和非生产者分别进行
 */
void CircuitSystem::evaluateComponents(bool bOnlyProducers) {
    auto compMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto &compItem:compMap) {
        auto component = compItem.mComponent;
        auto typeID = component->getBaseType();
        if (component->mShouldEvaluate) {
            auto typeFilter = typeID == CSPC || typeID == CSCA;
            if (typeFilter == bOnlyProducers) {
                component->evaluate(*this, compItem.mPos);
                component->mNeedsUpdate = true;
            }
        }
    }
}

FACING CircuitSystem::getDirection(const BlockPos &pos) {
    auto component = this->mSceneGraph.getBaseComponent(pos);
    return component ? component->getDirection() : FACING::NOT_DEFINED;
}

int CircuitSystem::getStrength(const BlockPos &pos) {
    auto component = this->mSceneGraph.getBaseComponent(pos);
    return component ? component->getStrength() : -1;
}

bool CircuitSystem::hasDirectPower(const BlockPos &pos) {
    auto component = this->mSceneGraph.getBaseComponent(pos);
    return component != nullptr && component->hasDirectPower();
}

bool CircuitSystem::invalidatePos(const BlockPos &pos) {
    //todo
}

bool CircuitSystem::isAvailableAt(const BlockPos &pos) {
    return this->mSceneGraph.getBaseComponent(pos) != nullptr;
}

void CircuitSystem::preSetupPoweredBlocks(const ChunkPos &pos) {
    //todo
}


void CircuitSystem::removeComponents(const BlockPos &pos) {
    //电路图没有锁住才移除
    if (!this->mLockGraph) {
        auto component = this->mSceneGraph.getBaseComponent(pos);
        this->mSceneGraph.remove(pos, component);
    }
}

void CircuitSystem::setStrength(const BlockPos &pos, int strength) {
    auto component = this->mSceneGraph.getBaseComponent(pos);
    if (component)
        component->setStrength(strength);
}

void CircuitSystem::updateDependencies(BlockSource *blockSource) {
    this->mSceneGraph.update(blockSource);
    this->mHasBeenEvaluated = false;
}

/*
 * 创建一个红石原件
 * 玩家放置一个红石相关的方块的时候会直接触发这个函数(其实上层还有一个泛型函数)
 * @pos 方块位置
 * @direction 方块方向(特殊值)
 * @newComponent 创建的红石组件（由上层泛型函数创建）
 */
BaseCircuitComponent *CircuitSystem::createComponent(const BlockPos &pos, FACING direction,
                                                     std::unique_ptr<BaseCircuitComponent> newComponent) {
    auto component = newComponent.get();
    //设置方向
    component->setDirection(direction);
    //如果当前电路是锁上的就加入失败，返回空指针
    if (this->mLockGraph) {
        newComponent.reset(component);
        return nullptr;
    } else {
        //没锁上就把新的组件传给@SceneGraph
        this->mSceneGraph.add(pos, std::move(newComponent));
        //返回裸指针作为句柄进行后续处理
        return this->mSceneGraph.getFromPendingAdd(pos);
    }
}
