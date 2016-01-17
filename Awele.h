/* *********************************************************
 * Awele.h
 * 
 * Mécanisme du jeu d'Awele et algorithmes de calcul de 
 * la meilleure solution.
 *
 * (C) Erwan Louët 1998, 1999, 2010, 2016.
 * ********************************************************* */

#ifndef AWELE_H
#define AWELE_H

#include <string>

class HalfBoard {
private:
  int seeds[6];
  int seedsTaken = 0;

  bool origin[6];
  bool winHere = false;

public:
  static constexpr int initialState[6] = {4, 4, 4, 4, 4, 4};

  HalfBoard *opposite;

  HalfBoard( const int *fill, HalfBoard *opp );
  HalfBoard( const int *fill );
  HalfBoard();
  HalfBoard( const HalfBoard *copyme );

  int sumSquares();

  std::string toString();
  HalfBoard reverse();
  void setOrigin(int sq);

  int play(int at);
  bool canPlay( int at );
  bool canPlay();
  int where();
  int handOver( int numOfSeeds );
  void reset();
};


//namespace com.sylou.awele {

/**
      * Classe principale de Awele programme principal de gestion 
      * du jeu.
      *
      * Principe de fonctionnement:
      * L'algorithme utilisé pour calculer les mouvements du 
      * joueur "ordinateur" est récursif. Pour chaque possibilité 
      * légale de jouer offerte à l'ordinateur, on met en face
      * de l'ordinateur un autre ordinateur (qui à son tour va 
      * chercher à simuler en utilisant un autre ordinateur, 
      * etc...). Cette récursivité est limitée à l'aide d'une
      * profondeur maximale qui définit le niveau du jeu
      * (i.e. le talent du joueur adverse).
      *
      */
class Awele{

private :
   int lastMove = -1;
   
   static const int default_difficulty=2;
   int difficulty; // TODO : should be static
   bool over=false;

public:

	static enum Errors{
		notAllowed,
		gameOver
	} Errors;


   HalfBoard player;
   HalfBoard opponent;

   /* constructor */
   Awele();
   Awele( const HalfBoard *pl, const HalfBoard *op );

   Awele reverse();
   void reset();
   void setDifficultyLevel(int level);
   void play (int square); // TODO throws exception
   void opponentPlays (int at); // TODO throws GameOverException
   int bestMove(int limit);
   bool gameIsOver();
   int bestOpponentScore( int depthLimit );
   std::string toString();
   int letOpponentPlay();
};

//}

#endif
