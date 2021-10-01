# sumo-traci-public-transport
This little projects shows, how the TraCI-API can be used to control traffic-lights in order to archieve less waiting times for public transport vehicles.

To use the program, you have to edit "traci.cfg.xml", which is the config for the program.

| Parameter Name | Describtion                                                                 | Value Type |
|----------------|-----------------------------------------------------------------------------|------------|
| Port           | Port, that the TraCI Server shoulduse (when set to -1, it uses a free port) | int        |
| sumocfg        | Path of the simulation that shall be opened                                 | string     |
| sumoexe        | Path of the sumo application                                                | string     |

The program can handle multiple traffic-lights at the same time.

| Parameter Name     | Describtion                                                                                                                                                                       | Value Type    |
|--------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------|
| id                 | Custom id for this instance                                                                                                                                                       | int           |
| tls                | Id of the TLS that should be controlled (multiple instances with same TLS possible)                                                                                               | String        |
| loops              | List of every loop that should be used for this configuration                                                                                                                     | Array         |
| loop               | Id of one loop, that should be used                                                                                                                                               | String        |
| prePhase           | Defines which phase comes before final phase (e.g. yellow) and for how long                                                                                                       | int, int      |
| phaseContinuations | Which phases shall be continued and extendet                                                                                                                                      | Array         |
| phase              | Id of Phase, that should be extendet                                                                                                                                              | int           |
| finalPhase         | Defines which phase is the main target, how long it should be and for how long it should be extendet if the cehicle didn't reached the last loop after the normal firation ended. | int, int, int |
| detection          | Defines which vClasses should have an influence on the control                                                                                                                    | Array         |
| detect             | One vClass, that should be used                                                                                                                                                   | String        |

To run the simulation, simply run the traci-program.
If you want to compile the project, make sure, you set the compiler to use ANSI-Libraries, because the TraCI-Library is using ANSI. You only have to add the directories of you sumo installation into the the includes paths and library paths
