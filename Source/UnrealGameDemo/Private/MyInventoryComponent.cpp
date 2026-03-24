#include "UnrealGameDemo/Public/MyInventoryComponent.h"

UMyInventoryComponent::UMyInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UMyInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	AddTestItem();
	// ...
	
}

void UMyInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMyInventoryComponent::AddTestItem()
{
	// Step 1：创建一个 FItemData 变量，用于存放要添加的测试物品数据。
	FItemData NewItem;

	// Step 2：设置物品的 ID。
	// TEXT("HealthPotion") 会创建一个宽字符文本，然后自动转换为 FName。
	// 这就是我们要往背包中添加的“生命药水”物品的标识。
	NewItem.ItemID = TEXT("生命药水");

	// Step 3：设置该物品的数量为 5。
	NewItem.Quantity = 1;

	// Step 4：把这个物品加入到背包数组 InventoryItems 中。
	// TArray::Add 会在数组末尾插入一个元素。
	InventoryItems.Add(NewItem);

	// Step 5：获取当前数组中有多少个元素，并打印到输出日志中。
	// InventoryItems.Num() 返回数组长度（元素个数）。
	const int32 ItemCount = InventoryItems.Num();

	// UE_LOG 用于输出日志信息到 Unreal 的 Output Log。
	// LogTemp：临时日志分类，简单测试用足够。
	// Log：日志级别（还有 Warning、Error 等）。
	// TEXT 中的 %d 是格式占位符，对应后面的 ItemCount 整数
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(
			-1, 
			5.f, 
			FColor::Green, 
			FString::Printf(TEXT("Inventory now holds %d item."), ItemCount));
	//广播背包数据的变化
	OnInventoryUpdated.Broadcast();
}

void UMyInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	//输入验证：如果物品 ID 为空或者数量不合法（小于等于 0），则直接返回，不执行添加操作。
	if (ItemID.IsNone() || Quantity <= 0) {return;}

	//遍历背包数组，查找是否已经有相同 ID 的物品（使用引用以便直接修改数量）
	for (FItemData &i : InventoryItems)
	{
		if (i.ItemID == ItemID)
		{
			i.Quantity += Quantity;
			OnInventoryUpdated.Broadcast();
			return;
		}
	}
	
	//声明新的物品数据结构并赋值
	FItemData NewItem;
	NewItem.ItemID = ItemID;
	NewItem.Quantity = Quantity;
	//将新的物品添加到背包数组中
	InventoryItems.Add(NewItem);
	//广播背包数据的变化事件
	OnInventoryUpdated.Broadcast();
}

bool UMyInventoryComponent::ConsumeItem(FName ItemID)
{
	// 防御：若传入空 ID，则无需继续
	if (ItemID.IsNone())
	{
		return false;
	}

	for (int32 i = 0; i < InventoryItems.Num(); ++i)
	{
		FItemData& Item = InventoryItems[i];

		if (Item.ItemID == ItemID)
		{
			if (Item.Quantity <= 0)
			{
				// 没货了，直接把这个槽位删掉（可选）
				InventoryItems.RemoveAt(i);
				OnInventoryUpdated.Broadcast();
				return false;
			}

			// 正常消耗
			Item.Quantity -= 1;

			// 数量为 0 就移除
			if (Item.Quantity == 0)
			{
				InventoryItems.RemoveAt(i);
			}

			OnInventoryUpdated.Broadcast();
			return true;
		}
	}
	
	// 未找到该物品
	return false;
}

int32 UMyInventoryComponent::GetItemQuantity(FName ItemID) const
{
	for (const FItemData& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return Item.Quantity;
		}
	}
	
	return 0; // 没有这个物品，数量为 0
}
