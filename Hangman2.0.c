#include<stdio.h>
#include<math.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include<string.h>
#include <windows.h>


#define MAX_WORD 5
#define MAX_USERS 10
#define GUESSES 7


int NUMBER_OF_USERS=0;
char DASHES[10];
int Strike=0;
int CorrectGusses=0;
int currentScore=0;
int matches_played=0;
int initflag = 1;

//word structure
typedef struct{
    char word[10];
    char clue[100];

}Word;
//game status structure
typedef struct {
    Word word;
    char guessed_letters[10];
    int wrong_attempts;
    int CorrectGusses;
} Game;
//user structure
typedef struct {
    char name[50];
    int id;
    int matches_played;
    int scores;
    int isGameSaved;
    int rank;
    Game saved_game;
} User;

//data loading functions
void loadWords();
void loadUsers();
//void loadLeaderboard();
//data saving functions

void saveUsers();
//void saveLeaderboard();

//functions
void clearscr()				//A function to clear the terminal/cmd window.
{
    system("@cls||clear");
}
void AddUsers(char username[50]);
void EditUser();
void displayLeaderboard();
void updateLeaderboard();
void About();
int login();
int mainMenu();
int newgame();
int resumegame();
int isExistingUser();
void saveCurrentUser();
//void getUser();
void setUser(int i,
    char name[50],
    int matches_played,
    int isGameSaved,
    int rank);
void drawHangman(int strike);
void drawTitle();
void drawgameOver();
void drawYouWon();
int init();
int game();


///////////////////////////////////////////////ARRAYS//////////////////////////////

//array of users
User userArray[MAX_USERS];
//array of words
Word wordList[MAX_WORD];
//Leaderboard array
User Leaderboard[MAX_USERS];
//Current user structure
User currentUser;
//Choosen word structure
Word choosenWord;


///////////////////////////////////////////////MAIN FUNCTION////////////////////////
int main(){
    int flag2;
    if(initflag==1){
        printf("Envery thing seems ok\n");
        loadUsers();
        loadWords();
        init();
        initflag+=1;
        login();
    }

    printf("\n\ninit flag1 = %d \n",flag2);

    flag2 = mainMenu();

    switch (flag2)
    {
    case 1:
        newgame();
        break;
    case 2:
        resumegame();
        break;
    case 3:
        displayLeaderboard();
        main();
        break;
    case 4:
        EditUser();
        main();
        break;
    case 5:
        About();
        main();
        break;
    case 6:
        initflag=1;
        saveUsers();
        main();
        break;
    case 7:

        break;
    default:
        break;
    }
    saveUsers();
}

//data loading function definations
//function written by SOWAD
void loadWords(){
    FILE *file;
    file = fopen(".\\data_files\\words.dat", "r");
    int fread_result = fread(wordList,sizeof(Word),MAX_WORD,file);

    if(fread_result==MAX_WORD)
        printf("\nWords copied succesfully\n");
    else
        printf("\nerror copying word file\n");
    fclose(file);
}
//function written by SOWAD
void loadUsers(){
    FILE *file;
    file = fopen(".\\data_files\\users_data.dat", "r");
    int fread_result = fread(userArray, sizeof(User),MAX_USERS,file);
    if(fread_result==MAX_USERS)
        printf("\nUSERS copied succesfully\n");

    else{
        fclose(file);
        file = fopen(".\\data_files\\users_data.dat", "w");
        saveUsers();
        loadUsers();
    }
    fclose(file);
}

//init function
int init(){

    for(int i =0;i<MAX_USERS;i++){
        if(userArray[i].id +1 != userArray[i+1].id)
            break;
        NUMBER_OF_USERS = userArray[i+1].id;
    }
}
//login function
int login(){
    clearscr();
    //starting animation
    for(int i = 0; i <= 7;i++){
        fflush(stdout);
        drawTitle();
        drawHangman(i);
        Sleep(500);//time in ms
        clearscr();
    }

    drawTitle();
    printf("Welcome\n");
    printf("1.Existing User\n");
    printf("2.New user\n");
    int temp;
    printf("\nChoose an option: ");
    fflush(stdin);
    scanf(" %d",&temp);
    char str[20];
    switch (temp)
    {
    case 1:
        printf("\nUsername:");
        scanf("%s", str);
        if(isExistingUser(str) == 0){
            clearscr();
            printf("\nUSERNAME DOSE NOT EXIST.\n");
            sleep(3);
            login();
            }
        break;
    case 2:
        printf("Username:");
        scanf("%s", str);
        AddUsers(str);
        break;
    default:
        clearscr();
        printf("\nINVALID INPUT\n");
        sleep(3);
        login();
        break;
    }
    //current user
    for (int i = 0; i < MAX_USERS; i++){
        int d = strcmp(str, userArray[i].name);
        if (d == 0){
            currentUser = userArray[i];
            break;
        }
    }
    return 1;
}

//function written by FARHAN
int mainMenu(){
    //updateing leaderboard
    saveCurrentUser();
    //updateLeaderboard();
    clearscr();
    drawTitle();
    int i, n;
    printf("Main Menu\n");
    printf("1.New Game:\n");
    printf("2.Resume Game:\n");
    printf("3.Leader board:\n");
    printf("4.Edit Username:\n");
    printf("5.About:\n");
    printf("6.Logout\n");
    printf("7.Exit:\n");

    do
    {
        printf("\nChose an option: ");
        scanf("%d", &n);
    } while (n < 1 || n > 7);
    return n;
}

//newgame function
int newgame(){
    fflush(stdin);
    //chooseWord();
    chooseWord();
    Strike = 0;
    CorrectGusses = 0;
    currentUser.isGameSaved=0;
    game();
}

//resume game function
int resumegame(){
    printf("\n\n%d\n",currentUser.isGameSaved);
    if(currentUser.isGameSaved == 1){
        choosenWord = currentUser.saved_game.word;
        Strike = currentUser.saved_game.wrong_attempts;
        CorrectGusses = currentUser.saved_game.CorrectGusses;
        strcpy(DASHES,currentUser.saved_game.guessed_letters);
        game();
        return 0;
    }
    else{
        clearscr();
        drawTitle();
        printf("\n\nnSAVED GAME NOT FOUND\nReturning to MAIN MENU\n");
        sleep(2);
        main();
    }
}



//game
int game(){
    clearscr();
    char tempchar;
    int letter_exist=0;
    char chosen_word[20],chosen_wordHint[100];
    strcpy(chosen_word,choosenWord.word);
    strcpy(chosen_wordHint,choosenWord.clue);
    int word_lenth = strlen(chosen_word);

    if(currentUser.isGameSaved==0){
        for(int i =0;i<word_lenth;i++)
            DASHES[i]='*';

        DASHES[word_lenth]='\0';
    }

    while(1){
        clearscr();
        drawTitle();
//Game own
        if(CorrectGusses>=word_lenth){
            printf("\nTHE WORD WAS:%s\n\n",chosen_word);
            printf("YOUR SCORE WAS: %d\n",currentScore);
            drawYouWon();
            currentUser.isGameSaved = 0;
            break;
        }

        drawHangman(Strike);
//Game over
        if(Strike == GUESSES){
            printf("\nTHE WORD WAS:%s\n\n",chosen_word);
            printf("YOUR SCORE WAS: %d\n",currentScore);
            drawgameOver();
            currentUser.isGameSaved = 0;
            break;
        }

        printf("\n\nGUESS THE WORD:%s",DASHES);
        printf("\n\nHINT:%s",chosen_wordHint);
        printf("\n~Enter '*' to save and return to main menu\n");
    //taking input form the user
        printf("\n\nGuess a letter: ");
        fflush(stdin);
        scanf("%c",&tempchar);

        if(tempchar=='*'){
            clearscr();
            drawTitle();
            printf("\nQuit the game?\n");
            printf("\n1.Yes\t2.No\n->");
            int tempquit;
            scanf("%d",&tempquit);
            if(tempquit == 2)
                continue;
            else{
                clearscr();
                drawTitle();
                printf("\nSave Game state?\n");
                printf("\n1.Yes\n2.No\n->");
                int tempexit;
                fflush(stdin);
                scanf("%d",&tempexit);
                if(tempexit==1){
                    currentUser.isGameSaved = 1;
                    currentUser.saved_game.word = choosenWord;
                    currentUser.saved_game.wrong_attempts = Strike;
                    currentUser.saved_game.CorrectGusses = CorrectGusses;
                    strcpy(currentUser.saved_game.guessed_letters ,DASHES);
                    clearscr();
                    drawTitle();
                    printf("\nGame saved at this point\n");
                    break;
                }
                else
                    break;
            }
        }

        for(int i =0;i<word_lenth;i++){
            if(toupper(chosen_word[i])==toupper(tempchar)){
                DASHES[i]=chosen_word[i];
                letter_exist+=1;
                CorrectGusses+=1;
            }
        }

        if(letter_exist==0){
            Strike++;
        }

        letter_exist =0;
    }
    currentUser.matches_played++;
    currentScore = 10*CorrectGusses-6*Strike;
    currentUser.scores += currentScore;
    saveCurrentUser();
    sleep(3);
    clearscr();
    printf("\nReturning to main menu\n");
    sleep(2);
    main();

}

void About(){
    clearscr();
    drawTitle();
    FILE *file;
    char c;

    file = fopen("about.txt","r");


    if(file == NULL)
    {
        printf("Error in opening file");
        return(-1);
    }
    while(1)
    {
        c = fgetc(file);
        if( feof(file) )
            break;
        printf("%c", c);
    }
    fclose(file);
    printf("\n\n\n\n\nEnter a charecter to exit: ");
    scanf(" %c");
}

//function written by AHNAF
void displayLeaderboard()
{
    updateLeaderboard();
    printf("===== LEADERBOARD =====\n");
    for (int i = 0; i < MAX_USERS; i++)
    {
        printf("Number %d. %s: %d\n", i + 1, Leaderboard[i].name, Leaderboard[i].scores);
    }
    printf("\n\n\n\nEnter any charecter to return to main menu: ");
    scanf(" %c");
}

void updateLeaderboard(){
    User temp;
    for(int i = 0;i<MAX_USERS;i++){
        Leaderboard[i] = userArray[i];
    }
    for(int j =0;j<MAX_USERS;j++){
        for(int i = 0;i<MAX_USERS-1;i++){
            if(Leaderboard[i].scores<Leaderboard[i+1].scores){
                temp = Leaderboard[i];
                Leaderboard[i]=Leaderboard[i+1];
                Leaderboard[i+1]=temp;
            }
            else
                continue;
        }
    }
    for(int i =0;i<NUMBER_OF_USERS;i++){
        for(int j =0;j<NUMBER_OF_USERS;j++){
            if(strcmp(Leaderboard[i].name,userArray[j].name)==0){
                userArray[j].rank = i+1;
            }
        }
    }
}


//function written by FARHAN

int isExistingUser(char username[20]){
    for (int i = 0; i < MAX_USERS; i++){
        int d = strcmp(username, userArray[i].name);
        if (d == 0)
            return 1;
    }
    return 0;
}

//function written by sowad
void EditUser(){
    clearscr();
    drawTitle();
    char str[10];
    char flag;
    printf("\nCHANGE USERNAME?\n");
    printf("\n1.YES\t2.NO\n->");
    fflush(stdin);
    scanf("%c",&flag);
    if(flag=='2')
        main();
    else{
        for(int i = 0;i<=NUMBER_OF_USERS;i++){
            if(strcmp(currentUser.name,userArray[i].name)==0){
                clearscr();
                drawTitle();
                printf("\nYOUR USER NAME:%s",currentUser.name);
                printf("\nYOUR ID:%d",currentUser.id);
                printf("\nMATCHES PLAYED:%d",currentUser.matches_played);
                printf("\nYOUR RANK:%d",currentUser.rank);
                printf("\nSAVED GAME STATUS:%d",currentUser.isGameSaved);
                printf("\nYOUR NEW USER NAME:");
                fflush(stdin);
                scanf("%s",str);
                if(isExistingUser(str) == 0){
                    strcpy(currentUser.name,str);
                    strcpy(userArray[i].name,str);
                    printf("\nYOUR USER NAME:%s",currentUser.name);
                    printf("\nYOUR ID:%d",currentUser.id);
                    printf("\nMATCHES PLAYED:%d",currentUser.matches_played);
                    printf("\nYOUR RANK:%d",currentUser.rank);
                    printf("\nSAVED GAME STATUS:%d",currentUser.isGameSaved);
                    sleep(3);
                }
                else{
                    printf("\nUSERNAME EXISTS. USE A DIFFRENT USERNAME\n");
                    sleep(3);
                    EditUser();
                }
            }
    }
    }
}

void AddUsers(char username[50]){
    for(int i =0;i<MAX_USERS;i++){
        if(userArray[i].id +1 == userArray[i+1].id)
            NUMBER_OF_USERS = userArray[i+1].id;
        else
            break;
    }
    setUser(NUMBER_OF_USERS,username,0,0,0);
}

void setUser(int i,
    char name[50],
    int matches_played,
    int isGameSaved,
    int rank){
        strcpy(userArray[i].name,name);
        userArray[i].id = NUMBER_OF_USERS+1;
        userArray[i].matches_played = matches_played;
        userArray[i].isGameSaved = isGameSaved;
        userArray[i].rank = rank;
        NUMBER_OF_USERS++;
    }

void chooseWord(){
    //choosing random word from the word list
    srand((unsigned) time(NULL));
    int word_index = (rand() % (MAX_WORD));
    choosenWord = wordList[word_index];
}


void saveCurrentUser(){
    for (int i = 0; i < MAX_USERS; i++){
        int d = strcmp(currentUser.name, userArray[i].name);
        if (d == 0){
            userArray[i] = currentUser;
            break;
        }
    }
}

//draw function
void drawHangman(int strike){
    switch (strike)
    {
    case 0:
        printf("  +---+\n      |\n      |\n      |\n      |\n      |\n=========");
        break;
    case 1:
        printf("  +---+\n  |   |\n      |\n      |\n      |\n      |\n=========");
        break;
    case 2:
        printf("  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n=========");
        break;
    case 3:
        printf( "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n=========");
        break;
    case 4:
        printf("  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n========="    );
        break;
    case 5:
        printf("  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n=========");
        break;
    case 6:
        printf("  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n=========");
        break;
    case 7:
        printf("  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n=========");
        break;
    }
}

void drawTitle(){
    printf(" _                                             \n");
    printf("| |                                            \n");
    printf("| |__   __ _ _ __   __ _ _ __ ___   __ _ _ __  \n");
    printf("| '_ \\ / _` | '_ \\ / _` | '_ ` _ \\ / _` | '_ \\ \n");
    printf("| | | | (_| | | | | (_| | | | | | | (_| | | | | \n");
    printf("|_| |_|\\__,_|_| |_|\\__, |_| |_| |_|\\__,_|_| |_| \n");
    printf("                    __/ |                      \n");
    printf("                   |___/  \n");
}


void drawgameOver(){
printf(" _______  _______  _______  _______    _______           _______  _______ \n");
printf("(  ____ \\(  ___  )(       )(  ____ \\  (  ___  )|\\     /|(  ____ \\(  ____ )\n");
printf("| (    \\/| (   ) || () () || (    \\/  | (   ) || )   ( || (    \\/| (    )|\n");
printf("| |      | (___) || || || || (__      | |   | || |   | || (__    | (____)|\n");
printf("| | ____ |  ___  || |(_)| ||  __)     | |   | |( (   ) )|  __)   |     __)\n");
printf("| | \\_  )| (   ) || |   | || (        | |   | | \\ \\_/ / | (      | (\\ (\n");
printf("| (___) || )   ( || )   ( || (____/\\  | (___) |  \\   /  | (____/\\| ) \\ \\__\n");
printf("(_______)|/     \\||/     \\|(_______/  (_______)   \\_/   (_______/|/   \\__/\n");
}

void drawYouWon(){
printf("");
printf("                                                                       __  __\n");
printf("                                                                      /  |/  |\n");
printf(" __    __   ______   __    __        __   __   __   ______   _______  $$ |$$ |\n");
printf("/  |  /  | /      \\ /  |  /  |      /  | /  | /  | /      \\ /       \\ $$ |$$ |\n");
printf("$$ |  $$ |/$$$$$$  |$$ |  $$ |      $$ | $$ | $$ |/$$$$$$  |$$$$$$$  |$$ |$$ |\n");
printf("$$ |  $$ |$$ |  $$ |$$ |  $$ |      $$ | $$ | $$ |$$ |  $$ |$$ |  $$ |$$/ $$/\n");
printf("$$ \\__$$ |$$ \\__$$ |$$ \\__$$ |      $$ \\_$$ \\_$$ |$$ \\__$$ |$$ |  $$ | __  __\n");
printf("$$    $$ |$$    $$/ $$    $$/       $$   $$   $$/ $$    $$/ $$ |  $$ |/  |/  |\n");
printf(" $$$$$$$ | $$$$$$/   $$$$$$/         $$$$$/$$$$/   $$$$$$/  $$/   $$/ $$/ $$/\n");
printf("/  \\__$$ |\n");
printf("$$    $$/\n");
printf(" $$$$$$/\n");
}
//data saving functions

void saveUsers(){
    clearscr();
    saveCurrentUser();
    FILE *file;
    file = fopen(".\\data_files\\users_data.dat", "w");
    int fwriteflag = fwrite(userArray,sizeof(User),MAX_USERS,file);
    if(fwriteflag==MAX_USERS)
        printf("\nUSERS saved succesfully\n");
    else
        printf("\nerror saving users file\n");
    fclose(file);

}
