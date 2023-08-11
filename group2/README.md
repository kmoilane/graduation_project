# Control software / firmware requirements
The goal is to create control software to run the production line without
breaking hardware. This would be the lowest level component in the final
product, so trying to keep it quite minimal is a bonus.
Extra data processing should be left for higher levels to handle.

- Figure out how to input / output data
    - You'll need to agree with the other teams on this.

- Create control software that operates the production line
    - Read the sensor inputs, and convert them from raw sensor data to more human-readable/usable form
    - Use the acquired data to create automatic control for the system
        - Use dummy data at start, since you don't have "real" or simulated data at that point
        - Try to maximise product output, while causing nothing to break and consuming as little energy as possible
            - This doesn't need to be sophisticated
        - Allow toggling manual overrides to any / all of the values (and still handle the other values)
    - Try to detect component failures and report them
    - Should be usable as a background process
        - The control software should run as a separate process, and possible to use from somewhere where a computer screen is not available
        - The control software should receive messages via some channel for user control
            - You should make a simple text UI for your own internal testing
    - Allow user to log state if requested
    - Testing!
    - Documentation
        - Describe outputs of your program accurately, knowing what kind of data your program produces is critical.
        - Documentation can be quite technical, since it is not going to be facing end-users but other programmers.
    - If time allows:
        - Add energy usage as a output parameter
        - Try to make your software not do unnecessary work
        - Make SDK library from the controls
