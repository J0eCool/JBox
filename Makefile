IT_TOOLS=../it-tools/src
FILES=\
out/circles.wasm \
out/game.wasm \
out/lexer.wasm \
out/parser.wasm \
out/life.wasm \
out/terminal.wasm \

OPT=-O1
WABT_FLAGS=--enable-simd

build: out $(FILES)

clean: out
	rm -r out/
.PHONY: clean

serve: build
	node src/server.js
.PHONY: serve

out:
	mkdir -p out

it_tools: $(IT_TOOLS)/*

out/%.wasm out/%.itl: src/modules/%.cpp
	python $(IT_TOOLS)/cpp_itl_generator.py src/modules/$*.cpp --cpp out/$*.cpp --itl out/$*.itl --wasm $*.wasm
	python $(IT_TOOLS)/adapter.py out/$*.itl -o out/$*.js
	emcc out/$*.cpp -o out/$*.wasm $(OPT) \
		-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s TOTAL_MEMORY=256MB \
		-s ALLOW_TABLE_GROWTH=1 -Wl,--export-table \
		-Iout -Isrc/modules -std=c++11 --no-entry --profiling-funcs -Wall
	wasm2wat $(WABT_FLAGS) out/$*.wasm -o out/$*.wat
	wasm-decompile $(WABT_FLAGS) out/$*.wasm -o out/$*.wade
