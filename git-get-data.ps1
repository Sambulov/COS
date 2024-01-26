git log -1 --date=format:'%Y%m%d' | findstr Date | %{$_ -replace "Date:",""} | %{$_ -replace " ",""}
