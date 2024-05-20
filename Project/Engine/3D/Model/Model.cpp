#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, const std::vector<Animation::AnimationData>& animationData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//メッシュの作成
	for (uint32_t i = 0; i < modelData.meshData.size(); ++i)
	{
		Mesh* mesh = new Mesh();
		mesh->Initialize(modelData.meshData[i]);
		meshes_.push_back(std::unique_ptr<Mesh>(mesh));
	}

	//マテリアルの作成
	for (uint32_t i = 0; i < modelData.materialData.size(); ++i)
	{
		Material* material = new Material();
		material->Initialize(modelData_.materialData[i].textureFilePath);
		materials_.push_back(std::unique_ptr<Material>(material));
	}

	//アニメーションの作成
	animation_ = std::make_unique<Animation>();
	animation_->Initialize(animationData, modelData_.rootNode);

	//SkinClusterの作成
	skinClusters_ = CreateSkinCluster(animation_->GetSkeleton(), modelData_);

	//Debug用のVertexBufferの生成
	if (!modelData_.skinClusterData[0].empty())
	{
		Animation::Skeleton skeleton = animation_->GetSkeleton();
		CreateBoneLineVertices(skeleton, skeleton.root, debugVertices_);
		CreateDebugVertexBuffer();
	}

	//描画パスを設定
	drawPass_ = drawPass;
}

void Model::Update(WorldTransform& worldTransform, const uint32_t animationNumber)
{
	//アニメーションの更新を行って、骨ごとのLocal情報を更新する
	animation_->ApplyAnimation(modelData_.rootNode.name, animationNumber);

	//アニメーションを適用
	worldTransform.matWorld_ = animation_->GetLocalMatrix() * worldTransform.matWorld_;

	//現在の骨ごとのLocal情報を基にSkeletonSpaceの情報を更新する
	animation_->Update();

	//SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
	const std::vector<Animation::Joint>& joints = animation_->GetJoints();
	for (size_t jointIndex = 0; jointIndex < joints.size(); ++jointIndex)
	{
		for (uint32_t i = 0; i < skinClusters_.size(); ++i)
		{
			assert(jointIndex < skinClusters_[i].inverseBindPoseMatrices.size());
			skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceMatrix = skinClusters_[i].inverseBindPoseMatrices[jointIndex] * joints[jointIndex].skeletonSpaceMatrix;
			skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Mathf::Transpose(Mathf::Inverse(skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceMatrix));
		}
	}
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	for (std::unique_ptr<Material>& material : materials_)
	{
		material->Update();
	}

	//RootのMatrixを適用
	if (modelData_.skinClusterData.empty())
	{
		worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
		worldTransform.TransferMatrix();
	}

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	for (uint32_t i = 0; i < meshes_.size(); ++i)
	{
		uint32_t materialIndex = meshes_[i]->GetMaterialIndex();
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = materials_[materialIndex]->GetTextureHandle();
		if (srvHandle.ptr == 0)
		{
			srvHandle = materials_[materialIndex]->GetTexture()->GetSRVHandle();
		}
		renderer_->AddObject(meshes_[i]->GetVertexBufferView(), skinClusters_[i].influenceBufferView, meshes_[i]->GetIndexBufferView(), materials_[materialIndex]->GetConstantBuffer()->GetGpuVirtualAddress(),
			worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
			srvHandle, skinClusters_[i].paletteResource->GetSRVHandle(), UINT(meshes_[i]->GetIndicesSize()), drawPass_);
	}

	//DebugObjectの追加
	if (isDebug_ && !modelData_.skinClusterData.empty())
	{
		UpdateVertexData(animation_->GetSkeleton(), animation_->GetSkeleton().root, debugVertices_);
		renderer_->AddDebugObject(debugVertexBufferView_, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(), UINT(debugVertices_.size()));
	}
}

std::vector<Model::SkinCluster> Model::CreateSkinCluster(const Animation::Skeleton& skeleton, const ModelData& modelData)
{
	std::vector<SkinCluster> skinClusters{};
	skinClusters.resize(modelData.skinClusterData.size());
	for (uint32_t i = 0; i < modelData.skinClusterData.size(); ++i)
	{
		//palette用のResourceを確保
		skinClusters[i].paletteResource = std::make_unique<StructuredBuffer>();
		skinClusters[i].paletteResource->Create(uint32_t(skeleton.joints.size()), sizeof(WellForGPU));
		WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinClusters[i].paletteResource->Map());
		skinClusters[i].mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする
		//skinCluster.paletteResource->Unmap();

		//influence用のResourceを確保。頂点ごとにinfluenced情報を追加できるようにする
		skinClusters[i].influenceResource = std::make_unique<UploadBuffer>();
		skinClusters[i].influenceResource->Create(sizeof(VertexInfluence) * modelData.meshData[i].vertices.size());
		VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinClusters[i].influenceResource->Map());
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.meshData[i].vertices.size());//0埋め。weightを0にしておく。
		skinClusters[i].mappedInfluence = { mappedInfluence,modelData.meshData[i].vertices.size()};
		//skinCluster.influenceResource->Unmap();

		//Influence用のVBVを作成
		skinClusters[i].influenceBufferView.BufferLocation = skinClusters[i].influenceResource->GetGpuVirtualAddress();
		skinClusters[i].influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.meshData[i].vertices.size());
		skinClusters[i].influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

		//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
		skinClusters[i].inverseBindPoseMatrices.resize(skeleton.joints.size());
		for (Matrix4x4& inverseBindPoseMatrix : skinClusters[i].inverseBindPoseMatrices)
		{
			inverseBindPoseMatrix = Mathf::MakeIdentity4x4();
		}

		//ModelDataを解析してInfluenceを埋める
		for (const auto& jointWeight : modelData.skinClusterData[i])//ModelのSkinClusterの情報を解析
		{
			auto it = skeleton.jointMap.find(jointWeight.first);//JointWeight,firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
			if (it == skeleton.jointMap.end())//そんな名前のJointは存在しない。なので次に回す
			{
				continue;
			}

			//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
			skinClusters[i].inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : jointWeight.second.vertexWeights)
			{
				auto& currentInfluence = skinClusters[i].mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
				for (uint32_t index = 0; index < kNumMaxInfluence; ++index)//空いているところに入れる
				{
					if (currentInfluence.weights[index] == 0.0f)//Weight == 0が空いている状態なので、その場所にweightとjointのindexを代入
					{
						currentInfluence.weights[index] = vertexWeight.weight;
						currentInfluence.jointIndices[index] = (*it).second;
						break;
					}
				}
			}
		}
	}

	return skinClusters;
}

void Model::CreateDebugVertexBuffer()
{
	//頂点バッファを作成
	debugVertexBuffer_ = std::make_unique<UploadBuffer>();
	debugVertexBuffer_->Create(sizeof(Vector4) * debugVertices_.size());

	//頂点バッファビューを作成
	debugVertexBufferView_.BufferLocation = debugVertexBuffer_->GetGpuVirtualAddress();
	debugVertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * debugVertices_.size());
	debugVertexBufferView_.StrideInBytes = sizeof(Vector4);

	//頂点バッファにデータを書き込む
	Vector4* vertexData = static_cast<Vector4*>(debugVertexBuffer_->Map());
	std::memcpy(vertexData, debugVertices_.data(), sizeof(Vector4) * debugVertices_.size());
	debugVertexBuffer_->Unmap();
}

void Model::CreateBoneLineVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	const Animation::Joint& parentJoint = skeleton.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Animation::Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneLineVertices(skeleton, childIndex, vertices);
	}
}

void Model::UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	vertices.clear();
	const Animation::Joint& parentJoint = skeleton.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Animation::Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneLineVertices(skeleton, childIndex, vertices);
	}
	Vector4* vertexData = static_cast<Vector4*>(debugVertexBuffer_->Map());
	std::memcpy(vertexData, vertices.data(), sizeof(Vector4) * vertices.size());
	debugVertexBuffer_->Unmap();
}