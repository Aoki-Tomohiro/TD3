#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "WorldTransform.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/StructuredBuffer.h"
#include "Engine/3D/Camera/Camera.h"
#include <span>

class Model
{
public:
	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData
	{
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	//モデルデータ構造体
	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		Material::MaterialData material;
		Animation::Node rootNode;
	};

	//Influence構造体
	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence
	{
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	//Well構造体
	struct WellForGPU
	{
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};

	//SkinCluster構造体
	struct SkinCluster
	{
		std::vector<Matrix4x4> inverseBindPoseMatrices;
		//Influence
		std::unique_ptr<UploadBuffer> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
		//MatrixPalette
		std::unique_ptr<StructuredBuffer> paletteResource;
		std::span<WellForGPU> mappedPalette;
	};

	void Create(const ModelData& modelData, const std::vector<Animation::AnimationData>& animationData, DrawPass drawPass);

	void Update(WorldTransform& worldTransform, const uint32_t animationNumber);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	void SetIsDebug(const bool isDebug) { isDebug_ = isDebug; };

	Mesh* GetMesh() { return mesh_.get(); };

	Material* GetMaterial() { return material_.get(); };

	Animation* GetAnimation() { return animation_.get(); };

private:
	SkinCluster CreateSkinCluster(const Animation::Skeleton& skeleton, const ModelData& modelData);

	void CreateBoneLineVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void CreateDebugVertexBuffer();

private:
	ModelData modelData_{};

	SkinCluster skinCluster_{};

	std::unique_ptr<Mesh> mesh_ = nullptr;

	std::unique_ptr<Material> material_ = nullptr;

	std::unique_ptr<Animation> animation_ = nullptr;

	DrawPass drawPass_ = Opaque;

	std::unique_ptr<UploadBuffer> debugVertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW debugVertexBufferView_{};

	std::vector<Vector4> debugVertices_{};

	bool isDebug_ = true;

	friend class ParticleSystem;
};

