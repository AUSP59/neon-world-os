param([string]$Path="C:\Program Files\Neon\neon.exe",[string]$Name="NeonWorldOS")
New-Service -Name $Name -BinaryPathName "`"$Path`" --version" -StartupType Automatic -Description "NEON World OS service"
# To remove: sc.exe delete $Name
