BUILD=build

all: cmake
	cd $(BUILD) && make

cmake: gitdeps
	mkdir -p $(BUILD)
	cd $(BUILD) && cmake ../

gitdeps:
	simple-deps --config firmware/module/arduino-libraries
	simple-deps --config firmware/test/arduino-libraries

clean:
	rm -rf $(BUILD) gitdeps
