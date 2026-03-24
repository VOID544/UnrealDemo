// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h" // 确保声明 UUserWidget
#include "MyPlayerController.generated.h"

// 前向声明：避免在头文件中包含太多实现细节头文件，保持编译速度
class UInputMappingContext;
class UInputAction;

/**
 * AMyPlayerController
 * 这个类实现了按 'I' 键切换背包 UI 的功能：
 * - 使用软引用（TSoftObjectPtr）让编辑器里可以指定 Input Mapping Context 和 Input Action（无需在编译时强制加载）
 * - 使用 TSubclassOf<UUserWidget> 在蓝图中指定背包 UI 类
 * - 运行时通过 CreateWidget 创建实例并通过 SetVisibility 显示/隐藏
 * - 切换鼠标显示与输入模式（GameOnly <-> GameAndUI）
 */
UCLASS()
class UNREALGAMEDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	// 软引用：Input Mapping Context（Input Action绑定目录）
	// 说明：使用 TSoftObjectPtr（软引用）可以在运行时按需加载对象，避免启动时强制加载资源。
	// 编辑时在 Details 面板里指向你的 Enhanced Input Mapping Context 资源。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 软引用：Input Action（用于切换背包）
	// 说明：在编辑器中指定对应的 InputAction（例如绑定到键盘 I 的按下事件）。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> ToggleInventoryAction;

	// 在蓝图中指定的背包 UI 类
	// 说明：在蓝图/编辑器里选一个 Widget Blueprint（继承自 UUserWidget）。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	// 运行时创建的背包 UI 实例
	// 说明：运行时通过 CreateWidget 创建并保存到这个指针中，避免重复创建。
	UPROPERTY()
	UUserWidget* InventoryWidgetInstance = nullptr;

	// 切换背包显示/隐藏
	// 说明：被绑定到 Enhanced Input 的 ToggleInventoryAction，当按键触发时调用。
	UFUNCTION()
	void ToggleInventory();
};
