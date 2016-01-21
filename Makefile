# Define .o files for simulation 
OBJECTS = simulation.o simulationDriver.o

# Default rules for building each .o
simulationDriver.o: simulationDriver.c simulation.h
	gcc -g -c simulationDriver.c -o simulationDriver.o

simulation.o: simulation.c simulation.h
	gcc -g -c simulation.c -o simulation.o

# Default rule for building executable
sim: ${OBJECTS}
	gcc -g -o sim ${OBJECTS}

# Special rule for removing .o files
clean:
	rm -f ${OBJECTS}
