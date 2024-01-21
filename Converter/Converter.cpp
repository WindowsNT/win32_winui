// Converter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include <conio.h>
#include "xml3all.h"


bool Unpackaged = true;
bool SelfContained = true;
std::string RootName;

std::wstring GetRootDir(const wchar_t* vcxproj)
{
	wchar_t bu[1000] = {};
	wchar_t* fp = 0;
	GetFullPathName(vcxproj, 1000, bu, &fp);
	if (!fp)
		return {};
	*fp = 0;
	return bu;
}

template <typename T = char>
bool PutFile(const wchar_t* f, std::vector<T>& d, bool Fw = false)
{
	HANDLE hX = CreateFile(f, GENERIC_WRITE, 0, 0, Fw ? CREATE_ALWAYS : CREATE_NEW, 0, 0);
	if (hX == INVALID_HANDLE_VALUE)
		return false;
	DWORD A = 0;
	WriteFile(hX, d.data(), (DWORD)(d.size() * sizeof(T)), &A, 0);
	CloseHandle(hX);
	if (A != d.size())
		return false;
	return true;
}

bool ChangePchH(const wchar_t* vcxproj)
{
	auto bu = GetRootDir(vcxproj);
	bu += L"\\pch.h";

	auto hX = CreateFile(bu.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	if (hX == INVALID_HANDLE_VALUE)
		return 0;

	LARGE_INTEGER li;
	GetFileSizeEx(hX, &li);
	std::vector<char> x(li.QuadPart + 1);
	DWORD a = 0;
	ReadFile(hX, x.data(), (DWORD)li.QuadPart, &a, 0);
	if (a != li.QuadPart)
	{
		CloseHandle(hX);
		return false;
	}
	if (strstr(x.data(), "#ifdef Win32_WINUI3"))
	{
		CloseHandle(hX);
		return true;
	}

	const char* a1 = R"(#ifdef Win32_WINUI3
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <microsoft.ui.xaml.window.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <wil/cppwinrt_helpers.h>
#include <appmodel.h>
#endif
)";
	WriteFile(hX, a1, (DWORD)strlen(a1), &a, 0);
	CloseHandle(hX);
	return true;
}



//CppWinRT 
const char* p1v = "2.0.240111.5";
// WIL
const char* p2v = "1.0.231216.1";
// SDK Build Tools
const char* p3v = "10.0.22621.2428";
// AppSdk
const char* p4v = "1.4.231219000";

bool ChangePackages(const wchar_t* vcxproj)
{
	auto bu = GetRootDir(vcxproj);
	bu += L"\\packages.config";


	XML3::XML x(bu.c_str());
	x.GetRootElement().SetElementName("packages");
	bool f1 = 0, f2 = 0, f3 = 0, f4 = 0;
	for (auto& e : x.GetRootElement())
	{
		if (e.vv("id").GetValue() == "Microsoft.Windows.CppWinRT")
		{
			f1 = 1;
			e.vv("version").SetValue(p1v);
			e.vv("targetFramework").SetValue("native");
		}
		if (e.vv("id").GetValue() == "Microsoft.Windows.ImplementationLibrary")
		{
			f2 = 1;
			e.vv("version").SetValue(p2v);
			e.vv("targetFramework").SetValue("native");
		}
		if (e.vv("id").GetValue() == "Microsoft.Windows.SDK.BuildTools")
		{
			f3 = 1;
			e.vv("version").SetValue(p3v);
			e.vv("targetFramework").SetValue("native");
		}
		if (e.vv("id").GetValue() == "Microsoft.WindowsAppSDK")
		{
			f4 = 1;
			e.vv("version").SetValue(p4v);
			e.vv("targetFramework").SetValue("native");
		}
	}
	if (!f1)
	{
		auto& e = x.GetRootElement().AddElement("package");
		e.vv("id").SetValue("Microsoft.Windows.CppWinRT");
		e.vv("version").SetValue(p1v);
		e.vv("targetFramework").SetValue("native");
	}
	if (!f2)
	{
		auto& e = x.GetRootElement().AddElement("package");
		e.vv("id").SetValue("Microsoft.Windows.ImplementationLibrary");
		e.vv("version").SetValue(p2v);
		e.vv("targetFramework").SetValue("native");
	}
	if (!f3)
	{
		auto& e = x.GetRootElement().AddElement("package");
		e.vv("id").SetValue("Microsoft.Windows.SDK.BuildTools");
		e.vv("version").SetValue(p3v);
		e.vv("targetFramework").SetValue("native");
	}
	if (!f4)
	{
		auto& e = x.GetRootElement().AddElement("package");
		e.vv("id").SetValue("Microsoft.WindowsAppSDK");
		e.vv("version").SetValue(p4v);
		e.vv("targetFramework").SetValue("native");
	}


	x.Save();
	return 1;
}


bool CreateManifest(const wchar_t* vcxproj)
{
	auto bu = GetRootDir(vcxproj);
	bu += L"\\Package.appxmanifest";

	const char* r1 = R"(<Package
  xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
  xmlns:mp="http://schemas.microsoft.com/appx/2014/phone/manifest"
  xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
  xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities"
  IgnorableNamespaces="uap rescap">

  <Identity
    Name="fdcbfca1-94a6-403d-b01e-6755fd14e70a"
    Publisher="CN=yo"
    Version="1.0.0.0" />

  <mp:PhoneIdentity PhoneProductId="fdcbfca1-94a6-403d-b01e-6755fd14e70a" PhonePublisherId="00000000-0000-0000-0000-000000000000"/>

  <Properties>
    <DisplayName>App</DisplayName>
    <PublisherDisplayName>yo</PublisherDisplayName>
    <Logo>Assets\StoreLogo.png</Logo>
  </Properties>

  <Dependencies>
    <TargetDeviceFamily Name="Windows.Universal" MinVersion="10.0.17763.0" MaxVersionTested="10.0.19041.0" />
    <TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.17763.0" MaxVersionTested="10.0.19041.0" />
  </Dependencies>

  <Resources>
    <Resource Language="x-generate"/>
  </Resources>

  <Applications>
    <Application Id="App"
      Executable="$targetnametoken$.exe"
      EntryPoint="$targetentrypoint$">
      <uap:VisualElements
        DisplayName="App"
        Description="App"
        BackgroundColor="transparent"
        Square150x150Logo="Assets\Square150x150Logo.png"
        Square44x44Logo="Assets\Square44x44Logo.png">
        <uap:DefaultTile Wide310x150Logo="Assets\Wide310x150Logo.png" />
        <uap:SplashScreen Image="Assets\SplashScreen.png" />
      </uap:VisualElements>
    </Application>
  </Applications>

  <Capabilities>
    <rescap:Capability Name="runFullTrust" />
  </Capabilities>
</Package>
)";


	XML3::XMLElement e = r1;
	XML3::XML x;
	x.SetRootElement(e);

	XML3::XMLSerialization xs;
	xs.NoCRLF = 1;
	x.Save(bu.c_str(),&xs);
	return true;
}


bool CreateAppXaml(const wchar_t* vcxproj)
{
	auto bu = GetRootDir(vcxproj);
	bu += L"\\App.xaml";

	const char* r1 = R"(<?xml version="1.0" encoding="utf-8"?>
<Application
    x:Class="App2.App"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:local="using:App2">
    <Application.Resources>
        <ResourceDictionary>
            <ResourceDictionary.MergedDictionaries>
                <XamlControlsResources xmlns="using:Microsoft.UI.Xaml.Controls" />
                <!-- Other merged dictionaries here -->
            </ResourceDictionary.MergedDictionaries>
            <!-- Other app resources here -->
        </ResourceDictionary>
    </Application.Resources>
</Application>
)";

	XML3::XMLElement e = r1;
	XML3::XML x;
	e.vv("x:Class").SetValue(std::string(RootName + ".App"));
	e.vv("xmlns:local").SetValue(std::string("using:" + RootName));
	x.SetRootElement(e);
	

	XML3::XMLSerialization xs;
	xs.NoCRLF = 1;
	x.Save(bu.c_str(), &xs);


	const char* r2 = R"(#pragma once

#include "App.xaml.g.h"

namespace winrt::%s::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
)";

	std::vector<char> d(100000);
	sprintf_s(d.data(), 100000, r2, RootName.c_str());
	auto bu2 = GetRootDir(vcxproj);
	bu2 += L"\\App.xaml.h";
	d.resize(strlen(d.data()));
	PutFile(bu2.c_str(), d,false);


	const char* r3 = R"(#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::%s::implementation
{
    App::App()
    {
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="e">Details about the launch request and process.</param>
    void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();
        window.Activate();
    }
}
)";

	d.resize(100000);
	sprintf_s(d.data(), 100000, r3, RootName.c_str());
	auto bu3 = GetRootDir(vcxproj);
	bu3 += L"\\App.xaml.cpp";
	d.resize(strlen(d.data()));
	PutFile(bu3.c_str(), d, false);
	return true;
}

int wmain(int argc,wchar_t** args)
{
	if (argc != 3)
	{
		std::wcout << L"Usage: converter input.vcxproj output.vcxproj" << std::endl;
		return 0;
	}

	// Load
	std::wcout << L"Loading: " << args[1] << std::endl;
	XML3::XML x(args[1]);
	auto& root = x.GetRootElement();


	root.vv("ToolsVersion").SetValue("15.0");

	// Find pch.h
	std::wcout << L"Updating pch.h...\r\n";
	ChangePchH(args[1]);

	// Packages
	std::wcout << L"Updating Nuget Packages...\r\n";
	ChangePackages(args[1]);

	// AppxManifest
	std::wcout << L"Creating AppxManifest...\r\n";
	CreateManifest(args[1]);

	// Collect all
	std::vector<std::shared_ptr<XML3::XMLElement>> alls;
	x.GetRootElement().GetAllChildren(alls);

	// Loop all
	std::wcout << L"Changing Intermediate Directory...\r\n";
	for (auto& e : alls)
	{
		// Add the preprocessor directive Win32_WINUI3
		if (e->GetElementName() == "PreprocessorDefinitions")
			e->SetContent(std::string("Win32_WINUI3;" + e->GetContent()).c_str());

		// Change the intermediate directory
		if (e->GetElementName() == "IntDir")
			e->SetContent(std::string(e->GetContent() + "WinUI3\\").c_str());
	}




	// Find the globals
	for (auto& e : x.GetRootElement())
	{
		if (e.GetElementName() == "PropertyGroup")
		{
			if (e.vv("Label").GetValue() == "Globals")
			{
				// Change Guid
				std::wcout << L"Updating Project Globals...\r\n";
				CLSID g = {};
				CoCreateGuid(&g);
				wchar_t gs[100] = {};
				StringFromGUID2(g, gs, 100);
				e.FindElementZ("ProjectGuid",true)->SetContent(XML3::XMLU(gs));

				// Root Namespace
				e.FindElementZ("RootNamespace", true)->SetContent(std::string(e.FindElementZ("RootNamespace", true)->GetContent() + "WinUI3").c_str());
				RootName = e.FindElementZ("RootNamespace", true)->GetContent();
				e.FindElementZ("ProjectName", true)->SetContent(RootName.c_str());

				// Add stuff
				e.FindElementZ("CppWinRTOptimized", true)->SetContent("true");
				e.FindElementZ("CppWinRTRootNamespaceAutoMerge", true)->SetContent("true");
				e.FindElementZ("MinimalCoreWin", true)->SetContent("true");
				e.FindElementZ("TargetName", true)->SetContent("$(RootNamespace)");
				e.FindElementZ("DefaultLanguage", true)->SetContent("en-US");
				e.FindElementZ("MinimumVisualStudioVersion", true)->SetContent("16");
				e.FindElementZ("AppContainerApplication", true)->SetContent("false");
				if (Unpackaged)	
					e.FindElementZ("AppxPackage", true)->SetContent("false");
				else
					e.FindElementZ("AppxPackage", true)->SetContent("true");
				e.FindElementZ("UseWinUI", true)->SetContent("true");
				e.FindElementZ("EnableMsixTooling", true)->SetContent("true");
				if (SelfContained)
					e.FindElementZ("WindowsAppSDKSelfContained", true)->SetContent("true");
				else
					e.FindElementZ("WindowsAppSDKSelfContained", true)->SetContent("false");
				e.FindElementZ("ApplicationType", true)->SetContent("Windows Store");
				e.FindElementZ("ApplicationTypeRevision", true)->SetContent("10.0");
				e.FindElementZ("WindowsTargetPlatformVersion", true)->SetContent("10.0");
				e.FindElementZ("WindowsTargetPlatformMinVersion",true)->SetContent("10.0.17763.0");
				if (Unpackaged)
					e.FindElementZ("WindowsPackageType", true)->SetContent("None");
				else
					e.FindElementZ("WindowsPackageType", true)->SetContent("MSIX");

			}
		}

	

	}

	if (1)
	{
		// Imports
		if (1)
		{
			std::wcout << L"Adding Nuget Imports to top...\r\n";
			/*
			<Import Project="packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.props" Condition="Exists('packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.props')" />
			<Import Project="packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.props" Condition="Exists('packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.props')" />
			<Import Project="packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.props" Condition="Exists('packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.props')" />
		    */
			XML3::XMLElement i1;
			i1.SetElementName("Import");
			i1.vv("Project").SetValue(std::string("packages\\Microsoft.Windows.SDK.BuildTools.") + std::string(p3v) + std::string("\\build\\Microsoft.Windows.SDK.BuildTools.props"));
			i1.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.Windows.SDK.BuildTools.") + std::string(p3v) + std::string("\\build\\Microsoft.Windows.SDK.BuildTools.props')"));
			root.InsertElement(0, i1);

			XML3::XMLElement i2;
			i2.SetElementName("Import");
			i2.vv("Project").SetValue(std::string("packages\\Microsoft.Windows.CppWinRT.") + std::string(p1v) + std::string("\\build\\native\\Microsoft.Windows.CppWinRT.props"));
			i2.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.Windows.CppWinRT.") + std::string(p1v) + std::string("\\build\\native\\Microsoft.Windows.CppWinRT.props')"));
			root.InsertElement(0, i2);

			XML3::XMLElement i3;
			i3.SetElementName("Import");
			i3.vv("Project").SetValue(std::string("packages\\Microsoft.WindowsAppSDK.") + std::string(p4v) + std::string("\\build\\native\\Microsoft.WindowsAppSDK.props"));
			i3.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.WindowsAppSDK.") + std::string(p4v) + std::string("\\build\\native\\Microsoft.WindowsAppSDK.props')"));
			root.InsertElement(0, i3);

		}

		// Imports
		if (1)
		{
			/*
<ImportGroup Label="ExtensionTargets">
	<Import Project="packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.targets" Condition="Exists('packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.targets')" />
	<Import Project="packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.targets" Condition="Exists('packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.targets')" />
	<Import Project="packages\Microsoft.Windows.ImplementationLibrary.1.0.231216.1\build\native\Microsoft.Windows.ImplementationLibrary.targets" Condition="Exists('packages\Microsoft.Windows.ImplementationLibrary.1.0.231216.1\build\native\Microsoft.Windows.ImplementationLibrary.targets')" />
	<Import Project="packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.targets" Condition="Exists('packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.targets')" />
  </ImportGroup>
 
 */
			std::wcout << L"Updating Nuget Targets...\r\n";
			auto& root2 = root.AddElement("ImportGroup");
			root2.vv("Label").SetValue("ExtensionTargets");

			auto& i3 = root2.AddElement("Import");
			i3.vv("Project").SetValue(std::string("packages\\Microsoft.WindowsAppSDK.") + std::string(p4v) + std::string("\\build\\native\\Microsoft.WindowsAppSDK.targets"));
			i3.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.WindowsAppSDK.") + std::string(p4v) + std::string("\\build\\native\\Microsoft.WindowsAppSDK.targets')"));

			auto& i2 = root2.AddElement("Import");
			i2.vv("Project").SetValue(std::string("packages\\Microsoft.Windows.CppWinRT.") + std::string(p1v) + std::string("\\build\\native\\Microsoft.Windows.CppWinRT.targets"));
			i2.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.Windows.CppWinRT.") + std::string(p1v) + std::string("\\build\\native\\Microsoft.Windows.CppWinRT.targets')"));

			auto& i1 = root2.AddElement("Import");
			i1.vv("Project").SetValue(std::string("packages\\Microsoft.Windows.SDK.BuildTools.") + std::string(p3v) + std::string("\\build\\Microsoft.Windows.SDK.BuildTools.targets"));
			i1.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.Windows.SDK.BuildTools.") + std::string(p3v) + std::string("\\build\\Microsoft.Windows.SDK.BuildTools.targets')"));

			auto& i4 = root2.AddElement("Import");
			i4.vv("Project").SetValue(std::string("packages\\Microsoft.Windows.ImplementationLibrary.") + std::string(p2v) + std::string("\\build\\native\\Microsoft.Windows.ImplementationLibrary.targets"));
			i4.vv("Condition").SetValue(std::string("Exists('packages\\Microsoft.Windows.ImplementationLibrary.") + std::string(p2v) + std::string("\\build\\native\\Microsoft.Windows.ImplementationLibrary.targets')"));


		}

		// Imports
		if (1)
		{
			/*
	<Target Name="EnsureNuGetPackageBuildImports" BeforeTargets="PrepareForBuild">
	<PropertyGroup>
	  <ErrorText>This project references NuGet package(s) that are missing on this computer. Use NuGet Package Restore to download them.  For more information, see http://go.microsoft.com/fwlink/?LinkID=322105. The missing file is {0}.</ErrorText>
	</PropertyGroup>
	<Error Condition="!Exists('packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.props')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.props'))" />
	<Error Condition="!Exists('packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.targets')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.WindowsAppSDK.1.4.231219000\build\native\Microsoft.WindowsAppSDK.targets'))" />
	<Error Condition="!Exists('packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.props')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.props'))" />
	<Error Condition="!Exists('packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.targets')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.Windows.CppWinRT.2.0.240111.5\build\native\Microsoft.Windows.CppWinRT.targets'))" />
	<Error Condition="!Exists('packages\Microsoft.Windows.ImplementationLibrary.1.0.231216.1\build\native\Microsoft.Windows.ImplementationLibrary.targets')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.Windows.ImplementationLibrary.1.0.231216.1\build\native\Microsoft.Windows.ImplementationLibrary.targets'))" />
	<Error Condition="!Exists('packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.props')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.props'))" />
	<Error Condition="!Exists('packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.targets')" Text="$([System.String]::Format('$(ErrorText)', 'packages\Microsoft.Windows.SDK.BuildTools.10.0.22621.2428\build\Microsoft.Windows.SDK.BuildTools.targets'))" />
  </Target>
			*/

			std::wcout << L"Updating Nuget PreBuilds...\r\n";
			auto& target = root.AddElement("Target");
			target.vv("Name").SetValue("EnsureNuGetPackageBuildImports");
			target.vv("BeforeTargets").SetValue("PrepareForBuild");
			auto& pg = target.AddElement("PropertyGroup");
			auto& et = pg.AddElement("ErrorText");
			et.SetContent("Missing NuGet Packages, use the restore to download them");


		}


		if (1)
		{
			/*
			    <Image Include="Assets\LockScreenLogo.scale-200.png" />
    <Image Include="Assets\SplashScreen.scale-200.png">
      <DeploymentContent>true</DeploymentContent>
    </Image>
    <Image Include="Assets\Square150x150Logo.scale-200.png" />
    <Image Include="Assets\Square44x44Logo.scale-200.png" />
    <Image Include="Assets\Square44x44Logo.targetsize-24_altform-unplated.png" />
    <Image Include="Assets\StoreLogo.png" />
    <Image Include="Assets\Wide310x150Logo.scale-200.png" />

	*/
			std::wcout << L"Updating Assets...\r\n";
			auto& p1 = root.AddElement("ItemGroup");

			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\LockScreenLogo.scale-200.png"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\SplashScreen.scale-200.png");
			i1.AddElement("DeploymentContent").SetContent("true"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\Square150x150Logo.scale-200.png"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\Square44x44Logo.scale-200.png"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\Square44x44Logo.targetsize-24_altform-unplated.png"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\StoreLogo.png"); }
			{ auto& i1 = p1.AddElement("Image"); i1.vv("Include").SetValue("Assets\\Wide310x150Logo.scale-200.png"); }


		}


		// App.Xaml/h/cpp
		if (1)
		{
			auto& i1 = root.AddElement("ItemGroup");
			auto& ii1 = i1.AddElement("ClInclude");
			ii1.vv("Include").SetValue("App.xaml.h");
			ii1.AddElement("DependentUpon").SetContent("App.xaml");
			auto& ii2 = i1.AddElement("ApplicationDefinition");
			ii2.vv("Include").SetValue("App.xaml");
			auto& ii3 = i1.AddElement("ClCompile");
			ii3.vv("Include").SetValue("App.xaml.cpp");
			ii3.AddElement("DependentUpon").SetContent("App.xaml");
		}

	}

	if (0)
	{
		// Add the Manifest
		std::wcout << L"Adding Package.appxmanifest...\r\n";
		auto& e = x.GetRootElement();
//		auto& p0 = e.AddElement("ItemDefinitionGroup");
		auto& p1 = e.AddElement("ItemGroup");
		auto& p2 = p1.AddElement("AppxManifest");
		p2.vv("Include").SetValue("Package.appxmanifest");
		auto& p3 = p2.AddElement("SubType");
		p3.SetContent("Designer");
	}


	// Xaml
	std::wcout << L"Creating App.xaml/h/cpp...\r\n";
	CreateAppXaml(args[1]);



	// Copy assets
	std::wcout << L"Copying Assets...\r\n";
	for (auto& c : { L"Wide310x150Logo.scale-200.png",L"LockScreenLogo.scale-200.png",L"SplashScreen.scale-200.png",L"Square44x44Logo.scale-200.png",L"Square44x44Logo.targetsize-24_altform-unplated.png",L"Square150x150Logo.scale-200.png",L"StoreLogo.png" })
	{
		auto bu = GetRootDir(args[2]);
		bu += L"\\Assets\\";
		SHCreateDirectory(0,bu.c_str());
		bu += c;
		CopyFile(c, bu.c_str(), false);
	}

	DeleteFile(args[2]);
	XML3::XMLSerialization xs;
	xs.NoCRLF = 1;
	x.Save(args[2],&xs);
	std::wcout << "End. " << std::endl;
	_getch();
	return 0;
}

