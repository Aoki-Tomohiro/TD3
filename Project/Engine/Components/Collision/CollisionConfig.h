#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b0001;

const uint32_t kCollisionAttributeEnemy = 0b0010;
const uint32_t kCollisionAttributeWeapon = 0b0100;
const uint32_t kCollisionAttributeBlock = 0b1000;

//CollisionMask
const uint32_t kCollisionMaskPlayer = 0b1110;
const uint32_t kCollisionMaskEnemy = 0b1101;
const uint32_t kCollisionMaskWeapon = 0b0010;
const uint32_t kCollisionMaskBlock = 0b0011;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = kCollisionPrimitiveSphere << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;