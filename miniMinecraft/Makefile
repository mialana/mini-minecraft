.PHONY: *

format: src
	find . -path ./include -prune -o -type f \( -iname '*.h' -o -iname '*.cpp' -o -iname '*.glsl' \) -print | xargs clang-format -i
