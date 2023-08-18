@if not exist "Build" mkdir "Build"
@gcc "Source\Main.c" "Source\Context.c" "Source\Game.c" "Libs\RayLib\Lib\Lib.a" -o "Build\Demo.exe" -O3 -std=c99 -Wall -Wextra -Werror -lwinmm -lgdi32
@rem -O3 -g