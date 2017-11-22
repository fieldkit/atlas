BUILD=build

all: cmake
	cd $(BUILD) && make

cmake: gitdeps
	mkdir -p $(BUILD)
	cd $(BUILD) && cmake ../

gitdeps:
	simple-deps

clean:
	rm -rf $(BUILD) gitdeps
