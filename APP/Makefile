########################################
#~ définitions
########################################

# Executables à générer
BIN=main

# liste des fichiers sources 
MAIN=main.cpp
IMAGE=Library/ImageBase.cpp

default: $(BIN)

########################################
#~ regles pour l'executable
########################################

obj/%.o: %.c
	gcc -Wall -Iinclude -c $< -o $@

main: $(MAIN:%.c=obj/%.o) $(IMAGE:%.c=obj/%.o)
	g++ -o $@ $+

clean:
	rm -f $(BIN) obj/*.o *~
