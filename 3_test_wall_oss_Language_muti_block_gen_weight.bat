@echo off
for %%i in (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37) do (
    echo ========== Running model_part %%i ==========
    .\bin\3_test_wall_oss_Language_muti_block_gen_weight.exe %%i
    
    if errorlevel 1 (
        echo Failed at model_part %%i
        pause
        exit /b 1
    )

    :: 休息 1 秒以释放内存
    timeout /t 5 /nobreak > nul
)
echo All done!
pause