for /r %%i in (*.vcxproj *.vcxproj.filters *.vcxproj.user) do (
    echo %%i
    del "%%i"
)

if exist experiment.sln (
    del "experiment.sln"
)
