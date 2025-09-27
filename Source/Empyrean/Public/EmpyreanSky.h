// Copyright (c) 2025 Pixane.

#pragma once

#include <GameFramework/Actor.h>

#include "EmpyreanSky.generated.h"

class UDirectionalLightComponent;
class UExponentialHeightFogComponent;

/*
 * Actor class the holds all the needed components for a sky representation.
 */
UCLASS(NotBlueprintable, ClassGroup = Empyrean, MinimalAPI, HideCategories = (Actor, Collision, DataLayers, Input, LOD, HLOD, Networking, Physics, WorldPartition))
class AEmpyreanSky : public AActor
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties. */
	AEmpyreanSky();

	//~ Begin of AActor implementation.
	void OnConstruction(const FTransform& Transform) override;
	//~ End of AActor implementation.

protected:
	/** Directional Light component that represents the Sun of this Sky. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Empyrean")
	TObjectPtr<UDirectionalLightComponent> SunLightComponent;

	/** Sky Light component of this Sky. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Empyrean")
	TObjectPtr<USkyLightComponent> SkyLightComponent;

	/** Height Fog of this Sky. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Empyrean")
	TObjectPtr<UExponentialHeightFogComponent> HeightFogComponent;

	/** Radius of the Sun Disk. */
	UPROPERTY(EditInstanceOnly, Category = "Empyrean|Sky", Meta = (ClampMin = 0, UIMin = 0))
	float HorizonStrenght;

	/** Radius of the Sun Disk. */
	UPROPERTY(EditInstanceOnly, Category = "Empyrean|Sun", Meta = (ClampMin = 0.1, UIMin = 0.1))
	float SunRadius;

	/** Brightness of the Sun Disk. */
	UPROPERTY(EditInstanceOnly, Category = "Empyrean|Sun", Meta = (ClampMin = 0, UIMin = 0))
	float SunBrightness;

	/** Scene Component to hold all the other components. */
	TObjectPtr<USceneComponent> SceneRootComponent;

	/** Static Mesh Component to display the SkyMesh's mesh. */
	TObjectPtr<UStaticMeshComponent> SkyMeshComponent;

	/** Update the Sky material. */
	void UpdateMaterial();

	/** Update the Sky material with the Sky runtime properties. */
	void UpdateSkyMaterial();

	/** Update the Sky material with the Sun light runtime properties. */
	void UpdateSunMaterial();

private:
	/** Dynamic Material instance to update the Sky material in runtime. */
	TObjectPtr<UMaterialInstanceDynamic> SkyMaterialInstance;

	/**
	 * DynamicMaterialParameterCollection with the Sky runtime parameters.
	 * Allows other materials to read from the Sky data. 
	 */
	TObjectPtr<UMaterialParameterCollectionInstance> SkyMaterialCollectionInstance;

#if WITH_EDITORONLY_DATA
	/** Sprite to show the Actor's sprite in Editor. */
	TObjectPtr<UBillboardComponent> EditorSprite;
#endif // WITH_EDITORONLY_DATA
};
