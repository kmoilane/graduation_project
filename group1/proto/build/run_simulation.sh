#!/bin/bash

tmux new-window "./Simulation"

# Run the second executable in another new tmux window
tmux split-window "./controller"

tmux split-window

# Attach to the tmux session
tmux attach-session -d
