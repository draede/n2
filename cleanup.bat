rmdir /s /q ".\Bin"
rmdir /s /q ".\Lib"
rmdir /s /q ".\Temp"

del /q /s /f ".\*.sdf"
del /q /s /f ".\*.opensdf"
del /q /s /f /a:h ".\*.suo"
del /q /s /f ".\*.user"
del /q /s /f ".\*.VC.db"

