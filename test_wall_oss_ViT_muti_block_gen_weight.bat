  @echo off
  for %%i in (1 2 3 4 5 6 7 8) do (
      echo ========== Running model_part %%i ==========
      .\bin\test_wall_oss_ViT_muti_block_gen_weight.exe %%i
      if errorlevel 1 (
          echo Failed at model_part %%i
          pause
          exit /b 1
      )
  )
  echo All done!
  pause