IT_TOOLS=../it-tools/src
FILES=out/index.html out/style.css out/shell.js \
out/circles.wasm \
out/game.wasm \
out/life.wasm \
out/terminal.wasm \

OPT=-O1

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
		-Iout -Isrc/modules -std=c++11 --no-entry --profiling-funcs
	wasm2wat out/$*.wasm -o out/$*.wat
	wasm-decompile out/$*.wasm -o out/$*.wade

out/%.html: out src/%.html
	cp src/$*.html out/$*.html
out/%.css: out src/%.css
	cp src/$*.css out/$*.css
out/%.js: out src/%.js
	cp src/$*.js out/$*.js
