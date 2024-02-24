Copy-Item 'C:\Program Files (x86)\Windows Kits\10\Vsix\VS2022\*\WDK.vsix' -Destination 'C:\TEMP\WDK.zip'
Expand-Archive 'C:\TEMP\WDK.zip' -DestinationPath 'C:\wdkvsix\'
Copy-Item 'C:\wdkvsix\$MSBuild\*' -Destination 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\' -Recurse -Force