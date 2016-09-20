all:
	@echo .
	@echo ServerProgram compiling...
	cd ServerProgram && make flex
	cd ServerProgram && make
	@echo .
	@echo CheckExistence compiling...
	cd CheckExistence && make
clean:
	@echo .
	@echo ServerProgram cleaning...
	cd ServerProgram && make clean
	@echo .
	@echo CheckExistence cleaning...
	cd CheckExistence && make clean
