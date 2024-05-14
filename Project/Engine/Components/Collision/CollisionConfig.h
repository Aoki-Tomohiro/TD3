#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b00001;
const uint32_t kCollisionAttributeEnemy = 0b00010;
const uint32_t kCollisionAttributeWeapon = 0b00100;
const uint32_t kCollisionAttributeNoWeapon = 0b01000;
const uint32_t kCollisionAttributeBlock = 0b10000;

//CollisionMask
const uint32_t kCollisionMaskPlayer = 0b10010;
const uint32_t kCollisionMaskEnemy = 0b11101;
const uint32_t kCollisionMaskWeapon = 0b00010;
const uint32_t kCollisionMaskNoWeapon = 0b00010;
const uint32_t kCollisionMaskBlock = 0b00011;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = kCollisionPrimitiveSphere << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;