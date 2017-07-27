## Welcome!

The pmod SDK is a Microsoft project to allow native code to be used on different languages and platforms with cross-platform in mind. The SDK offer both a runtime implementation and also code generation tools to easily produce 'projection' code into other languages like C#, WinRT, Objective-C, Java, NodeJS. It also offer additional support for MVVM pattern with support of observable obejcts that willl work on different platforms.

## Getting Started

For Windows platform open the VS 2017 pmod.sln solution to browse the code and build the runtime components. For iOS/MacOSX there are XCode projects that facilitate the browsing and build. For Android you can use the Android SDK tools or VS 2017 to build the binaries, make sure you add the proper workloads to build C++ cross platform and UWP.

## NuGet packages

For Windows there is support for NuGet packages for most of the components being build. Your VS 2017 projects can add a reference to the packages (both native and managed) usign this NuGet feed: [my pmod](https://www.myget.org/F/pmod/api/v3/index.json)

## Building the samples
Running and building the sample is the best way to evaluate the SDK. The repo contains two supported samples:
* DemoApp: shows and observable collection with items and connect it to different UI components in XAML, Cocoa and Android activities
* Movies: Use [MSFT Cpp REST SDK](https://github.com/Microsoft/cpprestsdk) library to connect to a REST service and display movies being played in theaters.


### Quick Links

*   [FAQ](https://github.com/Microsoft/pmod/wiki/FAQ)
*   [Source Code introduction](https://github.com/Microsoft/pmod/wiki/Introduction%20Source)
*   [Introduction (Word)](https://github.com/Microsoft/pmod/wiki/pmod-SDK.docx)
*   [Tutorial (Word)](https://github.com/Microsoft/pmod/wiki/SDK%20Tutorial.docx)
*   [NuGet feed](https://www.myget.org/F/pmod/api/v3/index.json)
*   [DemoApp solution for VS 2017](https://github.com/Microsoft/pmod/tree/master/samples/DemoApp/__build/vs)

*   Directly contact us: <rodrigov@microsoft.com>

# Contributing

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
