#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define HIGH 6
#define LOW 1

// hold player dice values
char player[5];

// hold reroll dice
char reroll[20];

// user response
char response;

/* function to print our the players dice */
void print_player(char *array, int length)
{
	int i;
	for (i = 0; i < 5; i++) {
		printf("%d ", array[i]);
	}
}


/* function to check if the reroll array contains a zero */
int check_for_zero(char *array, int length) 
{
	int i;
	for (i = 0; i < length; i++) {
		if (array[i] == '0') 
		{
			return 1;
		}
	}
	return 0;
}

/* function to create random dice rolls */
char rand_roll()
{
	char random;
	int fd = open ("/dev/dice",O_RDONLY );

	if (fd < 1)
	{
		printf("Error with /dev/dice");
	}
	read(fd, &random, sizeof(char));
	close(fd);
	return random;	
	//return rand() % (HIGH - LOW + 1) + LOW;
}

int cmpfunc (const void *a, const void *b) 
{
	return *(char*)a - *(char*)b;
}

/* function to score the upper section by summing *
 * the number of like values                      */
int upper_count(char *array, int n, int t)
{
	int total = 0;
	int i;
	for (i = 0; i < 5; i++) 
	{
		if (array[i] == n) 
		{
			total += t;
		}
	}
	return total;
}

/* function to score the three of a kind in the lower section */
int three_kind(char *array) 
{
	int i;
	int cnt = 0;
	int value = array[0];
	for (i = 0; i < 2; i++) 
	{
		if (array[i] == array[i+1]) {cnt++;}
	}
	if (cnt == 2) { 
		return (value*3); 
	} else return 0;
}

/* function to score the four of a kind in the lower section */
int four_kind(char *array) 
{
	int i;
	int cnt = 0;
	int value = array[0];
	for (i = 0; i < 3; i++) 
	{
		if (array[i] == array[i+1]) {cnt++;}
	}
	if (cnt == 3) { 
		return (value*3); 
	} else return 0;
}

/* function to score the small straight in the lower section */
int small_str(char *array) 
{
	int i;
	int cnt = 0;
	for (i = 0; i < 3; i++) 
	{
		if (array[i] < array[i+1]) {cnt++;}
	}
	if (cnt == 3) { 
		return 30; 
	} else return 0;
}

/* function to score the large straight in the lower section */
int large_str(char *array) 
{
	int i;
	int cnt = 0;
	for (i = 0; i < 4; i++) 
	{
		if (array[i] > array[i+1]) {cnt++;}
	}
	if (cnt == 4) { 
		return 40; 
	} else return 0;
}

/* function to score the full house in the lower section */
int full_house(char *array)
{
	return 25;
}

/* function to score the yahtzee in the lower section */
int yahtzee(char *array) 
{
	int i;
	int cnt = 0;
	for (i = 0; i < 5; i++)
	{
		if (array[i] == array[i+1]) {cnt++;}
	}
	if (cnt == 5) return 50;
}

/* function to score the chance of a kind in the lower section */
int chance(char *array)
{
    int total;
    int i;
    total = 0;
    for (i = 0 ; i < 5 ; ++i)
    {
    	int x = array[i];
    	total += x;
    }
    return total;
}

int main ()
{
	// individual score tracker
	int total_score = 0;

	// Score handlers for upper and lower sections
	typedef struct upper {
		int one;
		int two;
		int three;
		int four;
		int five;
		int six;
		int bonus;
	} Upper;

	typedef struct lower {
		int three_kind;
		int four_kind;
		int full_house;
		int small_str;
		int large_str;
		int yahtzee;
		int chance;
	} Lower;

	Upper upper_score = {0,0,0,0,0,0,0};
	Lower lower_score = {0,0,0,0,0,0,0};

	/* Declaration and seed srand() function */
	// int t = 3;
	// srand(t);

	/* tracks the number of turns the player has completed */
	int game_play = 0;
	
	printf("\nWelcome to Single player Yahtzee!\n");
	printf("Please enjoy your game. Good Luck!\n");

	/* do while loop to simulate game play */
	do {

	printf("\nYour roll:\n\n");

	int i;
	for (i = 0; i < 5; i++) {
		player[i] = rand_roll();
	}
	print_player(player, sizeof(player)/sizeof(player[0]));

	
	printf("\nWhich dice to reroll:  ");
   	fgets(reroll, 20, stdin);

   	/* Loop that prompts the user for rerolls */
   	int try = 0;
   	while (try < 2 && check_for_zero(reroll, 12) == 0)
   	{

   		for (i = 0; i < 12; i++) {
			if (reroll[i] == '1') 
			{
				player[0] = rand_roll();
			} 
			else if (reroll[i] == '2') 
			{
				player[1] = rand_roll();
			} 
			else if (reroll[i] == '3') 
			{
				player[2] = rand_roll();
			}
			else if (reroll[i] == '4') 
			{
				player[3] = rand_roll();
			} 
			else if (reroll[i] == '5')
			{
				player[4] = rand_roll();				
			} 
		}
		// flushing input for stdin
		char ch;
		while ((ch = getchar()) != '\n' && ch != EOF);

		try++;
		printf("\nYour reroll: \n\n");
		print_player(player, sizeof(player)/sizeof(player[0]));

		printf("\nWhich dice to reroll:  ");
   		fgets(reroll, 20, stdin);
	}

	qsort(player, 5, sizeof(char), cmpfunc);

		
	printf("Place dice into:\n1) Upper Section\n2) Lower Section\n\nSelection? ");
	scanf("%s", &response);

	if(response == '1') 
	{
		printf("\nPlace dice into:\n1) Ones\n2) Twos\n");
		printf("3) Threes\n4) Fours\n5) Fives\n6) Sixes\n");
		printf("\nSelection: ");
		scanf("%s", &response);

		if (response == '1')
		{
			int t = upper_count(player, 1, 1);
			upper_score.one = t;
			total_score += t;

		}
		else if (response == '2')
		{
			int t = upper_count(player, 2, 2);
			upper_score.two = t;
			total_score += t;

		}
		else if (response == '3')
		{
			int t = upper_count(player, 3, 3);
			upper_score.three = t;
			total_score += t;
		}
		else if (response == '4')
		{
			int t = upper_count(player, 4, 4);
			upper_score.four = t;
			total_score += t;
		}
		else if (response == '5')
		{
			int t = upper_count(player, 5, 5);
			upper_score.five = t;
			total_score += t;
		}
		else if (response == '6')
		{
			int t = upper_count(player, 6, 6);
			upper_score.six = t;
			total_score += t;
		}
	} 
	else if (response == '2') 
	{
		printf("\nPlace dice into: \n1) Three of a Kind: \n2) Four of a Kind: \n3) Small Straight:");
        printf("\n4) Large Straight: \n5) Full House: \n6) Yahtzee: \n7) Chance: \n");
        printf("\nSelection: ");		
		scanf("%s", &response);

		if (response == '1')
		{
			int t = three_kind(player);
			lower_score.three_kind = t;
			total_score += t;
		}
		else if (response == '2')
		{
			int t = four_kind(player);
			lower_score.four_kind = t;
			total_score += t;
		}
		else if (response == '3')
		{
			int t = small_str(player);
			lower_score.small_str = t;
			total_score += t;
		}
		else if (response == '4')
		{
			int t = large_str(player);
			lower_score.large_str = t;
			total_score += t;
		}
		else if (response == '5')
		{
			int t = full_house(player);
			lower_score.full_house = t;
			total_score += t;
		}
		else if (response == '6')
		{
			int t = yahtzee(player);
			lower_score.yahtzee = t;
			total_score += t;
		}
		else if (response == '7')
		{
			int t = chance(player);
			lower_score.chance = t;
			total_score += t;
		}
	} else printf("DIDNT MAKE IT\n");

	// check for bonus points
	if ((upper_score.one + upper_score.two + upper_score.three + upper_score.four + upper_score.five + upper_score.six) > 63)
	{
		upper_score.bonus = 35;
	}

	// print total score 
	printf("Your total score is %d\n\n", total_score);
	printf("Ones: %d\t\t\t\tFours: %d\nTwos: %d\t\t\t\tFives: %d\nThrees: %d\t\t\tSixes: %d\nUpper Section Bonus: \n", upper_score.one, upper_score.four, upper_score.two, upper_score.five, upper_score.three, upper_score.six, upper_score.bonus);
	printf("Three of a Kind: %d\t\tFour of a Kind: %d\nSmall Straight: %d\t\tLarge Straight: %d\n", lower_score.three_kind, lower_score.four_kind, lower_score.small_str, lower_score.large_str);
	printf("Full House: %d\t\t\tYahtzee: %d\nChance: %d\n\n\n", lower_score.full_house, lower_score.yahtzee);

	game_play++;
	} while (game_play <= 13);

	printf("Thanks for playing Single Player Yahtzee!\n");

	return 0;
}
