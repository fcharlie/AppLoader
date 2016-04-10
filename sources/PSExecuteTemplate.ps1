#
$env:PATH+=";"+"F:\Development;"
Start-Process -FilePath "Notepad" -ArgumentList "`"$env:TEMP\hello.txt`" -HHHH"