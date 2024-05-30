#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer =   0b0000001;
const uint32_t kCollisionAttributeEnemy =    0b0000010;
const uint32_t kCollisionAttributeWeapon =   0b0000100;
const uint32_t kCollisionAttributeNoWeapon = 0b0001000;
const uint32_t kCollisionAttributeBlock =    0b0010000;
const uint32_t kCollisionAttributeSwitch =   0b0100000;
const uint32_t kCollisionAttributeWall =     0b1000000;

//CollisionMask
const uint32_t kCollisionMaskPlayer =   0b1110010;
const uint32_t kCollisionMaskEnemy =    0b1011101;
const uint32_t kCollisionMaskWeapon =   0b0000010;
const uint32_t kCollisionMaskNoWeapon = 0b0000010;
const uint32_t kCollisionMaskBlock =    0b0000011;
const uint32_t kCollisionMaskSwitch =   0b0000001;
const uint32_t kCollisionMaskWall =     0b0000011;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = kCollisionPrimitiveSphere << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;