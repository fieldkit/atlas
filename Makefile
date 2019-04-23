BUILD ?= $(abspath build)

default: all

$(BUILD):
	mkdir -p $(BUILD)

all: $(BUILD) gitdeps
	cd $(BUILD) && cmake ../
	$(MAKE) -C $(BUILD)

gitdeps:
	simple-deps --config firmware/module/dependencies.sd
	simple-deps --config firmware/test/dependencies.sd
	simple-deps --config firmware/calibration/dependencies.sd

clean:
	rm -rf $(BUILD)

veryclean: clean
	rm -rf gitdeps
