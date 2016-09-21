all:
	@echo .
	@echo ServerProgram compiling...
	cd ServerProgram && make flex
	cd ServerProgram && make
	@echo .
	@echo CheckExistence compiling...
	cd CheckExistence && make
	@echo .
	@echo ExpertGUI compiling...
	cd ExpertGUI/ExpertGUI && qmake ExpertGUI.pro && make
	@echo .
	@echo UserGUI compiling...
	cd UserGUI/UserGUI && qmake UserGUI.pro && make
clean:
	@echo .
	@echo ServerProgram cleaning...
	cd ServerProgram && make clean
	@echo .
	@echo CheckExistence cleaning...
	cd CheckExistence && make clean
	@echo .
	@echo ExpertGUI compiling...
	cd ExpertGUI/ExpertGUI && make clean
	@echo .
	@echo UserGUI compiling...
	cd UserGUI/UserGUI && make clean
