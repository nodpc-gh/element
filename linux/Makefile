# Element IDE - Linux Build (FASM → ELF)
# Requires FASM (flat assembler) installed
# Install: sudo apt install fasm  (or download from flatassembler.net)

SERVER = element_linux
SRCDIR = src
WWDIR  = www

.PHONY: all run clean www-copy

all: $(WWDIR)/index.html $(SRCDIR)/main_server.asm
	fasm $(SRCDIR)/main_server.asm $(SERVER)
	@echo "Build: $(SERVER)"
	@ls -lh $(SERVER)

run: all
	./$(SERVER)

clean:
	rm -f $(SERVER)
	rm -f element.db

# Копирование www из основной Windows-версии
www-copy:
	cp -r ../../output/www/* $(WWDIR)/
