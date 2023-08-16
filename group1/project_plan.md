# Simulation group - Project plan

## Project group (names): Juho, Karri, Kirsi

## 1) Please write below a short text (pitch) of your project idea.

Our idea is to create a production line environment simulation (C++ program). 
The environment produces sensor data that is sent to shared memory addresses for
controller to read. Similarly we read commands from controller for our hradware
to act upon. Goal is to achieve realistic simulation,
so that in software, swapping simulation to real sensors and hardware would be
as seamless as possible.


## 2) Please explain how you’re going to implement the project. Please include the technologies, tools, methods etc. that you’re going to use. Here you can add the project’s functionalities: MVP and additional functionalities.

### Implementation:

First we start by planning and dividing the program into smaller pieces. We
create tasks, and assign responsibilities. Then we quickly start to create the
prototype to get some data moving. We will divide the project into logical
virtual base classes like Sensor, Hardware, and derive these to more specific
types. The exact implementation will be decided during planning phase, and may
change afterwards. Then we create functions for classes, and algorithms for
simulation and unit test them.

By now we have most likely encountered some terrible, moral breaking, soul
crushing bugs and problems that we didn't consider during the planning phase.
We'll fix those like it's childsplay and start to test integration with
controller i.e. shared memory implementation.

##### Countless broken keyboards and monitos later...

When bugs are fixed and SHM is working (maybe while it's under work), all or 
some of us can start to
polish the prototype into an MVP. MVP would include communication and some
minimum environment simulation, so that controller and UI can work with it
properly. Additional functionality would be to implement better algorithms into
the simulation environment (e.g. how heat and cooling affect all sensors etc).

During the SHM implementation and after it, we will discuss with the controller 
group about the implementation and possibly update our simulation based on 
the discussions/their needs.

### Technologies and tools:
VS Code, CMake, C++, doctest, Git, Github projects, agile, Linux

## 3) Create an estimated timeline / schedule that you are following during project weeks

### Monday

- Planning
    - Structure and modularization
    - Assign tasks

### Tuesday

- Prototyping
    - Namespace(s)
    - Classes
    - Functions
    - Environment and algorithms

### Wednesday

- Prototyping continues
- SHM learning/implementation

### Thursday

- Prototyping continues
- SHM implementation and integration testing with controller group
(if they're ready)

### Friday - Wednesday

- Prototype -> MVP
- Bug fixing
- Algorithm optimization
- General keyboard smashing
- Testing
- Presentation
- Dress rehearsals on Tuesday 14-15:30 

### Thursday

- Fine tuning

### Friday

- Product launch! 🚀
