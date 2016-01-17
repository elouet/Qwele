//namespace com.sylou.Awele{

#include "Awele.h"
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

constexpr int HalfBoard::initialState[6];

/* Simple constructor */
Awele::Awele(): player( HalfBoard::initialState), opponent( HalfBoard::initialState ) {
    player.opposite = &opponent;
    opponent.opposite = &player;
    difficulty = default_difficulty;
}

Awele::Awele( const HalfBoard *pl, const HalfBoard *op )
: player(pl), opponent(op)
{
    player.opposite = &opponent;
    opponent.opposite = &player;
}

/** Cette méthode permet de créer un nouveau plateau 
    de jeu, reflet de l'objet mais vu de l'autre côté, ce qui 
    permet de se mettre à la place du joueur adverse, et à
    partir de là, de simuler des mouvements. */

Awele Awele::reverse(){
  Awele ret( &opponent, &player );
  return ret;
}


/** Réinitialise le plateau à 4 graines par cases. */
void Awele::reset(){
	player.reset();
	opponent.reset();
	lastMove = -1;
	over = false;
}

/** Modifie la difficulté du jeu. */
void Awele::setDifficultyLevel(int level){
    difficulty = level;
}

/** Modifie le plateau par le jeu de la case indiquée (numéro 0 à 5).
    @param square numéro de la case à jouer. La case 0 est la case à gauche. */
void Awele::play (int square){ // TODO throws GameOverException {

	if ( player.canPlay(square) ){
		player.play(square);
		lastMove = -1; // Does not apply to user
		if (gameIsOver()){
			over = true;
			throw gameOver;
		}
	}else{
		throw notAllowed;
	}
}

/** Modifie le plateau comme si le joueur opposé avait joué la case spécifiée.
    @param at numéro de la case (0 pour la case la plus à gauche vu du côté opposé). */
void Awele::opponentPlays (int at){
	opponent.play(at);

    if (reverse().gameIsOver()){
    	over = true;
        throw gameOver;
    }
}
   
/** Etudie le meilleur mouvement, par simulation, dans la limite de
    récursion fixée. Cette méthode est récursive, elle essaie
    toutes les possibilités de jeu et évalue chacune d'elles en faisant
    jouer l'adversaire. On décide à la fin de la meilleure solution.
    Afin de rendre le jeu moins monotone, en case d'égalité de valeur
    entre deux coups, le hasard est utilisé.
     @param limit Limitation dans la récursion. */
int Awele::bestMove(int limit){
	int a, score;
	int bestScore = -50000, bestMove = -2;
	std::vector<int> *possibleMoves;

	// Essaye tous les mouvement autorisés
	for (a=5;a>=0;a--){
		if (player.canPlay(a))
		{
			// Pour chaque movement autorisé, crée une
			// nouvelle instance de Awele.
			Awele tmpGame( &player, &opponent);

			// Evalue la valeur de cette option...
			score = tmpGame.player.play(a);
			// ...y compris à long terme
			score -= tmpGame.bestOpponentScore( limit - 1 );

			// Si le résultat est meilleur que ce que nous
			// avons trouvé jusqu'ici, prend note.
			if ( score > bestScore ){
				possibleMoves = new std::vector<int>();
				bestScore = score;
				possibleMoves->push_back( a );
				bestMove = a;
			} else if ( score == bestScore ){
				possibleMoves->push_back( a );
				bestMove = -1;
			}
		}
	}

	// Cas ou plusieures options se valent: tirage au sort
	if (bestMove == -1)
	{
		int r = std::rand() % possibleMoves->size();
		return possibleMoves->at( r );
	}

	if ( bestMove == -2 ){
		throw notAllowed;
	}

	// Si pas le choix, joue le meilleur mouvement (ou bien -2)
	return bestMove;
}

// Only valid assuming it's the opponent's turn to play
// use reverse() otherwise
bool Awele::gameIsOver(){
	if (opponent.sumSquares() == 0){
		return true;
	}
	if ( ! ( player.canPlay()|| opponent.canPlay() )){
		return true;
	}

	// TODO : spot   0 0 0 0 0 1 / 1 0 0 0 0 0 type of config
	if ( player.sumSquares() == 1 && opponent.sumSquares() == 1 ){
		if ( player.where() == opponent.where() ){
			return true;
		}
	}

	return false;
}

/** Cette méthode évalue le meilleur profit que l'adversaire peut
       tirer d'un plateau (on suppose que c'est au tour de l'adversaire 
       de jouer). Cette méthode récursive est très similaire à la
       méthode bestMove, mais renvoie le score et non le mouvement
       à effectuer.
       @param depthLimit limite à la récursivité. */
int Awele::bestOpponentScore( int depthLimit ){

	// faut-il continuer ?
	if (depthLimit < 0){
		return 0;
	}
	else{
		int a, maxScore=-5000, tmpScore=-5000;
		Awele tmpAwele;

		// Etudie toutes les possibilités parmi les
		// possibilités de jouer de l'adversaire
		for (a=5;a>=0;a--){
			if (opponent.canPlay(a)){
				tmpAwele = this->reverse(); // Creates a new instance
				tmpScore = tmpAwele.player.play(a);
				if (tmpAwele.gameIsOver()){
					tmpScore += player.sumSquares() - opponent.sumSquares() ;
				}else{
					tmpScore -= tmpAwele.bestOpponentScore( depthLimit - 1 );
				}
				if (tmpScore > maxScore){
					maxScore = tmpScore;
				}
			}
		}
		return (maxScore == -5000)?0:maxScore;
	}
}

/** Retourne une représentation écrite du plateau. */
std::string Awele::toString(){
	return opponent.reverse().toString() + "\n-----------\n" + player.toString() + "\n";
}

/** Fait jouer l'adversaire selon son meilleur profit possible. */
int Awele::letOpponentPlay(){
	int move;
	int diflev;

	diflev = difficulty;
	if (player.sumSquares()+player.sumSquares() < 24 ) { diflev *= 2; }
	if (player.sumSquares()+player.sumSquares() < 12 ) { diflev *= 2; }
	if (player.sumSquares()+player.sumSquares() < 6 ) { diflev *= 2; }

	move = this->reverse().bestMove(diflev);
	if(move >= 0 && move < 6){
		try {
			opponentPlays( move );
		}catch (enum Awele::Errors go){
			// TODO : do something ?
		}
	}

	return move;
}




/** Constucteur permettant d'initialiser le jeu.
       @param fill Répartition initiale des graines
       (par exemple {@link Awele#initialState initialState}). 
       @param opp Demi-plateau situé en face. */
HalfBoard::HalfBoard( const int *fill, HalfBoard *opp ) {
	int a;
	for (a = 0 ; a < 6 ; ++a ){
		origin[a]=false;
		seeds[a]=fill[a];
	}
	opposite = opp;
}

void HalfBoard::reset(){
	std::memcpy( seeds, initialState, sizeof(seeds) );
	seedsTaken = 0;
}

HalfBoard::HalfBoard( const HalfBoard *c)
{
	int a;
	for (a = 0 ; a < 6 ; ++a ){
		origin[a]=false;
		seeds[a] = c->seeds[a];
	}
}


/** Ce constructeur est utilisé lorsque l'on ne connait pas encore
       le demi-plateau en face. Il est nécéssaire de mettre à jour cette
       valeur par la suite (par accès direct à {@link #opposite opposite}). */
HalfBoard::HalfBoard( const int *fill ) : HalfBoard::HalfBoard( fill, NULL ){
}

/** Retourne le nombre total de graines dans les cases. */
int HalfBoard::sumSquares(){
	int a, tot;

	for (a=0, tot=0 ; a < 6 ; a++){
		tot += seeds[a];
	}
	return tot;
}

/** Représentation en texte des cases. Retourne une chaîne contenant
       le nombre de graines dans chaque case. */
std::string HalfBoard::toString(){
	char buffer[32];

	std::sprintf( buffer, "%d %d %d %d %d %d", seeds[0], seeds[1], seeds[2], seeds[3], seeds[4], seeds[5] );
	std::string *str = new std::string( buffer );
	return *str;
}

/** Cette méthode retourne un demi-plateau. Ceci n'a de sens que pour
       représenter le jeu vu d'en face. */
HalfBoard HalfBoard::reverse(){
	int reversed[6];
	int a;
	for (a=0;a<6;a++)
		reversed[a]=seeds[5-a];

	HalfBoard hb( reversed );

	return hb;
}

/** Cette méthode met en place le témoin de départ du jeu: la case
       jouée est marquée de sorte qu'aucune graine n'y soit jouée si
       le nombre de graines jouées implique un tour complet. */
void HalfBoard::setOrigin(int sq){
	int a;

	for (a = 0; a<6 ; a++ ){
		origin[a]=false;
		opposite->origin[a]=false;
	}
	if ( sq >= 0 && sq < 6 )
		origin[sq]=true;

	winHere = false;
	opposite->winHere = true;
}

/** Joue une des cases. Celle-ci est vidée, et les graines
       sont réparties une par une dans les cases suivantes. Si
       il reste des graines une fois arrivé à la dernière case,
       celles-ci sont transmises au demi-plateau en face par 
       l'invocation de la méthode {@link #handOver handOver}.
       @param at Position de la case à jouer. */
int HalfBoard::play(int at){
	int seedsTaken;
	int score = 0;

	setOrigin(at);
	seedsTaken = seeds[at];

	seeds[at]=0;

	for (at = at+1;at<6 && seedsTaken > 0;at++)
	{   seeds[at]++;seedsTaken--;}

	if (seedsTaken > 0){
		score = opposite->handOver(seedsTaken);
	}
	return score;
}

/** Teste une case afin de savoir si une case peut être jouée.
       Il faut que la case contienne des graines mais aussi que 
       le jeu de la case ne mette pas l'adversaire en position 
       de famine.
       @param at Numéro de la case à tester. */
bool HalfBoard::canPlay( int at ){
	// No seeds to play
	if (seeds[at]==0){
		return false;
	}
	// If opponent has no seeds, we have to give some
	else {
		Awele test(this, opposite);
		test.player.play(at);
		return (test.opponent.sumSquares()==0)?false:true;
	}
}

bool HalfBoard::canPlay( ){
	int a;
	for (a=0;a<6;a++){
		if (canPlay(a)){
			return true;
		}
	}
	return false;
}

int HalfBoard::where( ){
	int a;
	for (a=0;a<6;a++){
		if (seeds[a]>0){
			return a;
		}
	}
	return -1;
}

/** Cette méthode est exécutée lorsque des graines parviennent
       du plateau en face pour être réparties. On teste ici la
       prise de graines dans le cas où le demi-plateau qui recoit
       les graines est celui de l'adversaire (flag {link #winHere winHere}).
       @param numOfSeeds Le nombre de graines recues. */
int HalfBoard::handOver( int numOfSeeds ){
	int a;int won = 0;

	for (a=0;a<6 && numOfSeeds > 0;a++){
		// Step over originating square
		if (origin[a] == false){
			seeds[a]++;numOfSeeds--;
		}

		if (numOfSeeds == 0 && winHere){
			while (a>=0){
				if (seeds[a]==2 || seeds[a]==3)
				{
					won+=seeds[a];
					seeds[a]=0;
					a--;
				}
				else{
					opposite->seedsTaken += won;
					return won;
				}
			}
			opposite->seedsTaken += won;
			return won;
		}
	}

	if (numOfSeeds > 0)
		return opposite->handOver( numOfSeeds );
	else return 0;
}

