MAKE = make --no-print-directory

all:
	@$(MAKE) --directory="./SPU/"
	@$(MAKE) --directory="./ASSEMBLER/"
clean:
	@$(MAKE) --directory="./SPU/" clean
	@$(MAKE) --directory="./ASSEMBLER/" clean
	@rm -f AsmBin/*.bin

factorial:
	@./ASSEMBLER/assembler --source ./AsmSrc/factorial --target ./AsmBin/factorial.bin
	@./SPU/spu --source ./AsmBin/factorial.bin

quadratic:
	@./ASSEMBLER/assembler --source ./AsmSrc/quadratic_equation --target ./AsmBin/quadratic_equation.bin
	@./SPU/spu --source ./AsmBin/quadratic_equation.bin

run:
	@./ASSEMBLER/assembler --source ../backend/beprog.sasm --target ./AsmBin/source.bin
	@./SPU/spu --source ./AsmBin/source.bin

lang:
	@./ASSEMBLER/assembler --source ../backend/beprog.sasm --target ../prog.opa

exec:
	@./SPU/spu --source ../prog.opa
