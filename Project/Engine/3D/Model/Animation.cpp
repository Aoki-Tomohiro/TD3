#include "Animation.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>

void Animation::Initialize(const std::vector<AnimationData>& animationData, const Node& rootNode)
{
	//アニメーションの初期化
	animationData_ = animationData;

	//ワールド行列の初期化
	localMatrix_ = Mathf::MakeIdentity4x4();

	//スケルトンを作成
	skeletonData_ = CreateSkeleton(rootNode);
}

void Animation::Update()
{
	//全てのJointの更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeletonData_.joints)
	{
		joint.localMatrix = Mathf::MakeAffineMatrix(joint.scale, joint.rotate, joint.translate);
		if (joint.parent)//親がいれば親の行列を掛ける
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeletonData_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else//親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Animation::ApplyAnimation(const std::string& name, const uint32_t animationNumber)
{
	assert(animationNumber < animationData_.size());
	if (animationData_.size() != 0)
	{
		//アニメーションがある場合かつ再生中の場合
		if (isPlay_)
		{
			//一時停止中なら処理を飛ばす
			if (isPause_)
			{
				return;
			}

			//時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応する方が望ましい
			animationTime_ += 1.0f / speed_;

			//最後までいったら最初からリピート再生。リピートしなくても別にいい
			if (animationTime_ > animationData_[animationNumber].duration)
			{
				//ループフラグがtrueならば
				if (isLoop_)
				{
					animationTime_ = 0.0f;
					localMatrix_ = Mathf::MakeIdentity4x4();
				}
				else
				{
					animationTime_ = animationData_[animationNumber].duration;
					isPlay_ = false;
				}
			}
		}

		//RigidAnimation
		if (auto it = animationData_[animationNumber].nodeAnimations.find(name); it != animationData_[animationNumber].nodeAnimations.end())
		{
			NodeAnimation& rootNodeAnimation = (*it).second;
			Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
			localMatrix_ = Mathf::MakeAffineMatrix(scale, rotate, translate);
		}

		//SkeletonAnimation
		for (Joint& joint : skeletonData_.joints)
		{
			//対象のJointのAnimationがあれば、値の適用を行う。下記のif文はc++17から可能になった初期化付きif文。
			if (auto it = animationData_[animationNumber].nodeAnimations.find(joint.name); it != animationData_[animationNumber].nodeAnimations.end())
			{
				const NodeAnimation& rootNodeAnimation = (*it).second;
				joint.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				joint.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				joint.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
			}
		}
	}
}

void Animation::PlayAnimation()
{
	if (!isPause_)
	{
		localMatrix_ = Mathf::MakeIdentity4x4();
		animationTime_ = 0.0f;
	}
	isPlay_ = true;
	isPause_ = false;
}

void Animation::PauseAnimation()
{
	isPause_ = true;
}

void Animation::StopAnimation()
{
	animationTime_ = 0.0f;
	isPlay_ = false;
	isPause_ = false;
	localMatrix_ = Mathf::MakeIdentity4x4();
}

Vector3 Animation::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Mathf::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Mathf::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Animation::Skeleton Animation::CreateSkeleton(const Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints)
	{
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Animation::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Mathf::MakeIdentity4x4();
	joint.scale = node.scale;
	joint.rotate = node.rotate;
	joint.translate = node.translate;
	joint.index = int32_t(joints.size());//現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);//SkeletonのJoint列に追加
	for (const Node& child : node.children)
	{
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	//自身のIndexを返す
	return joint.index;
}