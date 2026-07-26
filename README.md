# Crossed Contracts
### Working on title (Project codename: K7)

> A multiplayer stealth sandbox where you're not the only assassin.

---

## About

K7 is a multiplayer stealth-action game where several assassins compete to eliminate their targets and escape alive.
Inspired by the chaotic multi-assassin dynamic of movies like *Smokin' Aces*,every mission is different. 
Sometimes another killer is hunting the same target. Sometimes they are hunting you. Guards, civilians, and other players continue living their own lives while missions unfold, forcing players to constantly adapt instead of following a scripted plan.

You are free to choose your own approach. Walk through the crowd unnoticed, disguise yourself, wait for the perfect opportunity, or strike quickly before another assassin completes the contract.

**Winning isn't only about eliminating the target—it's about surviving long enough to leave the area.**

---

## Core Gameplay

- Multiple assassins operating in the same mission.
- One or more targets.
- Compete to finish the contract first.
- Escape before being caught or eliminated.
- Blend into crowds and avoid suspicion.
- Use disguises, weapons, and planning.
- Every match can play out differently.

---

## Dynamic World

The world doesn't stop because the player arrives.

Civilians go to work, eat, socialize, panic, flee danger, and react to events around them.

Guards protect important locations and respond to threats.

Other assassins have their own objectives and may become your greatest obstacle—or your unexpected ally.

---

## Current Development

The project is currently in an early prototype stage.

The core gameplay framework and foundational mechanics are implemented, while many gameplay systems are still under active development.

### Implemented systems

- C++ gameplay framework
- Character hierarchy
- Weapon framework
- Combat system
- Damage and ragdoll
- Clothing system
- Inventory
- Item pickup
- Foundation of civilian behavior
- Dynamic task priority system (Work in Progress)

---

## Project Architecture

AK7LivingEntity *(current class name: K7Npc)*

```
AK7LivingEntity
│
├── Health
├── Damage
├── Death
├── Clothing
└── Life State
    │
    ├── AK7Civilian
    │
    └── AK7CombatBase
        │
        ├── Weapon System
        ├── Combat
        ├── Inventory
        ├── Reload
        │
        ├── AK7GuardBase (planned)
        └── AK7KillerBase
            │
            ├── Equip clothing
            ├── Drag bodies
            ├── Remove clothing from dead characters
            ├── Dress other characters to reduce suspicion (planned)
            │
            └── AK7Character
                └── Player movement, input handling and gameplay implementation
```

---

## Repository

This repository focuses on the gameplay architecture and C++ implementation.

Game assets are intentionally excluded because they contain Unreal Engine and third-party content that cannot be redistributed publicly.

---

## Planned Features


- Multiplayer
- Rival assassins
- Assassin skill tree
- Cooperative missions
- Single-player campaign
- AI-controlled assassins
- Playable guards
- Mission planning
- Smarter civilian behaviors
- Dynamic reputation and memory systems
- Expanded equipment and disguise mechanics
- Continued polishing of current gameplay systems