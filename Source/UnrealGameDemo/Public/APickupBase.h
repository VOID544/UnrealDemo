// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// 需要用到静态网格组件和球体碰撞组件的声明
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// 前向声明你的背包组件类（假设名为 UMyInventoryComponent，
// 只是声明名字，这里不需要包含它的头文件，减少编译依赖）
class UMyInventoryComponent;

#include "APickupBase.generated.h"

UCLASS()
class UNREALGAMEDEMO_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// 构造函数：用于创建组件并做一些初始设置
	APickupBase();

protected:
	// 游戏开始或生成时调用
	virtual void BeginPlay() override;

	// ================= 组件声明区域 =================
	// 球体碰撞组件：用于检测玩家是否“接触到”这个拾取物
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "pickkup|Components")
	USphereComponent* SphereComponent;

	// 静态网格组件：用于在场景中显示拾取物的 3D 模型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "pickkup|Components")
	UStaticMeshComponent* MeshComponent;

	// ================= 重叠事件函数声明 =================
	// 当其他物体与 SphereComponent 发生重叠时，会触发这个函数
	// 函数签名要与 OnComponentBeginOverlap 事件所要求的一致
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,	// 发生重叠的组件（这里就是我们的 SphereComponent）
		AActor* OtherActor,							// 与我们发生重叠的“另一个” Actor（可能是玩家，也可能是别的东西）
		UPrimitiveComponent* OtherComp,				// 对方身上的哪个组件与我们重叠
		int32 OtherBodyIndex,						// 物理体索引（一般用不到）
		bool bFromSweep,							// 是否是“扫掠”产生的重叠
		const FHitResult& SweepResult				// 如果是扫掠，会带上一些命中信息
	);

public:	
	// 每帧调用（如果启用了 Tick）
	virtual void Tick(float DeltaTime) override;

};

