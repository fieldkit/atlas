BUILD=build

default: all

$(BUILD):
	mkdir -p $(BUILD)

all: $(BUILD) gitdeps
	cd $(BUILD) && cmake ../
	cd $(BUILD) && make

gitdeps:
	simple-deps --config firmware/module/arduino-libraries
	simple-deps --config firmware/test/arduino-libraries

clean:
	rm -rf $(BUILD)

veryclean: clean
	rm -rf gitdeps
