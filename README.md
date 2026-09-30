# project-c
# PokerMaster – Texas Hold'em Poker Engine

## 1. Project Description

PokerMaster is a terminal-based Texas Hold'em Poker game
implemented completely in C.

The project focuses on implementing card management,
randomization, poker hand evaluation, betting logic,
and winner determination without relying on external
game libraries.

## 2. Problem Statement

Develop a command-line poker engine capable of managing
a complete Texas Hold'em game for multiple players.

The system should handle card generation, shuffling,
dealing, betting rounds, hand evaluation, and winner
determination.

## 3. Goals

- Implement a standard 52-card deck.
- Implement randomized card shuffling.
- Support multiple players.
- Implement Texas Hold'em game flow.
- Implement poker hand ranking.
- Determine winners automatically.
- Implement betting and chip management.
- Handle ties and folded players.
- Maintain game statistics.

## 4. Specifications

### Players
- 2–6 players
- Each player receives 2 private cards.

### Community Cards
- 3-card flop
- 1-card turn
- 1-card river

### Hand Rankings

1. Straight Flush
2. Four of a Kind
3. Full House
4. Flush
5. Straight
6. Three of a Kind
7. Two Pair
8. One Pair
9. High Card

## 5. Technologies

- Language: C
- Compiler: GCC
- Build System: Make
- Interface: Command Line
- Version Control: Git/GitHub

## 6. Algorithms

### Deck Shuffling
Fisher-Yates Shuffle

### Hand Evaluation
The program evaluates all possible
5-card combinations from the player's
7 available cards.

### Winner Determination
Hands are compared using:

1. Hand rank
2. Primary card values
3. Tie-breaking cards (kickers)

## 7. Data Structures

- Structures
- Arrays
- Enumerations
- Functions
- Modular source files

## 8. Project Architecture

...

## 9. Future Enhancements

- Poker AI
- Save/load functionality
- Tournament mode
- Statistics
- Side pots