# NOTES

**Name: Qianyu Huang** 

## Extensions I did
An extension I implemented is tracking the longest session for each player, then printing those stats onto the report. The program tracks the duration of each continuous session and extracts the player's longest sesison. 

## Changes to the design (and why)
To support my extension, i did add private member variables in the Player class to track the duration of an active session as well as the longest session. Additionally, playersByLongestSession() was added to ServerLog to sort players by their longest continuous session in decending order.

## Answers to the questions in the handout

The server's play time number is a bit higher since Minecraft starts counting a player's play time when they connect to the server. However, the program i wrote starts counting from when the game line is printed in the log. Due to the fact that the message is printed a bit after the player connects, the program does not record the few seconds between those two events. As a reuslt, the server's recoreded play time is higher than my calculated time. 