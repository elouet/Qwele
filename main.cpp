#include "Awele.h"
#include <iostream>


int main( int argc, char **argv )
{
	const int meseeds[] = {1, 0, 0, 2, 2, 1};
	const int himseeds[] = {0, 0, 0, 0, 0, 2};

 	HalfBoard me( meseeds );
	HalfBoard him( himseeds );

	Awele game( &me, &him ); 

	game.setDifficultyLevel(8);

	std::cout << game.toString();

	int play;

	do {
		std::cout << "enter move : " << std::endl;

		try {
			std::cin >> play;

			game.play(play);
			std::cout << game.toString();

			std::cout << std::endl << "thinking..."<< std::endl << std::endl;
			game.letOpponentPlay();

			std::cout << game.toString();
		}catch (enum Awele::Errors ao){
			switch (ao){
			case Awele::notAllowed:
				std::cout << "Not allowed !"<< std::endl;
				break;
			case Awele::gameOver:
				std::cout << "Game over !"<< std::endl;
				return 0;
			}
		}
	} while ( ! game.reverse().gameIsOver() );

}
