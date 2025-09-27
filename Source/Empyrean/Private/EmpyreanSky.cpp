// Copyright (c) 2025 Pixane.

#include "EmpyreanSky.h"

#include <Components/DirectionalLightComponent.h>
#include <Components/ExponentialHeightFogComponent.h>
#include <Components/SkyLightComponent.h>
#include <Materials/MaterialParameterCollection.h>
#include <Materials/MaterialParameterCollectionInstance.h>

#if WITH_EDITORONLY_DATA
#include <Components/BillboardComponent.h>
#include <ImageUtils.h>
#endif // WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(EmpyreanSky)

#define LOCTEXT_NAMESPACE "Empyrean"

AEmpyreanSky::AEmpyreanSky()
{
	// Start with tick disabled and enable it if needed.
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.bAllowTickBatching = true;
	
	// This actor should always be loaded regardless of distance.
	bIsSpatiallyLoaded = false;

	// Prevent the actor from affecting level bounds.
	bRelevantForLevelBounds = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Create the Actor's root component.
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	RootComponent = SceneRootComponent;
	RootComponent->SetMobility(EComponentMobility::Static);

	// Create and define all the parameters for the Mesh Component.
	SkyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkyMeshComponent"));
	SkyMeshComponent->SetupAttachment(RootComponent);
	SkyMeshComponent->SetMobility(EComponentMobility::Static);
	SkyMeshComponent->SetGenerateOverlapEvents(false);
	SkyMeshComponent->SetNotifyRigidBodyCollision(false);
	SkyMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SkyMeshComponent->BodyInstance.bAutoWeld = false;
	SkyMeshComponent->SetCanEverAffectNavigation(false);
	SkyMeshComponent->CastShadow = false;
	SkyMeshComponent->bAffectDynamicIndirectLighting = false;
	SkyMeshComponent->bReceiveMobileCSMShadows = false;
	SkyMeshComponent->bReceivesDecals = false;
	SkyMeshComponent->bRenderInDepthPass = false;
	SkyMeshComponent->bTreatAsBackgroundForOcclusion = true;
	SkyMeshComponent->bUseAsOccluder = false;
	SkyMeshComponent->SetRelativeScale3D(FVector(100.0f, 100.0f, 100.0f));

	// Set path for the sky static mesh. This is hard-coded because it is meant to fixed and not user changeable.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> skyMeshFile(TEXT("/Empyrean/Meshes/SM_EmpyreanSky.SM_EmpyreanSky"));
	if (skyMeshFile.Succeeded())
	{
		// Assign the mesh to the static mesh reference.
		SkyMeshComponent->SetStaticMesh(skyMeshFile.Object);
	}

	// Setup the Sun's directional light component.
	SunLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLightComponent"));
	SunLightComponent->SetupAttachment(RootComponent);
	SunLightComponent->SetMobility(EComponentMobility::Stationary);
	SunLightComponent->SetUseTemperature(true);

	// Setup the Sky light component.
	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLightComponent"));
	SkyLightComponent->SetupAttachment(RootComponent);
	SkyLightComponent->SetMobility(EComponentMobility::Static);

	// Setup Height Fog component.
	HeightFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFogComponent"));
	HeightFogComponent->SetupAttachment(RootComponent);
	HeightFogComponent->SetMobility(EComponentMobility::Static);
	HeightFogComponent->SetFogCutoffDistance(150000.0f);

	// Initialize member variables.
	HorizonStrenght = 0.6;
	SunRadius = 0.2;
	SunBrightness = 0.3;
	SkyMaterialInstance = nullptr;
	SkyMaterialCollectionInstance = nullptr;

#if WITH_EDITORONLY_DATA
	// Create Editor sprite.
	EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SkySpriteComponent"));
	if (!IsRunningCommandlet() && EditorSprite)
	{
		EditorSprite->SetupAttachment(RootComponent);
		EditorSprite->SetRelativeScale3D(FVector(0.5f));
		UTexture2D* skySpriteTexture = FImageUtils::ImportFileAsTexture2D(FPaths::ProjectPluginsDir() + "Empyrean/Resources/SkyThumbnail.png");
		if (skySpriteTexture)
		{
			skySpriteTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
			skySpriteTexture->bUseLegacyGamma = true;
			skySpriteTexture->LODGroup = TextureGroup::TEXTUREGROUP_World;
			skySpriteTexture->UpdateResource();
			EditorSprite->Sprite = skySpriteTexture;
		}
		EditorSprite->SetMobility(EComponentMobility::Static);
		EditorSprite->SpriteInfo.Category = TEXT("EmpyreanSky");
		EditorSprite->SpriteInfo.DisplayName = LOCTEXT("EmpyreanSky", "EmpyreanSky");
		EditorSprite->SetUsingAbsoluteScale(true);
		EditorSprite->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA
}

void AEmpyreanSky::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITORONLY_DATA
	// Hide the sprites for all component to only show the Empyrean sprite.
	TArray<USceneComponent*> childComponents;
	TArray<USceneComponent*> sunChildComponents;
	SunLightComponent->GetChildrenComponents(true, sunChildComponents);
	childComponents.Append(sunChildComponents);
	TArray<USceneComponent*> skyChildComponents;
	SkyLightComponent->GetChildrenComponents(true, skyChildComponents);
	childComponents.Append(skyChildComponents);
	TArray<USceneComponent*> fogChildComponents;
	HeightFogComponent->GetChildrenComponents(true, fogChildComponents);
	childComponents.Append(fogChildComponents);

	for (USceneComponent*& component : childComponents)
	{
		component->SetVisibility(false);
	}
#endif // WITH_EDITORONLY_DATA

	if (!SkyMaterialCollectionInstance)
	{
		SkyMaterialCollectionInstance = GetWorld()->GetParameterCollectionInstance(LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Empyrean/Materials/MPC_EmpyreanSky.MPC_EmpyreanSky")));
	}

	if (!SkyMaterialInstance)
	{
		SkyMaterialInstance = UMaterialInstanceDynamic::Create(LoadObject<UMaterial>(nullptr, TEXT("/Empyrean/Materials/M_EmpyreanSky.M_EmpyreanSky")), this);
		SkyMeshComponent->SetMaterial(0, SkyMaterialInstance);
	}

	UpdateMaterial();
}

void AEmpyreanSky::UpdateMaterial()
{
	UpdateSkyMaterial();
	UpdateSunMaterial();
}

void AEmpyreanSky::UpdateSkyMaterial()
{
	if (SkyMaterialInstance)
	{
		// Update the Sky parameters on the Material Instance.
		SkyMaterialInstance->SetScalarParameterValue(FName("HorizonStrenght"), HorizonStrenght);
	}
}

void AEmpyreanSky::UpdateSunMaterial()
{
	if (SkyMaterialInstance)
	{
		// Update the Sun parameters on the Material Instance.
		SkyMaterialInstance->SetVectorParameterValue(FName("SunLightColor"), SunLightComponent->GetLightColor());
		SkyMaterialInstance->SetVectorParameterValue(FName("SunLightDirection"), SunLightComponent->GetForwardVector());
		SkyMaterialInstance->SetScalarParameterValue(FName("SunRadius"), SunRadius / 10000.0f);
		SkyMaterialInstance->SetScalarParameterValue(FName("SunBrightness"), SunBrightness / 100.0f);
	}

	if (SkyMaterialCollectionInstance)
	{
		// Update the Sun parameters on the Material Parameter Collection.
		SkyMaterialCollectionInstance->SetVectorParameterValue(FName("SunLightColor"), SunLightComponent->GetLightColor());
		SkyMaterialCollectionInstance->SetVectorParameterValue(FName("SunLightDirection"), SunLightComponent->GetForwardVector());
	}
}

#undef LOCTEXT_NAMESPACE
