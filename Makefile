SRC_DIR = src

default: all

sea:
	$(MAKE) -C $(SRC_DIR)

all: sea

.PHONY: all
