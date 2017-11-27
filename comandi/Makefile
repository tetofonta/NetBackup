COMPILER = gcc
SIGN = ../../Utils/signElf

%: %.c
	$(COMPILER) $< -o $@ -O2 -shared -fPIC
	$(SIGN) %