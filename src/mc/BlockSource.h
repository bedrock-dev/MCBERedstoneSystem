//
// Created by xhy on 2021/3/16.
//

#ifndef MCBEREDSTONESYSTEM_BLOCKSOURCE_H
#define MCBEREDSTONESYSTEM_BLOCKSOURCE_H

#include "BlockPos.h"
class BlockSource {
public:
    bool  areChunksFullyLoaded(const BlockPos&pos,int radius);

    bool hasChunksAt(BlockPos pos, int i);
};


#endif //MCBEREDSTONESYSTEM_BLOCKSOURCE_H
