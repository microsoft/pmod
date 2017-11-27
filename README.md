## Welcome!

The pmod SDK is a Microsoft project to allow native code to be used on different languages and platforms with cross-platform in mind. The SDK offer both a runtime implementation and also code generation tools to easily produce 'projection' code into other languages like C#, WinRT, Objective-C, Java, NodeJS. It also offer additional support for MVVM pattern with support of observable objects that willl work on different platforms.

## MSFT internal usage

Since people have asked about the library internal usage on Microsoft(s) products, here are some facts:
 * Skype for Business: The popular and former Office LYNC product use the V1 of this library that was only intended for Windows/Desktop platform. That project defines over > 500 model types and several complex View Models to power the real time low level protocol with the Office UI rendering engine. The project was a great success with the help of an Office binding adapter that allow View models to be easly connected with the UI bindings

 * Skype: The effort was to build a next generation V2 SDK based on a lot of V1 concepts that will run on every possible platform specially on phones with additional requirements to improve perf and memory consumption. The idea was to use the library to unify the protocol native API to be consumed on Android/iOS/UWP without the need to handcraft wrappers for each platform. The project was cancelled at the last minute in favor to continue with the old approach and the future migration to use other languages (like javascript).

 This SDK library is based on the V2 effort with additional requirements to have all the code generation tools to run on linux and mac and also a better layer separation to use the projection without the need to use the full library implementation

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
