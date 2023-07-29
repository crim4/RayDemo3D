@if not exist "Build" mkdir "Build"
@gcc "Source\Main.c" "Libs\RayLib\Lib\Lib.a" -o "Build\Demo.exe" -std=c99 -Wall -Wextra -Werror -lwinmm -lgdi32
@rem -O3 -g