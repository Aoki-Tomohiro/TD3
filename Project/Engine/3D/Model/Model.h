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
		std::vector<std::map<std::string, JointWeightData>> skinClusterData;
		std::vector<Mesh::MeshData> meshData;
		std::vector<Material::MaterialData> materialData;
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

	const size_t GetMeshCount() const { return meshes_.size(); };

	Mesh* GetMesh(size_t index) { return meshes_[index].get(); };

	Material* GetMaterial(size_t index) { return materials_[index].get(); };

	Animation* GetAnimation() { return animation_.get(); };

private:
	std::vector<Model::SkinCluster> CreateSkinCluster(const Animation::Skeleton& skeleton, const ModelData& modelData);

	void CreateBoneLineVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void CreateDebugVertexBuffer();

private:
	ModelData modelData_{};

	std::vector<Model::SkinCluster> skinClusters_{};

	std::vector<std::unique_ptr<Mesh>> meshes_{};

	std::vector<std::unique_ptr<Material>> materials_{};

	std::unique_ptr<Animation> animation_ = nullptr;

	DrawPass drawPass_ = Opaque;

	std::unique_ptr<UploadBuffer> debugVertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW debugVertexBufferView_{};

	std::vector<Vector4> debugVertices_{};

	bool isDebug_ = false;

	friend class ParticleSystem;
};

