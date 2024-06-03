for /r %%i in (*.vcxproj) do (
    echo %%i
    del "%%i"
)

if exist experiment.sln (
    del "experiment.sln"
)
