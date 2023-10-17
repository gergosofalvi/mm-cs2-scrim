![Build pass](https://github.com/marqdevx/mm-cs2-scrim/actions/workflows/compile-metamod-addon.yaml/badge.svg)

## This repository is just a tweak of https://github.com/Source2ZE/CS2Fixes/
Removing "unnecessary" features from the original addon, to have the minimum to just run the commands.

## DISCLAIMER: *not* responsible for any issues or breakages caused by the plugin. 

# Competitive Practice & Scrim for CS2
This plugin adds the minimum requirements for competitive teams that need a more handy way of managing a few commands from the server side, like config management or replay recording.

## Available Commands

`.pracc`: launch the practice config  
`.scrim`: start the competitive match  
`.pause`: pauses the match  
`.unpause`: request the unpause  
`.record`: start the demo recording  
`.stoprecord`: finish the recording and save it under `gotv/<date>.dem`  
`.restore <round>`: load the desired round's backup  

## Roadmap
- Scrim
  - [X] Pauses
  - [X] Medic (round backup)
  - [ ] Coach
  - [X] gotv .record
    - [X] include date and time
    - [ ] include map name
- Practice
  - [X] Map
  - [ ] Spawns
  - [ ] Remove smokes
  - [ ] Move to spec
  - [ ] Noclip

## Compilation

### Requirements

- [Metamod:Source](https://www.sourcemm.net/downloads.php/?branch=master) (build 1219 or higher)
- [AMBuild](https://wiki.alliedmods.net/Ambuild)

### Instructions
TODO

## Authors from the [original repository](https://github.com/Source2ZE/CS2Fixes/)
- [@xen-000](https://github.com/xen-000)
- [@poggicek](https://github.com/poggicek)
