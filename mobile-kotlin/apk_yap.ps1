Write-Host "PyVakitci Android APK derleniyor..." -ForegroundColor Cyan
Set-Location -Path $PSScriptRoot
$env:GRADLE_USER_HOME = "C:\Users\Rahman\source\apps\_cache\gradle"
$env:JAVA_HOME        = "C:\Program Files\Android\Android Studio\jbr"
$env:ANDROID_HOME     = "C:\Users\Rahman\AppData\Local\Android\Sdk"

.\gradlew.bat assembleDebug

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n==========================================" -ForegroundColor Red
    Write-Host "[HATA] Gradle derleme hatasi aldi!" -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "`nCikmak icin herhangi bir tusa basin..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit $LASTEXITCODE
}

$apkPath = "app\build\outputs\apk\debug\app-debug.apk"
$destPath = "$PSScriptRoot\com.rahmanyazgan.pyvakitci.apk"

if (Test-Path $apkPath) {
    Copy-Item -Path $apkPath -Destination $destPath -Force
    Write-Host "`n==========================================" -ForegroundColor Cyan
    Write-Host "[BASARILI] APK basariyla olusturuldu!" -ForegroundColor Green
    Write-Host "Dosya: $destPath" -ForegroundColor Yellow
    Write-Host "==========================================" -ForegroundColor Cyan

    # WhatsApp Otomatik Gonderim
    $sendScript = "$PSScriptRoot\scripts\send_whatsapp.py"
    if (Test-Path $sendScript) {
        Write-Host "`n[WHATSAPP] APK WhatsApp (+905535836434) uzerinden gonderiliyor..." -ForegroundColor Cyan
        python "$sendScript" "$destPath" "905535836434"
    }
} else {
    Write-Host "`n==========================================" -ForegroundColor Red
    Write-Host "[HATA] APK dosyasi bulunamadi: $apkPath" -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
}

Write-Host "`nTamamlandi."

