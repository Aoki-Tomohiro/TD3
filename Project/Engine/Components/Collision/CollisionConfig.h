#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b0001;
const uint32_t kCollisionAttributeEnemy = 0b00010;
const uint32_t kCollisionAttributeWeapon = 0b00100;
const uint32_t kCollisionAttributeBlock = 0b01000;
const uint32_t kCollisionAttributeBomb = 0b10000;

//CollisionMask
const uint32_t kCollisionMaskPlayer = 0b11110;
const uint32_t kCollisionMaskEnemy = 0b01101;
const uint32_t kCollisionMaskWeapon = 0b00010;
const uint32_t kCollisionMaskBlock = 0b10011;
const uint32_t kCollisionMaskBomb = 0b11001;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = kCollisionPrimitiveSphere << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;