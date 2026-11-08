# 调 Keil UV4 命令行编译，并把日志打印到 VS Code 终端
# 自动适配两种工程布局：
#   <工作区>\mdk\*.uvproj           例程文件夹直接作为工作区
#   <工作区>\project\mdk\*.uvproj   库在根目录、代码在 project 下的工程
param(
    [string]$Project,
    [switch]$Rebuild,
    [switch]$OpenKeil
)

$ErrorActionPreference = 'Stop'
$UV4 = 'D:\KEIL\UV4\UV4.exe'

$workspace = Split-Path $PSScriptRoot -Parent

if (-not $Project) {
    foreach ($d in @((Join-Path $workspace 'mdk'), (Join-Path $workspace 'project\mdk'))) {
        if (Test-Path $d) {
            $f = Get-ChildItem -Path $d -Filter *.uvproj -File -ErrorAction SilentlyContinue |
                Select-Object -First 1
            if ($f) { $Project = $f.FullName; break }
        }
    }
}
if (-not $Project) {
    $Project = Get-ChildItem -Path $workspace -Recurse -Filter *.uvproj -File -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not (Test-Path $UV4)) {
    Write-Host "找不到 UV4.exe: $UV4" -ForegroundColor Red
    exit 1
}
if (-not $Project -or -not (Test-Path $Project)) {
    Write-Host "在工作区里找不到 .uvproj 工程文件" -ForegroundColor Red
    exit 1
}

if ($OpenKeil) {
    Start-Process -FilePath $UV4 -ArgumentList ('"{0}"' -f $Project)
    Write-Host "已用 Keil 打开: $Project" -ForegroundColor Green
    exit 0
}

$log  = Join-Path (Split-Path $Project -Parent) 'build.log'
$flag = if ($Rebuild) { '-r' } else { '-b' }

if (Test-Path $log) { Remove-Item $log -Force }

Write-Host "编译: $Project" -ForegroundColor Cyan

# UV4.exe 是 GUI 程序，用 & 直接调用 PowerShell 不会等待它结束，也拿不到返回码
# 必须用 Start-Process -Wait -PassThru，并给可能带空格的路径加引号
$procArgs = @($flag, ('"{0}"' -f $Project), '-o', ('"{0}"' -f $log))
$proc = Start-Process -FilePath $UV4 -ArgumentList $procArgs -Wait -PassThru -NoNewWindow
$code = $proc.ExitCode

if (Test-Path $log) {
    Write-Host "`n---------- Keil 编译日志 ----------" -ForegroundColor Yellow
    # Keil 日志是 GBK(936) 编码，先按 936 读，失败再退回默认
    try {
        $text = [System.IO.File]::ReadAllText($log, [System.Text.Encoding]::GetEncoding(936))
    } catch {
        $text = Get-Content $log -Raw
    }
    Write-Host $text
    Write-Host "-----------------------------------`n" -ForegroundColor Yellow
} else {
    Write-Host "没有生成 build.log，检查工程是否能正常打开" -ForegroundColor Yellow
}

switch ($code) {
    0  { Write-Host "[成功] 编译成功，0 error / 0 warning" -ForegroundColor Green }
    1  { Write-Host "[警告] 编译完成，但存在警告（Keil 返回 1 不代表失败）" -ForegroundColor Yellow }
    2  { Write-Host "[失败] 编译失败：有错误" -ForegroundColor Red }
    3  { Write-Host "[失败] 编译失败：致命错误" -ForegroundColor Red }
    11 { Write-Host "[失败] 无法打开工程文件" -ForegroundColor Red }
    15 { Write-Host "[失败] 设备/许可证错误（检查 Keil 是否已激活）" -ForegroundColor Red }
    default { Write-Host "UV4 返回码: $code" -ForegroundColor Yellow }
}

exit $code
