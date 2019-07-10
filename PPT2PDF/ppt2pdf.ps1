[CmdletBinding()]
param(
    $Src,
    $Dest
)

$SrcPath = Convert-Path $Src
$DestPath = Convert-Path $Dest

$PowerPointApplication = New-Object -ComObject PowerPoint.Application

Get-ChildItem -Path $SrcPath -Filter *.ppt? | ForEach-Object {
    Write-Host "Converting " $_.FullName "..."
    $PowerPointFile = $PowerPointApplication.Presentations.Open($_.FullName)
    $PDFFileName = "$($DestPath)\$($_.BaseName).pdf"
    $Option = [Microsoft.Office.Interop.PowerPoint.PpSaveAsFileType]::ppSaveAsPDF
    $PowerPointFile.SaveAs($PDFFileName, $Option)
    $PowerPointFile.Close()
}

$PowerPointApplication.Quit()
[System.Runtime.InteropServices.Marshal]::ReleaseComObject($PowerPointApplication)
[System.GC]::Collect()
[System.GC]::WaitForPendingFinalizers()