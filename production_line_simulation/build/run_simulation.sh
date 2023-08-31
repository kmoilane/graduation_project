#!/bin/bash


# Check if we are in a tmux session

if [ -n "$TMUX" ]; then

echo "Already in a tmux session. Exit and try again."

else

tmux new-session -d -s simulation
tmux new-window -t simulation "./simulation"
tmux split-window -v -t simulation "./ui"
tmux split-window -h -t simulation "./controller"
tmux attach-session -t simulation

fi

 