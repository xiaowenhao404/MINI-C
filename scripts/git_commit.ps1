# Mini-C Git 提交辅助脚本
# 用途：在 PowerShell 中安全地进行 Git 提交，避免路径和编码问题

param(
    [Parameter(Mandatory=$true)]
    [string]$Title,
    
    [Parameter(Mandatory=$false)]
    [string[]]$Details
)

# 获取脚本所在目录的父目录（项目根目录）
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir

# 切换到项目根目录
Set-Location $ProjectRoot

# 构建提交信息
$CommitMessage = $Title + "`n"
if ($Details) {
    $CommitMessage += "`n"
    foreach ($detail in $Details) {
        $CommitMessage += "- $detail`n"
    }
}

# 创建临时提交信息文件
$TempFile = Join-Path $ProjectRoot "temp_commit_msg.txt"
$CommitMessage | Out-File -FilePath $TempFile -Encoding UTF8 -NoNewline

try {
    # 执行提交
    Write-Host "正在提交..." -ForegroundColor Yellow
    git commit -F $TempFile
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✓ 提交成功！" -ForegroundColor Green
        Write-Host "`n最新提交:" -ForegroundColor Cyan
        git log -1 --oneline
    } else {
        Write-Host "`n✗ 提交失败！" -ForegroundColor Red
    }
}
finally {
    # 清理临时文件
    if (Test-Path $TempFile) {
        Remove-Item $TempFile -Force
    }
}

