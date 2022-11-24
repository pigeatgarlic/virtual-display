Dockerfile for building Indirect Display Driver

# Docker image
## Installed tools
- [Visual Studio Build Tools](https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2022)
- [Windows SDK 10.0.22621 (Windows 11, version 22H2 SDK)](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
- [Windows Driver Kit 10.0.22621 (Windows 11, version 22H2 WDK)](https://go.microsoft.com/fwlink/?linkid=2196230)

## Build image
Make sure to use Docker with Windows containers.

```
docker build -t buildtools:latest -m 4GB .
```

## Using the built image
Map the local source code and build in the container terminal.

```
docker run -v c:\path\to\virtual-display:c:\code -m 4GB -it buildtools

# in the container
# need /p:TargetVersion=Windows10 due to https://gitlab.kitware.com/cmake/cmake/-/issues/23717
cd c:\code
MSBuild.exe .\IddSampleDriver.sln /p:Configuration=Debug /p:Platform=x64 /p:TargetVersion=Windows10
```

# References
1. [Windows driver samples](https://github.com/microsoft/Windows-driver-samples)
2. [Windows Driver Kit (WDK)](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)
3. [Install Build Tools into a container](https://learn.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022)