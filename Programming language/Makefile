MAKE = make --no-print-directory

all:
	@$(MAKE) --directory="./preprocessor/"
	@$(MAKE) --directory="./frontend/"
	@$(MAKE) --directory="./middleend/"
	@$(MAKE) --directory="./backend/"
	@$(MAKE) --directory="./MyProcessor"
clean:
	@$(MAKE) --directory="./preprocessor/" clean
	@$(MAKE) --directory="./frontend/" clean
	@$(MAKE) --directory="./middleend/" clean
	@$(MAKE) --directory="./backend/" clean
	@$(MAKE) --directory="./MyProcessor" clean
	@rm -f ./*.sh ./*.opa
