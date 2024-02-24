# escape=`

# Use the latest Windows Server Core 2019 image.
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Restore the default Windows shell for correct batch processing.
SHELL ["cmd", "/S", "/C"]

# Copy our Install script.
COPY Install.cmd C:\TEMP\

# Download collect.exe in case of an install failure.
ADD https://aka.ms/vscollect.exe C:\TEMP\collect.exe

RUN `
    # Download the Build Tools bootstrapper.
    curl -SL --output vs_buildtools.exe https://aka.ms/vs/17/release/vs_buildtools.exe `
    `
    && (call C:\TEMP\Install.cmd vs_buildtools.exe --quiet --wait --norestart --nocache `
        --installPath "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools" `
        --add Microsoft.VisualStudio.Workload.VCTools `
        # --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 `
        # --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 `
        # --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 `
        # --remove Microsoft.VisualStudio.Component.Windows81SDK `
        # --add Microsoft.VisualStudio.Component.VC.14.34.17.4.x86.x64.Spectre `
        # --add Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre `
    )
    # `
    # Cleanup
    # && del /q vs_buildtools.exe

RUN `
    (call C:\TEMP\Install.cmd vs_buildtools.exe --quiet --wait --norestart --nocache `
        --installPath "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools" `
        --add Microsoft.VisualStudio.Component.VC.14.34.17.4.x86.x64.Spectre `
        --add Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre `
    )

# Windows SDK 10.0.22621
RUN `
    curl -SL --output winsdksetup.exe https://go.microsoft.com/fwlink/p/?linkid=2196241 `
    `
    && (start /w winsdksetup.exe /features + /norestart /quiet /log c:\TEMP\winsdk.log)

# Windows Driver Kit 10.0.22621
RUN `
    curl -SL --output wdksetup.exe https://go.microsoft.com/fwlink/?linkid=2196230 `
    `
    && (start /w wdksetup.exe /features + /norestart /quiet /log c:\TEMP\wdk.log)
COPY InstallWdkVsix.ps1 C:\InstallWdkVsix.ps1
RUN powershell -file C:\InstallWdkVsix.ps1

# Define the entry point for the docker container.
# This entry point starts the developer command prompt and launches the PowerShell shell.
USER ContainerAdministrator
COPY . C:/code
WORKDIR C:/code

ENTRYPOINT ["C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat", "&&", "powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]

