// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyInventoryComponent.generated.h"

// ===================== 背包更新事件委托定义 =====================
// DECLARE_DYNAMIC_MULTICAST_DELEGATE 声明一个“动态多播委托”（可在蓝图中绑定多个监听者）。
// 不需要任何参数，所以使用最基础的版本。
// 命名为 FOnInventoryUpdated，表示定义的事件类型。
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

// ===================== 物品数据结构定义 =====================
// USTRUCT(BlueprintType) 表示：
// - 这是一个 UStruct，可以被反射系统识别（比如保存、网络同步等）。
// - BlueprintType 让这个结构体可以在蓝图中被使用（变量类型、函数参数等）。
USTRUCT(BlueprintType)
struct FItemData
{
	// GENERATED_BODY() 是所有 USTRUCT 必须的宏，
	// 用来生成反射系统所需的样板代码（类似 UCLASS 里的 GENERATED_BODY）。
	GENERATED_BODY()

public:
	// ItemID：
	// - 使用 FName 作为物品 ID，查找和比较效率更高。- FName 是 Unreal 中常用的轻量级字符串类型，适合做标识符。
	// - EditAnywhere：可以在编辑器的任何地方编辑（例如放在关卡里的组件实例上）。
	// - BlueprintReadWrite：蓝图中可以读写这个字段。
	// - Category = "Inventory"：在详情面板和蓝图中归类到 Inventory 分类下。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemID = NAME_None;

	// Quantity：
	// - 记录该物品的数量。
	// - 同样通过 EditAnywhere + BlueprintReadWrite 暴露给编辑器和蓝图。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 0;
};
// ===================== 物品数据结构定义结束 =====================


// UMyInventoryComponent：负责管理背包数据的组件。
// 可以挂在任意 Actor 上，让该 Actor 拥有一个简单的背包系统。
// - ClassGroup=(Custom)：在编辑器的组件列表中归类到 Custom 组下，方便查找。
//- BlueprintSpawnableComponent：允许在蓝图中直接添加这个组件（例如在角色蓝图里添加一个 InventoryComponent）。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class UNREALGAMEDEMO_API UMyInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// 构造函数：设置该组件的一些默认属性（比如 Tick 开关等）。
	UMyInventoryComponent();

protected:
	// BeginPlay：游戏开始时（或组件被激活时）调用一次。
	virtual void BeginPlay() override;

public:	
	// TickComponent：每帧调用（如果 bCanEverTick 为 true）。
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ===================== 背包数据与测试函数 =====================

	// InventoryItems：
	// - 使用 TArray<FItemData> 存储背包中的所有物品。
	// - EditAnywhere：可以在编辑器详情面板中直接添加测试物品。
	// - BlueprintReadWrite：在蓝图逻辑中可以读取和修改数组内容（比如添加/删除物品）。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemData> InventoryItems;

	// AddTestItem：
	// - 用于测试背包功能的简单函数。
	// - 调用时会往 InventoryItems 中添加一个名为 "HealthPotion" 数量为 5 的物品。
	// - 并在输出日志中打印当前数组的长度，方便在 Output Log 中观察。
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddTestItem();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FName ItemID, int32 Quantity);

	// ConsumeItem：尝试消耗指定 ID 的物品，成功返回 true，失败返回 false。
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(FName ItemID);
	
	// BlueprintPure 表示这个函数是纯函数（不修改任何状态），在蓝图中调用时会被优化成直接访问数据而不是执行节点。
	//const 表示这个函数不会修改任何成员变量，可以在 const 对象上调用。
	UFUNCTION(BlueprintPure)
	int32 GetItemQuantity(FName ItemID) const;

	// ===================== 背包更新事件（蓝图可绑定） =====================
	// - 使用我们上面声明的 FOnInventoryUpdated 动态多播委托类型。
	//BlueprintAssignable 允许我们在蓝图里使用 "Bind Event to..." 节点来监听这个大喇叭。
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
	void SaveInventory(); // 将当前背包写入存档
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Persistence")
	void LoadInventory(); // 从存档读取背包并同步到当前组件
};
