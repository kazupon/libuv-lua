LIBUV_DIR = deps/libuv

libuv:
	$(MAKE) -C $(LIBUV_DIR)

clean-libuv:
	$(MAKE) -C $(LIBUV_DIR) clean

clean: clean-libuv

.PHONY: libuv clean-libuv clean
