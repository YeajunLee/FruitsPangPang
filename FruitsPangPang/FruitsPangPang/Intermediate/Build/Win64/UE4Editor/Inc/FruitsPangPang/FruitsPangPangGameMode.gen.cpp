// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "FruitsPangPang/FruitsPangPangGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFruitsPangPangGameMode() {}
// Cross Module References
	FRUITSPANGPANG_API UClass* Z_Construct_UClass_AFruitsPangPangGameMode_NoRegister();
	FRUITSPANGPANG_API UClass* Z_Construct_UClass_AFruitsPangPangGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_FruitsPangPang();
// End Cross Module References
	void AFruitsPangPangGameMode::StaticRegisterNativesAFruitsPangPangGameMode()
	{
	}
	UClass* Z_Construct_UClass_AFruitsPangPangGameMode_NoRegister()
	{
		return AFruitsPangPangGameMode::StaticClass();
	}
	struct Z_Construct_UClass_AFruitsPangPangGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AFruitsPangPangGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_FruitsPangPang,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AFruitsPangPangGameMode_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "FruitsPangPangGameMode.h" },
		{ "ModuleRelativePath", "FruitsPangPangGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AFruitsPangPangGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AFruitsPangPangGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AFruitsPangPangGameMode_Statics::ClassParams = {
		&AFruitsPangPangGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x008802ACu,
		METADATA_PARAMS(Z_Construct_UClass_AFruitsPangPangGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AFruitsPangPangGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AFruitsPangPangGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AFruitsPangPangGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AFruitsPangPangGameMode, 3662510375);
	template<> FRUITSPANGPANG_API UClass* StaticClass<AFruitsPangPangGameMode>()
	{
		return AFruitsPangPangGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AFruitsPangPangGameMode(Z_Construct_UClass_AFruitsPangPangGameMode, &AFruitsPangPangGameMode::StaticClass, TEXT("/Script/FruitsPangPang"), TEXT("AFruitsPangPangGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AFruitsPangPangGameMode);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
