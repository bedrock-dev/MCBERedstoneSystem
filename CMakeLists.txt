cmake_minimum_required(VERSION 3.17)
project(MCBERedstoneSystem)

set(CMAKE_CXX_STANDARD 14)


add_executable(MCBERedstoneSystem
        src/component/CircuitComponentList.cpp
        src/component/BaseCircuitComponent.cpp
        src/mc/BlockPos.cpp
        src/CircuitSystem.cpp
        src/component/CircuitTrackingInfo.cpp
        src/CircuitSceneGraph.cpp
        src/mc/BlockSource.cpp
        src/component/producer/ProducerComponent.cpp
        src/component/producer/capacitor/RedstoneTorchCapacitor.cpp
        src/component/producer/capacitor/PulseCapacitor.cpp
        src/component/producer/capacitor/RepeaterCapacitor.cpp
        src/component/producer/capacitor/ComparatorCapacitor.cpp
        src/component/transporter/TransporterComponent.cpp
        src/component/consumer/ConsumerComponent.cpp
        src/component/consumer/PistonConsumer.cpp
        src/component/powered_block/PoweredBlockComponent.cpp
        )
include_directories(MCBERedstoneSystem src)
