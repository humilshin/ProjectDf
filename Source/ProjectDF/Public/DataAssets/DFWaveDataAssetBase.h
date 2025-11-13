#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/DFWaveStructs.h"
#include "DFWaveDataAssetBase.generated.h"
// ✅ 하나의 DataAsset에 여러 웨이브를 저장
UCLASS(BlueprintType)
class PROJECTDF_API UDFWaveDataAssetBase : public UDataAsset
{
    GENERATED_BODY()

public:
    // 이 웨이브 세트의 이름 (예: Stage1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave Set")
    FName WaveSetName;

    // 전체 웨이브 리스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave Set")
    TArray<FWaveInfo> Waves;
};