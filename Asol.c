//Date:  25Aug2017Fri 21:31:01
//File:  Asol.c
//Email: ashraf.allie01@gmail.com
/*Desc:  A card game also known as President or Daifugo or Daihinmin




    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


//----------------
//Macro Defintions
//----------------
#include <stdlib.h>
#include <ncurses.h>
//#include <time.h>

#define ESCAPE_KEY     27
#define ENTER_KEY      13
#define TAB_KEY         9
#define SPACE_BAR_KEY  32


//-------------------------------------------------
//Data Structure Declarations (struct, enum, union)
//-------------------------------------------------
enum SubWindowType {Main, Menu};
enum MainSubWindowType {SplashScreen, HowToPlay, NewGame, PickingPhase,
                        SwappingPhase, PlayingPhase, HighScore, GameSettings};
enum PlayerRankType {Asol, Secretary, VicePresident, President, Unset};


//----------------------------
//Global Variable Declarations
//----------------------------
WINDOW *MainSubWin, *MenuSubWin, *InfoSubWin, *StatusSubWin;

enum SubWindowType CurrentSubWindow = Main;
enum MainSubWindowType CurrentMainSubWindow = SplashScreen;
enum PlayerRankType PlayerRank[4] = {Asol, Secretary, VicePresident, President},
                    NewPlayerRank[4] = {Unset, Unset, Unset, Unset},
                    CurrentPlayer;

char Player_Name[11];
char *Players_Names[4];
char ACard[4] = {'\0', '\0', '\0', '\0'};

unsigned char Screen_Rows,
              Screen_Cols,
              fp_row = 0,
              HighlightedMenuOption = 1,
              Card_Row = 2,
              Card_Col = 0,
              Card_Index = 0,
              Card_List[54],
              PlayersCardIndexChosen[4],
              PlayersLastCards[4],
              repick = 1,
              CardsRemaining = 52,
              InGame = 0,
              CardSelectionAmount = 0;


//----------------------------------
//Function Declarations (prototypes)
//----------------------------------
void  Splash_Screen(unsigned char InGameSplashScreen);
void  Ncurses_Initialization(void);
void  Term_Screen_Size_Detection(void);
void  Screen_Border_Setup(void);
void  Screen_SubWindow_Setup(void);
void  Display_Menu(unsigned char Option);
void  Choice_Selection(void);
void  Status_Window(void);
void  Info_Window(unsigned char Msg);
void  How_To_Play(unsigned char fp_row_arg);
void  New_Game(unsigned short int KeyPressed);
char* Card(unsigned char CardIndex);
void  Picking_Phase(unsigned short int KeyPressed);
void  Card_Sort(unsigned char index, unsigned char index2);
void  Swapping_Phase(void);
void  Playing_Phase(unsigned short int KeyPressed);
void  Load_Game(void);
void  Save_Game(void);
void  High_Score(void);
void  Game_Settings(void);
void  Delete_SubWindows(void);
void  Ncurses_End(void);


//------------
//Main Program
//------------
int main(void)
{
 sprintf(Player_Name, "__________");
 //srand(time(NULL));

 Splash_Screen(0);
 Ncurses_Initialization();
 Term_Screen_Size_Detection();
 Screen_Border_Setup();
 Screen_SubWindow_Setup();
 Status_Window();
 Info_Window(0);
 Display_Menu(HighlightedMenuOption);
 Splash_Screen(1);
 Choice_Selection();
 Ncurses_End();

 puts("\nCard_List[]:\n");
 for (Card_Index = 0; Card_Index <= 53; Card_Index++)
 {
  printf("%s ", Card(Card_List[Card_Index]));
  if (Card_Index == 13 || Card_Index == 27 || Card_Index == 40 || Card_Index == 53)
    putchar('\n');
 }

 return 0;
}


//-------------------------------------
//Function Definitions (Implementation)
//-------------------------------------


//FUNCTION: Splash_Screen
void Splash_Screen(unsigned char InGameSplashScreen)
{
 const char *version = "v0.6.18";

 if (InGameSplashScreen == 0)
 {
  printf(
  "Asol\n"
  "A card game also known as President or Daifugo or Daihinmin\n\n"
  "Created by Ashraf\n"
  "ashraf.allie01@gmail.com\n"
  "%s\n"
  "Copyleft 2017\n\n",
  version);
 }
 else if (InGameSplashScreen == 1)
 {
  mvwprintw(MainSubWin,1,7,     "-            -----            ---        |");
  mvwprintw(MainSubWin,2,6,    "/ \\         /       \\        /     \\      |");
  mvwprintw(MainSubWin,3,5,   "/   \\        \\                             |");
  mvwprintw(MainSubWin,4,4,  "/     \\         -----         |       |     |");
  mvwprintw(MainSubWin,5,3, "/-------\\              \\                     |");
  mvwprintw(MainSubWin,6,2,"/         \\     \\       /        \\     /      |");
  mvwprintw(MainSubWin,7,1,"/           \\      -----            ---         ----------");

  mvwprintw(MainSubWin, 10, 22, "Created by Ashraf");
  mvwprintw(MainSubWin, 12, 19, "ashraf.allie01@gmail.com");
  mvwprintw(MainSubWin, 14, 24, "Copyleft 2017");
  mvwprintw(MainSubWin, 16, 23, "version %s", version);
 }
}


//FUNCTION: Ncurses_Initialization
void Ncurses_Initialization(void)
{
 initscr();
 start_color();
 cbreak();
 noecho();
 nonl();
 intrflush(stdscr, TRUE);
 keypad(stdscr, TRUE);


 init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
 init_pair(2, COLOR_YELLOW,  COLOR_BLACK);
 init_pair(3, COLOR_CYAN,    COLOR_BLACK);
 init_pair(4, COLOR_GREEN,   COLOR_BLACK);
 init_pair(5, COLOR_RED,     COLOR_BLACK);
}


//FUNCTION: Term_Screen_Size_Detection
void Term_Screen_Size_Detection(void)
{
 //if ((getmaxy(stdscr) <= 24) || (getmaxx(stdscr) <= 80))
 if ((getmaxy(stdscr) != 25) || (getmaxx(stdscr) != 81))
 {
  endwin();
  puts("\n");
  system("resize -s 25 81");
  refresh();
 }

 if ((getmaxy(stdscr) != 25) || (getmaxx(stdscr) != 81))
 {
  printw(//"Window size too small\nPlease resize\n"
         //"Your screen size is:\n"
         "Resize screen to 25 rows and 81 columns\n"
         "Screen_Rows = %d\n"
         "Screen_Cols = %d\n\n",
         //"Screen rows must be greater than 24\n"
         //"Screen columns must be greater than 80\n\n",
         getmaxy(stdscr), getmaxx(stdscr));

  getch();
  endwin();
  exit(EXIT_FAILURE);
 }

 Screen_Rows = 24; // 25 rows 0 to 24
 Screen_Cols = 80; // 81 columns 0 to 80

}


//FUNCTION: Screen_Border_Setup
void Screen_Border_Setup(void)
{
 color_set(1, NULL);
 border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER,
        ACS_LLCORNER, ACS_LRCORNER);

 mvvline(1, Screen_Cols - 18, ACS_VLINE, 23); //internal vertical border
 mvhline(Screen_Rows - 3, 1, ACS_HLINE, 61);  //border between Main & Info
                                              //border between Menu & Status
 mvhline(Screen_Rows - 4, Screen_Cols - 17, ACS_HLINE, 17);

 //Part of internal border conjunctions
 mvaddch(0,               Screen_Cols - 18, ACS_TTEE);
 mvaddch(Screen_Rows - 4, Screen_Cols - 18, ACS_LTEE);
 mvaddch(Screen_Rows - 4, Screen_Cols,      ACS_RTEE);
 mvaddch(Screen_Rows - 3, 0,                ACS_LTEE);
 mvaddch(Screen_Rows - 3, Screen_Cols - 18, ACS_RTEE);
 mvaddch(Screen_Rows,     Screen_Cols - 18, ACS_BTEE);

 color_set(0, NULL);
}


//FUNCTION: Screen_SubWindow_Setup
void Screen_SubWindow_Setup(void)
{
 MainSubWin   = subwin(stdscr, 20, 61,  1,  1);
 MenuSubWin   = subwin(stdscr, 19, 17,  1, 63);
 InfoSubWin   = subwin(stdscr,  2, 61, 22,  1);
 StatusSubWin = subwin(stdscr,  3, 17, 21, 63);
}


//FUNCTION: Display_Menu
void Display_Menu(unsigned char Option)
{
 unsigned char Menu_Row, Menu_Col;
 char *Menu[] =
 {
  "Main Menu:\n",
  "1. How To Play",
  "2. New Game",
  "3. Load Game",
  "4. Save Game",
  "5. High Score",
  "6. Settings",
  "7. Quit",

  "\n Play A Card:\n",
  " 8. Singles",
  " 9. Doubles",
  "10. Triples",
  "11. Quadruples",
  "12. Pass",
  "13. Backto Game"
 };

 wclear(MenuSubWin);
 for (Menu_Row = 0; Menu_Row <= 14; Menu_Row++)
 {
  waddch(MenuSubWin, ' ');

  if (
      ((Option <= 7) && (Menu_Row <= 7) && (Option == Menu_Row)) ||

      ((InGame == 4) && (Option >= 8) && (Option <= 13) && (Menu_Row >= 9) &&
       (Menu_Row == Option + 1))
     )
    wattron(MenuSubWin, A_REVERSE);

  for (Menu_Col = 0; Menu[Menu_Row][Menu_Col] != '\0'; Menu_Col++)
  {
   if (((Option == Menu_Row) && (Option >= 1) && (Option <= 7) &&
       (Menu_Col == 2)) ||

       ((InGame == 4)  && (Menu_Row == Option+1) && (Option >= 8) &&
        (Option <= 13) && (Menu_Col == 3))
      )
     wprintw(MenuSubWin, "[");
   else
       wprintw(MenuSubWin, "%c", Menu[Menu_Row][Menu_Col]);
  }

  if (
      ((Option <= 7) && (Menu_Row <= 7) && (Option == Menu_Row)) ||

      ((InGame == 4) && (Option >= 8) && (Option <= 13) && (Menu_Row >= 9) &&
       (Menu_Row == Option + 1))
     )
  {
   wprintw(MenuSubWin, "]");
   wattroff(MenuSubWin, A_REVERSE);
  }

  waddch(MenuSubWin, '\n');

  if ((InGame == 1) && (Menu_Row == 7))
    Menu_Row = 14;
 }

 wrefresh(MenuSubWin);
}


//FUNCTION: Choice_Selection
void Choice_Selection(void)
{
 int Key, EnteredChoice;

 do
 {
  Key = getch();

  switch (Key)
  {
   case KEY_DOWN:
        switch (CurrentSubWindow)
        {
         case Menu:
              if ((InGame == 0) && (HighlightedMenuOption == 7))
                HighlightedMenuOption = 0;
              else if (HighlightedMenuOption == 13)
                     HighlightedMenuOption = 0;

              Display_Menu(++HighlightedMenuOption);
              break;

         case Main:
              switch (CurrentMainSubWindow)
              {
               case HowToPlay:
                    if ((fp_row >= 19) && (fp_row <= 55))
                      How_To_Play(fp_row - 18);
                    break;

               case PickingPhase:
                    Picking_Phase(Key);
                    break;

               case SwappingPhase:
                    break;

               case PlayingPhase:
                    Playing_Phase(Key);
                    break;

               case GameSettings:
                    break;

               default:
                        break;
              }
              break;
        }
        break;

   case KEY_UP:
        switch (CurrentSubWindow)
        {
         case Menu:
              if ((InGame == 0) && (HighlightedMenuOption == 1))
                HighlightedMenuOption = 8;
              else if (HighlightedMenuOption == 1)
                     HighlightedMenuOption = 14;

              Display_Menu(--HighlightedMenuOption);
              break;

         case Main:
              switch (CurrentMainSubWindow)
              {
               case HowToPlay:
                    if ((fp_row >= 20) && (fp_row <= 56))
                      How_To_Play(fp_row - 20);
                    break;

               case PickingPhase:
                    Picking_Phase(Key);
                    break;

               case SwappingPhase:
                    break;

               case PlayingPhase:
                    Playing_Phase(Key);
                    break;

               case GameSettings:
                    break;

               default:
                        break;
              }
              break;
        }
        break;

   case KEY_LEFT:
        switch (CurrentSubWindow)
        {
         case Menu:
              break;

         case Main:
              switch (CurrentMainSubWindow)
              {
               case PickingPhase:
                    Picking_Phase(Key);
                    break;

               case PlayingPhase:
                    Playing_Phase(Key);
                    break;

               default:
                        break;
              }

        }

        break;

   case KEY_RIGHT:
        switch (CurrentSubWindow)
        {
         case Menu:
              break;

         case Main:
              switch (CurrentMainSubWindow)
              {
               case PickingPhase:
                    Picking_Phase(Key);
                    break;

               case PlayingPhase:
                    Playing_Phase(Key);
                    break;

               default:
                        break;
              }

        }
        break;

   case 'A' ... 'Z':
   case 'a' ... 'z':
        if (CurrentMainSubWindow == NewGame)
          New_Game(Key);
        break;

   case '1' ... '9':
        if (CurrentSubWindow == Menu)
        {
         HighlightedMenuOption = Key - 48;
         Display_Menu(HighlightedMenuOption);
        }

   case ENTER_KEY:

        switch (CurrentSubWindow)
        {
         case Menu:
              EnteredChoice = HighlightedMenuOption;
              switch (EnteredChoice)
              {
               //How To Play
               case 1:
                    Status_Window();
                    CurrentMainSubWindow = HowToPlay;
                    if (fp_row == 0) How_To_Play(fp_row);
                    else How_To_Play(fp_row - 19);
                    break;

               //New Game
               case 2:
                    Status_Window();
                    CurrentMainSubWindow = NewGame;
                    InGame = 1;
                    New_Game(1);
                    break;

               //Load Game
               case 3:
                    Load_Game();
                    break;

               //Save Game
               case 4:
                    Save_Game();
                    break;

               //High Score
               case 5:
                    Status_Window();
                    CurrentMainSubWindow = HighScore;
                    High_Score();
                    break;

               //Game Settings
               case 6:
                    Status_Window();
                    CurrentMainSubWindow = GameSettings;
                    Game_Settings();
                    break;

               //Quit
               case 7:
                    Key = ESCAPE_KEY;
                    break;

               //Single Card
               case 8:
                    CardSelectionAmount = 1;
                    break;

               //Double Card
               case 9:
                    CardSelectionAmount = 2;
                    break;

               //Triple Card
               case 10:
                    CardSelectionAmount = 3;
                    break;

               //Quadruple Card
               case 11:
                    CardSelectionAmount = 4;
                    break;

               //Pass
               case 12:
                    break;

               //Backto Game
               case 13:
                    if ((CurrentMainSubWindow == HowToPlay) && (InGame == 4))
                    {
                     Status_Window();
                     CurrentMainSubWindow = PlayingPhase;
                     Playing_Phase(0);
                    }
                    break;
              }

              break;

         case Main:
              switch (CurrentMainSubWindow)
              {
               case NewGame:
                    New_Game(Key);
                    break;

               case PickingPhase:
                    Picking_Phase(Key);
                    break;

               case SwappingPhase:
                    Swapping_Phase();
                    break;

               case PlayingPhase:
                    Playing_Phase(Key);
                    break;

               default:
                        break;
              }


              break;
        }

        break;

   case SPACE_BAR_KEY:
        if (CurrentMainSubWindow == PlayingPhase)
          Playing_Phase(Key);
        break;

   case KEY_BACKSPACE:
        if (CurrentMainSubWindow == NewGame)
          New_Game(Key);
        break;

   case TAB_KEY:
        Status_Window();
        break;

   case ESCAPE_KEY:
        EnteredChoice = 7;
        break;
  }

 }
 while ((Key != ESCAPE_KEY) | (EnteredChoice != 7));
}


//FUNCTION: Status_Window
void Status_Window(void)
{
 wclear(StatusSubWin);

 wprintw(StatusSubWin, " Round:\n");
 wprintw(StatusSubWin, " Score:\n");

 switch (CurrentSubWindow)
 {
  case Main:
       CurrentSubWindow = Menu;
       wprintw(StatusSubWin, " Window: Menu");
       break;

  case Menu:
       CurrentSubWindow = Main;
       wprintw(StatusSubWin, " Window: Main");
       break;
 }

 wrefresh(StatusSubWin);
}


//FUNCTION: Info_Window
void Info_Window(unsigned char Msg)
{
 char *Info[] =
 {
  /*0*/ "Use arrow keys to navigate and the tab key to switch windows."
        "Press Escape to quit and menu options 1 to 9 in menu mode.",
  /*1*/ "Unable to open file HowToPlay.text.",
  /*2*/ "Enter player name to be used in the game and high score.",
  /*3*/ "Enter at least 1 character for name.",
  /*4*/ "Use arrow keys to navigate and press Enter to accept card or\n"
        "press 2 in menu mode to select a new game.",
  /*5*/ "Select another card. Card was previously selected."
 };

 wclear(InfoSubWin);
 wprintw(InfoSubWin, "%s", Info[Msg]);
 wrefresh(InfoSubWin);
}


//FUNCTION: How_To_Play
void How_To_Play(unsigned char fp_row_arg)
{
 FILE* fp;
 char Character;

 fp_row = 0;
 fp = fopen("Asol/HowToPlay.text", "r");

 if (fp == NULL)
 {
  wcolor_set(InfoSubWin, 5, NULL);
  wattron(InfoSubWin, A_BOLD);
  Info_Window(1);
  wattroff(InfoSubWin, A_BOLD);
 }
 else
 {
  wclear(MainSubWin);
  wcolor_set(MainSubWin, 4, NULL);

  while ((fp_row < fp_row_arg) && !feof(fp))
  {
   fscanf(fp, "%c", &Character);
   if (Character == '\n') fp_row++;
  }

  while ((getcury(MainSubWin) <= 18) && !feof(fp))
  {
   if (fscanf(fp, "%c", &Character) != EOF)
     waddch(MainSubWin, Character);

   if (Character == '\n') fp_row++;

   if ((getcury(MainSubWin) == 19) && !feof(fp) && ftell(fp) <= 2089)
   {
    wcolor_set(MainSubWin, 2, NULL);
    wattron(MainSubWin, A_BOLD);
    wprintw(MainSubWin, "[MORE]");
    wattroff(MainSubWin, A_BOLD);
    wcolor_set(MainSubWin, 4, NULL);
   }

  }

  wrefresh(InfoSubWin);
  wrefresh(MainSubWin);

  fclose(fp);
  wcolor_set(MainSubWin, 0, NULL);
 }
}


//FUNCTION New_Game
void New_Game(unsigned short int KeyPressed)
{
 unsigned char index = 0;

 wcolor_set(MainSubWin, 0, NULL);
 wcolor_set(InfoSubWin, 0, NULL);
 wclear(MainSubWin);

 mvwprintw(MainSubWin,  0, 26, "New Game");
 mvwaddch(MainSubWin, 8, 15, ACS_ULCORNER);
 whline(MainSubWin, ACS_HLINE, 30);
 mvwaddch(MainSubWin, 8, 46, ACS_URCORNER);
 mvwaddch(MainSubWin, 9, 15, ACS_VLINE);
 wprintw(MainSubWin, "Enter Name (max 10 characters)");
 waddch(MainSubWin, ACS_VLINE);
 mvwaddch(MainSubWin, 10, 15, ACS_VLINE);
 wprintw(MainSubWin, "          __________          ");
 waddch(MainSubWin, ACS_VLINE);
 mvwaddch(MainSubWin, 11, 15, ACS_LLCORNER);
 whline(MainSubWin, ACS_HLINE, 30);
 mvwaddch(MainSubWin, 11, 46, ACS_LRCORNER);


 while ((Player_Name[index] != '_') && (index <= 9))
      index++;

 switch (KeyPressed)
 {
  case 'A' ... 'Z':
  case 'a' ... 'z':
       if (index <= 9)
       {
        Player_Name[index] = KeyPressed;
        index++;
       }
       break;

  case KEY_BACKSPACE:
       if (index > 0)
       {
        index--;
        Player_Name[index] = '_';
       }
       break;

  case ENTER_KEY:
       if (index == 0) Info_Window(3);
       else
       {
        //Player_Name[index] = '\0';
        CurrentMainSubWindow = PickingPhase;
       }
       break;

  default: Info_Window(2);
 }

 mvwprintw(MainSubWin, 10, 26, "%s", Player_Name);
 wrefresh(MainSubWin);

 if (CurrentMainSubWindow == PickingPhase)
 {
  InGame = 2;
  repick = 1;
  CardsRemaining = 52;
  Card_Row = 2;
  Card_Col = 0;
  Display_Menu(HighlightedMenuOption);
  Picking_Phase(1);
 }
}


//FUNCTION: Card
char* Card(unsigned char CardIndex)
{
 char *CardType   = "CDHS";


 //Card Value
 switch (CardIndex / 4 + 3)
 {
  case 0 ... 9:
       ACard[0] = ' ';
       ACard[1] = (CardIndex / 4 + 3) + 48;
       break;

  case 10:
       ACard[0] = '1';
       ACard[1] = '0';
       break;

  case 11:
       ACard[0] = ' ';
       ACard[1] = 'J';
       break;

  case 12:
       ACard[0] = ' ';
       ACard[1] = 'Q';
       break;

  case 13:
       ACard[0] = ' ';
       ACard[1] = 'K';
       break;

  case 14:
       ACard[0] = ' ';
       ACard[1] = 'A';
       break;

  case 15:
       ACard[0] = ' ';
       ACard[1] = '2';
       break;

  case 16:
       ACard[0] = 'J';
       ACard[1] = 'O';
       ACard[2] = 'K';
       break;
 }

 //Card Type
 if (CardIndex <= 51)
   ACard[2] = CardType[CardIndex % 4];

 return ACard;
}


//FUNCTION: Picking_Phase
void Picking_Phase(unsigned short int KeyPressed)
{
 unsigned char Row,
               Col = 1,
               Card_Number = 1,
               loop,
               loop2;

 char *CardType = "CDHS";

 switch (KeyPressed)
 {
  case 1:
       //Initialize randomized card array of 52 cards
       for (loop = 0; loop < 52; loop++)
       {
        Card_List[loop] = ((unsigned char) rand() % 52); //0 to 51

        for (loop2 = 0; loop2 < loop; loop2++)
           if (Card_List[loop2] == Card_List[loop])
           {
            loop--;
            break;
           }
       }

       break;

  case KEY_DOWN:
       if (((Card_Row <= 11) && (Card_Col >= 12) && (Card_Col <= 54)) ||
           ((Card_Row <= 14) && (Card_Col >=  0) && (Card_Col <=  6))
          )
        {
         Card_Row = Card_Row + 3;
         Card_Index += 10;
        }
       break;

  case KEY_UP:
       if (Card_Row >= 5)
       {
         Card_Row = Card_Row - 3;
         Card_Index -= 10;
       }
       break;

  case KEY_LEFT:
       if (Card_Col >= 6)
       {
        Card_Col = Card_Col - 6;
        Card_Index-- ;
       }
       break;

  case KEY_RIGHT:
       if (((Card_Row == 17) && (Card_Col ==  0)) ||
           ((Card_Row >=  2) && (Card_Row <= 14) && (Card_Col <= 48))
          )
       {
        Card_Col = Card_Col + 6;
        Card_Index++ ;
       }
       break;

  case ENTER_KEY:

       if (Card_List[Card_Index] < 54)
         PlayersCardIndexChosen[0] = Card_Index;
       else
           break;


        //Each player chooses a uniqe card index 0 to 51
        for (loop = 1; loop < 4; loop++)
        {
         if ((repick == 1) || (Card_List[PlayersCardIndexChosen[loop]] >= 54))
           PlayersCardIndexChosen[loop] = ((unsigned char) rand() % 52);

         for (loop2 = 0; loop2 < loop; loop2++)
         {
          if (
              (PlayersCardIndexChosen[loop2] == PlayersCardIndexChosen[loop]) ||
              (Card_List[PlayersCardIndexChosen[loop]] >= 54)
             )
          {
           loop--;
           break;
          }
         }
        }

        if (repick == 1)
          repick++;



       //Test the 4 cards values
       //if all 4 == same card value then all 4 players re-pick new cards from
       //the remainding 48 cards. Players cannot pick the same card index twice.
       //Card indexes that were chosen that have the same value, add 54 to that
       //cards. Define values >= 54 as a non choosable cards.

       for (loop = 0; loop < 3; loop++)
          for (loop2 = loop + 1; loop2 < 4; loop2++)
             if (
                 ((Card_List[PlayersCardIndexChosen[loop]]  < 54) &&
                  (Card_List[PlayersCardIndexChosen[loop2]] < 54) &&
                  (Card_List[PlayersCardIndexChosen[loop]]/4 + 1 == Card_List[PlayersCardIndexChosen[loop2]]/4 + 1)) ||

                 ((Card_List[PlayersCardIndexChosen[loop]]  >= 54) &&
                  (Card_List[PlayersCardIndexChosen[loop2]] <  54) &&
                  ((Card_List[PlayersCardIndexChosen[loop]]-54)/4 + 1 == Card_List[PlayersCardIndexChosen[loop2]]/4 + 1)) ||

                 ((Card_List[PlayersCardIndexChosen[loop]]  < 54) &&
                  (Card_List[PlayersCardIndexChosen[loop2]] >=  54) &&
                  (Card_List[PlayersCardIndexChosen[loop]]/4 + 1 == (Card_List[PlayersCardIndexChosen[loop2]]-54)/4 + 1))

                )
             {
              if (Card_List[PlayersCardIndexChosen[loop]]  < 54)
              {
               Card_List[PlayersCardIndexChosen[loop]]  += 54;
               if (CardsRemaining > 0) CardsRemaining--;
              }

              if (Card_List[PlayersCardIndexChosen[loop2]] < 54)
              {
               Card_List[PlayersCardIndexChosen[loop2]] += 54;
               if (CardsRemaining > 0) CardsRemaining--;
              }
             }



        //Players with the same card value must re-pick new cards
        while (
               (CardsRemaining > 0) &&
               (
                (Card_List[PlayersCardIndexChosen[0]] >= 54) ||
                (Card_List[PlayersCardIndexChosen[1]] >= 54) ||
                (Card_List[PlayersCardIndexChosen[2]] >= 54) ||
                (Card_List[PlayersCardIndexChosen[3]] >= 54)
               )
              )
        {
         if (Card_List[PlayersCardIndexChosen[0]] >= 54)
           break;
         for (loop = 1; loop < 4; loop++)
            if (Card_List[PlayersCardIndexChosen[loop]] >= 54)
              PlayersCardIndexChosen[loop] = ((unsigned char) rand() % 52);
        }



       //Calculate rank for the 4 players
       if (Card_List[PlayersCardIndexChosen[0]] >= 54 && KeyPressed == ENTER_KEY)
         break;
       else
           for (loop = 0; loop < 4; loop++)
              for (loop2 = 0; loop2 < 4; loop2++)
                 if (
                     ((Card_List[PlayersCardIndexChosen[loop]]/4 + 1 < Card_List[PlayersCardIndexChosen[loop2]]/4 + 1) &&
                     (PlayerRank[loop] > PlayerRank[loop2])) ||
                     ((Card_List[PlayersCardIndexChosen[loop]]/4 + 1 > Card_List[PlayersCardIndexChosen[loop2]]/4 + 1) &&
                     (PlayerRank[loop] < PlayerRank[loop2]))
                    )
                 {
                  enum PlayerRankType TempPlayerRank = PlayerRank[loop];
                  PlayerRank[loop]  = PlayerRank[loop2];
                  PlayerRank[loop2] = TempPlayerRank;
                 }

       break;
 }



 wclear(MainSubWin);

 //Display the randomized 52 cards to main window
 for (Row = 0; Row <= 18; Row++)
 {
  if ((Row % 3) == 0)
  {
   if (Row != 0)
     mvwprintw(MainSubWin, Row, 1,
               "+---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+");

   if (Row+1 != 19)
     mvwprintw(MainSubWin, Row + 1, 1,
               "+---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+");
  }
  else if ((Row % 3) == 2)
         for (Col = 1; (Col < getmaxx(MainSubWin)-1) && (Card_Number % 53);
              Col = Col+6
             )
            mvwprintw(MainSubWin, Row, Col, "|%3d|", Card_Number++);
 }

 mvwprintw(MainSubWin,  0, 14, "Choose a card to decide your rank:");
 mvwprintw(MainSubWin, 16, 13, "President:      <Card Chosen>  <Player 1>");
 wclrtoeol(MainSubWin);
 mvwprintw(MainSubWin, 17, 13, "Vice-President: <Card Chosen>  <Player 2>");
 mvwprintw(MainSubWin, 18, 13, "Secretary:      <Card Chosen>  <Player 3>");
 wclrtoeol(MainSubWin);
 mvwprintw(MainSubWin, 19, 13, "Asol:           <Card Chosen>  <Player 4>");

 if (
     (KeyPressed == ENTER_KEY) &&
     (Card_List[PlayersCardIndexChosen[0]] < 54) &&
     (Card_List[PlayersCardIndexChosen[1]] < 54) &&
     (Card_List[PlayersCardIndexChosen[2]] < 54) &&
     (Card_List[PlayersCardIndexChosen[3]] < 54)
    )
 {
  char *CardValues = " A234567890JQK";

  for (loop = 0; loop < 4; loop++)
     mvwprintw(MainSubWin, 19 - PlayerRank[loop], 29,
               "%c%c%c            %s%c  ",
               ((Card_List[PlayersCardIndexChosen[loop]]/4 + 1) != 10)? ' ':'1',
               CardValues[Card_List[PlayersCardIndexChosen[loop]]/4 + 1],
               CardType[Card_List[PlayersCardIndexChosen[loop]]%4],
               (loop == 0)? Player_Name : "Player ",
               (loop == 0)? '\0': 48 + loop);

  InGame = 3;
  CurrentMainSubWindow = SwappingPhase;
 }

 //Set cursor navigation on current card
 wcolor_set(MainSubWin, 2, NULL);
 wattron(MainSubWin, A_BOLD);
 mvwaddch(MainSubWin, Card_Row, Card_Col, '[');
 mvwaddch(MainSubWin, Card_Row, Card_Col+6, ']');
 wattroff(MainSubWin, A_BOLD);
 wcolor_set(MainSubWin, 0, NULL);


 if (Card_List[Card_Index] >= 54)
 {
  char *CardValues = " A234567890JQK";

  mvwprintw(MainSubWin, 16, 13, "<Rank>         ");
  wclrtoeol(MainSubWin);
  mvwprintw(MainSubWin, 17, 13, "<Rank>         ");
  mvwprintw(MainSubWin, 18, 13, "<Rank>         ");
  wclrtoeol(MainSubWin);
  mvwprintw(MainSubWin, 19, 13, "<Rank>         ");

  for (loop = 0; loop < 4; loop++)
     mvwprintw(MainSubWin, 19 - PlayerRank[loop], 29,
               "%c%c%c            %s%c  ",
               ((((Card_List[PlayersCardIndexChosen[loop]]>=54)?Card_List[PlayersCardIndexChosen[loop]]-54:Card_List[PlayersCardIndexChosen[loop]])/4 + 1) != 10)? ' ':'1',
               CardValues[((Card_List[PlayersCardIndexChosen[loop]]>=54)?Card_List[PlayersCardIndexChosen[loop]]-54:Card_List[PlayersCardIndexChosen[loop]])/4 + 1],
               CardType[((Card_List[PlayersCardIndexChosen[loop]]>=54)?Card_List[PlayersCardIndexChosen[loop]]-54:Card_List[PlayersCardIndexChosen[loop]])%4],
               (loop == 0)? Player_Name : "Player ",
               (loop == 0)? '\0': 48 + loop);

  Info_Window(5);
 }

 else
     Info_Window(4);



 wrefresh(MainSubWin);
}


//FUNCTION: Card_Sort
void Card_Sort(unsigned char index, unsigned char index2)
{
 unsigned char loop,
               loop2,
               TempCard;


 for (loop = index; loop < index2; loop++)
    for (loop2 = loop + 1; loop2 <= index2; loop2++)
       if ((Card_List[loop2] / 4 + 3) < (Card_List[loop] / 4 + 3))
       {
        TempCard = Card_List[loop];
        Card_List[loop]  = Card_List[loop2];
        Card_List[loop2] = TempCard;
       }
}


//FUNCTION: Swapping_Phase
void Swapping_Phase(void)
{
 unsigned char Row,
               Col,
               CardIndex,
               Card0,
               Card1;


 Players_Names[PlayerRank[0]] = Player_Name;
 Players_Names[PlayerRank[1]] = "Player 1";
 Players_Names[PlayerRank[2]] = "Player 2";
 Players_Names[PlayerRank[3]] = "Player 3";


 //Initialize randomized card array of 54 cards
 for (Row = 0; Row < 54; Row++)
 {
  Card_List[Row] = (unsigned char) (rand() % 54); //0 to 53

  for (Col = 0; Col < Row; Col++)
     if (Card_List[Col] == Card_List[Row])
     {
      Row--;
      break;
     }
 }

 //Sort each player's cards in ascending order
 Card_Sort( 0, 13); //President
 Card_Sort(14, 27); //Vice-President
 Card_Sort(28, 40); //Secretary
 Card_Sort(41, 53); //Asol:



 wclear(MainSubWin);
 wclear(InfoSubWin);

 mvwprintw(MainSubWin, 0, 23, "Swapping Phase:");

 mvwprintw(MainSubWin, 1, 1, "President (%s)", Players_Names[3]);
 mvwprintw(MainSubWin, 1,
           ((PlayerRank[0] == President) || (PlayerRank[0] == Asol))? 33 : 31,
           "Vice-President (%s)", Players_Names[2]);
 mvwprintw(MainSubWin,
    ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))? 13 : 10,
    ((PlayerRank[0] == President) || (PlayerRank[0] == Asol))? 33 : 31,
    "Secretary (%s)", Players_Names[1]);
 mvwprintw(MainSubWin,
    ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))? 13 : 10,
    1,
    "Asol (%s)", Players_Names[0]);


 //Human player
 switch (PlayerRank[0])
 {
  case President:
       CardIndex = 0;
       for (Row = 2; Row <= 10; Row++)
          for (Col = 1; Col <= 25; Col += 6)
          {
           if (Row == 2 || Row ==  4 || Row == 5 || Row == 7 ||
               Row == 8 || Row == 10)
             mvwprintw(MainSubWin, Row, Col, "+---+ ");
           else
           {
            mvwprintw(MainSubWin, Row, Col, "|%s|", Card(Card_List[CardIndex]));
            if (CardIndex <= 12) CardIndex++;
           }

           if ((Row == 8 || Row == 9 || Row == 10) && Col == 25)
             mvwprintw(MainSubWin, Row, Col, "     ");
          }

       break;

  case VicePresident:
       CardIndex = 14;
       for (Row = 2; Row <= 10; Row++)
          for (Col = 31; Col <= 59; Col += 6)
          {
           if (Row == 2 || Row ==  4 || Row == 5 || Row == 7 ||
               Row == 8 || Row == 10)
             mvwprintw(MainSubWin, Row, Col, "+---+ ");
           else
           {
            mvwprintw(MainSubWin, Row, Col, "|%s|", Card(Card_List[CardIndex]));
            if (CardIndex <= 26) CardIndex++;
           }

          if ((Row == 8 || Row == 9 || Row == 10) && Col == 55)
            mvwprintw(MainSubWin, Row, Col, "     ");
         }

       break;

  case Secretary:
       CardIndex = 28;
       for (Row = 11; Row <= 19; Row++)
          for (Col = 31; Col <= 59; Col += 6)
          {
           if (Row == 11 || Row == 13 || Row == 14 || Row == 16 ||
               Row == 17 || Row == 19)
             mvwprintw(MainSubWin, Row, Col, "+---+ ");
           else
           {
            mvwprintw(MainSubWin, Row, Col, "|%s|", Card(Card_List[CardIndex]));
            if (CardIndex <= 39) CardIndex++;
           }

           if ((Row  == 17 || Row  == 18 || Row == 19) &&
               (Col == 49 || Col == 55))
             mvwprintw(MainSubWin, Row, Col, "     ");
          }

       break;

  case Asol:
       CardIndex = 41;
       for (Row = 11; Row <= 19; Row++)
          for (Col = 1; Col <= 25; Col += 6)
          {
           if (Row == 11 || Row == 13 || Row == 14 || Row == 16 ||
               Row == 17 || Row == 19)
             mvwprintw(MainSubWin, Row, Col, "+---+ ");
           else
           {
            mvwprintw(MainSubWin, Row, Col, "|%s|", Card(Card_List[CardIndex]));
            if (CardIndex <= 52) CardIndex++;
           }

           if ((Row  == 17|| Row  == 18|| Row == 19) &&
               (Col == 19 || Col == 25))
             mvwprintw(MainSubWin, Row, Col, "     ");
          }

       break;

  default:
           break;
 }



 //Players 1, 2 and 3
 for (unsigned char Player = 1; Player <= 3; Player++)
    switch (PlayerRank[Player])
    {
     case President:
          mvwprintw(MainSubWin, 2, 1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 3, 1, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, 4, 1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 5, 1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 6, 1, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, 7, 1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          break;

     case VicePresident:
          if (PlayerRank[0] == President || PlayerRank[0] == Asol)
            Col = 33;
          else
              Col = 31;

          mvwprintw(MainSubWin, 2, Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 3, Col, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, 4, Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 5, Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, 6, Col, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, 7, Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          break;

     case Secretary:
          if (PlayerRank[0] == President)
          {
           Row = 14;
           Col = 33;
          }
          else if (PlayerRank[0] == VicePresident)
          {
           Row = 14;
           Col = 31;
          }
          else if (PlayerRank[0] == Asol)
          {
           Row = 11;
           Col = 33;
          }

          mvwprintw(MainSubWin, Row,   Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+1, Col, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, Row+2, Col, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+3, Col, "+-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+4, Col, "| | | | | | | | | | | |");
          mvwprintw(MainSubWin, Row+5, Col, "+-+ +-+ +-+ +-+ +-+ +-+");
          break;

     case Asol:
          if (PlayerRank[0] == President || PlayerRank[0] == VicePresident)
            Row = 14;
          else
              Row = 11;

          mvwprintw(MainSubWin, Row,   1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+1, 1, "| | | | | | | | | | | | | |");
          mvwprintw(MainSubWin, Row+2, 1, "+-+ +-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+3, 1, "+-+ +-+ +-+ +-+ +-+ +-+");
          mvwprintw(MainSubWin, Row+4, 1, "| | | | | | | | | | | |");
          mvwprintw(MainSubWin, Row+5, 1, "+-+ +-+ +-+ +-+ +-+ +-+");
          break;

     default:
              break;
    }


 //Card exchange

 if (PlayerRank[0] == President || PlayerRank[0] == Asol)
 {
  wprintw(InfoSubWin, " President and Asol 2 cards exchange:\n");
  wprintw(InfoSubWin, " President: %s, ", Card(Card_List[0]));
  wprintw(InfoSubWin, "%s             ", Card(Card_List[1]));
  wprintw(InfoSubWin, "Asol: %s, ", Card(Card_List[52]));
  wprintw(InfoSubWin, "%s", Card(Card_List[53]));
 }
 else
 {
  wprintw(InfoSubWin, " Vice-President and Secretary 1 card exchange:\n");
  wprintw(InfoSubWin, " Vice-President: %s           ", Card(Card_List[14]));
  wprintw(InfoSubWin, "Secretary: %s", Card(Card_List[40]));
 }

 //Pres/Asol exchange
 Card0         = Card_List[0];
 Card1         = Card_List[1];
 Card_List[0]  = Card_List[52];
 Card_List[1]  = Card_List[53];
 Card_List[52] = Card0;
 Card_List[53] = Card1;

 //Vice/Sec exchange
 Card0         = Card_List[14];
 Card_List[14] = Card_List[40];
 Card_List[40] = Card0;



 Card_Sort( 0, 13); //President
 Card_Sort(14, 27); //Vice-President
 Card_Sort(28, 40); //Secretary
 Card_Sort(41, 53); //Asol:




 wrefresh(MainSubWin);
 wrefresh(InfoSubWin);

 CurrentMainSubWindow = PlayingPhase;

 //Initializing cursor (square brackets) to select cards in playing phase
 switch (PlayerRank[0])
 {
  case President:
       Card_Row = 2;
       Card_Col = 0;
       Card_Index = 0;
       break;

  case VicePresident:
       Card_Row = 2;
       Card_Col = 30;
       break;

  case Secretary:
       Card_Row = 12;
       Card_Col = 30;
       break;

  case Asol:
       Card_Row = 12;
       Card_Col = 0;
       break;

  default:
           break;
 }

 InGame = 4;
 CurrentPlayer = President;
 PlayersLastCards[President]     = 13;
 PlayersLastCards[VicePresident] = 27;
 PlayersLastCards[Secretary]     = 40;
 PlayersLastCards[Asol]          = 50;
}


//FUNCTION: Playing_Phase
void Playing_Phase(unsigned short int KeyPressed)
{
 unsigned char loop, Row, Col;

 char *PlayerRankTitles[] =
 {
  "Asol",
  "Secretary",
  "Vice-President",
  "President"
 };

 wclear(MainSubWin);
 Display_Menu(HighlightedMenuOption);

 //Print 4 ranks and player names
 for (loop = 0; loop <= 3; loop++)
 {
  switch (PlayerRank[loop])
  {
   case President:
        Row = 0;
        Col = 1;
        break;

   case VicePresident:
        if (PlayerRank[0] == President)
        {
         Row = 0;
         Col = 37;
        }
        else if ((PlayerRank[0] == Secretary) || (PlayerRank[0] == Asol))
        {
         Row = 0;
         Col = 33;
        }
        else
        {
         Row = 0;
         Col = 27;
        }
        break;

   case Secretary:
        if ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))
        {
         Row = 13;
         Col = 33;
        }
        else if (PlayerRank[0] == Asol)
        {
         Row = 10;
         Col = 37;
        }
        else
        {
         Row = 10;
         Col = 27;
        }
        break;

   case Asol:
        if ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))
        {
         Row = 13;
         Col = 1;
        }
        else
        {
         Row = 10;
         Col = 1;
        }
        break;

   default:
            break;
  }

  mvwprintw(MainSubWin, Row, Col, ((PlayerRank[0] == President) && 
            (PlayerRank[loop] == VicePresident))? "%.10s (%s)" : "%s (%s)",
            PlayerRankTitles[PlayerRank[loop]], Players_Names[PlayerRank[loop]]);
 }



 //Current player's turn
 switch (CurrentPlayer)
 {
  case President:
       if (PlayerRank[0] == President)
         {
          //wait on user card's selection
          switch (KeyPressed)
          {
           //case KEY_DOWN:
                  //if on 1st card row & 2nd card row exist or more than 5 cards
                  //then Card_Row += 3
                  //else on card row 2 & card row 3 exists
                  //break;

           //case KEY_UP:
                  //break;

           case KEY_LEFT:
                if (Card_Index == 0)
                {
                 Card_Row = 8;
                 Card_Col = 18;
                 Card_Index = 13;
                }
                else
                {
                 Card_Index--;
                 if ((Card_Index % 5) == 4)
                   Card_Row -= 3;
                 Card_Col = 6*(Card_Index % 5);
                }
                break;

           case KEY_RIGHT:
                if (Card_Index == 13)
                {
                 Card_Row = 2;
                 Card_Col = 0;
                 Card_Index = 0;
                }
                else
                {
                 Card_Index++;
                 if ((Card_Index % 5) == 0)
                   Card_Row += 3;
                 Card_Col = 6*(Card_Index % 5);
                }
                break;

           //case ENTER_KEY:
                  //break;

           //case SPACE_BAR_KEY:
                  //break;
          }
         }
       //else
             //Player plays his cards
       break;

  case VicePresident:
       break;

  case Secretary:
       break;

  case Asol:
       break;

  default:
           break;
 }



 // Print Human player's cards
 switch (PlayerRank[0])
 {
  case President:
       Row = 1;
       Col = 1;
       for (loop = 0; loop <= 13; loop++)
       {
        if (Card_List[loop] <= 53)
        {
         mvwprintw(MainSubWin, Row, Col, "+---+");
         if ((Card_Row == Row+1) && (Card_Col+1 == Col))
         {
          wcolor_set(MainSubWin, 2, NULL);
          wattron(MainSubWin, A_BOLD);
          mvwaddch(MainSubWin, Card_Row, Card_Col, '[');
          mvwaddch(MainSubWin, Card_Row, Card_Col+6, ']');
          wattroff(MainSubWin, A_BOLD);
          wcolor_set(MainSubWin, 0, NULL);
         }
         mvwprintw(MainSubWin, Row+1, Col, "|%s|", Card(Card_List[loop]));
         mvwprintw(MainSubWin, Row+2, Col, "+---+");

         //if 5 cards printed then Row += 3; or Col == 31
         Col += 7;
         if (Col == 36)
         {
          Row += 3;
          Col = 1;
         }
        }
        else continue;
       }
       break;

  case VicePresident:
       Row = 1;
       Col = 27;
       for (loop = 14; loop <= 27; loop++)
       {
        if (Card_List[loop] <= 53)
        {
         mvwprintw(MainSubWin, Row,   Col, "+---+");
         mvwprintw(MainSubWin, Row+1, Col, "|%s|", Card(Card_List[loop]));
         mvwprintw(MainSubWin, Row+2, Col, "+---+");

         Col += 7;
         if (Col == 62)
         {
          Row += 3;
          Col = 27;
         }
        }
        else continue;
       }
       break;

  case Secretary:
       Row = 11;
       Col = 31;
       for (loop = 28; loop <= 40; loop++)
       {
        if (Card_List[loop] <= 53)
        {
         mvwprintw(MainSubWin, Row,   Col, "+---+");
         mvwprintw(MainSubWin, Row+1, Col, "|%s|", Card(Card_List[loop]));
         mvwprintw(MainSubWin, Row+2, Col, "+---+");

         Col += 6;
         if (Col == 61)
         {
          Row += 3;
          Col = 31;
         }
        }
        else continue;
       }
       break;

  case Asol:
       Row = 11;
       Col = 1;
       for (loop = 41; loop <= 53; loop++)
       {
        if (Card_List[loop] <= 53)
        {
         mvwprintw(MainSubWin, Row,   Col, "+---+");
         mvwprintw(MainSubWin, Row+1, Col, "|%s|", Card(Card_List[loop]));
         mvwprintw(MainSubWin, Row+2, Col, "+---+");

         Col += 6;
         if (Col == 31)
         {
          Row += 3;
          Col = 1;
         }
        }
        else continue;
       }
       break;

  default:
           break;
 }

 //Print cards of players 1, 2 and 3
 for (unsigned char Player = 1; Player <= 3; Player++)
    switch (PlayerRank[Player])
    {
     case President:
          Row = 1;
          Col = 1;
          for (loop = 0; loop <= 13; loop++)
          {
           if (Card_List[loop] <= 53)
           {
            mvwprintw(MainSubWin, Row,   Col, "+-+");
            mvwprintw(MainSubWin, Row+1, Col, "| |");
            mvwprintw(MainSubWin, Row+2, Col, "+-+");

            //if 7 cards printed then Row += 3; or Col == 29
            Col += 4;
            if (Col == 25)
            {
             Row += 3;
             Col = 1;
            }
           }
           else continue;
       }
          break;

     case VicePresident:
          Row = 1;
          if (PlayerRank[0] == President)
            Col = 37;
          else if (PlayerRank[0] == Asol)
                 Col = 33;

          for (loop = 14; loop <= 27; loop++)
          {
           if (Card_List[loop] <= 53)
           {
            mvwprintw(MainSubWin, Row,   Col, "+-+");
            mvwprintw(MainSubWin, Row+1, Col, "| |");
            mvwprintw(MainSubWin, Row+2, Col, "+-+");

            Col += 4;

            if ((PlayerRank[0] == President) && (Col == 61))
            {
             Row += 3;
             Col = 37;
            }
            else if (PlayerRank[0] == Secretary && Col == 59)
            {
             Row += 3;
             Col = 31;
            }

           }
           else continue;
          }
          break;

     case Secretary:
          Row = (PlayerRank[0] == President || PlayerRank[0] == VicePresident)?
                14 : 11;
          Col = (PlayerRank[0] == President || PlayerRank[0] == Asol)? 33 : 31;

          for (loop = 28; loop <= 40; loop++)
          {
           if (Card_List[loop] <= 53)
           {
            mvwprintw(MainSubWin, Row,   Col, "+-+");
            mvwprintw(MainSubWin, Row+1, Col, "| |");
            mvwprintw(MainSubWin, Row+2, Col, "+-+");

            Col += 4;

            if ((PlayerRank[0] == President || PlayerRank[0] == Asol) &&
                (Col == 61))
            {
             Row += 3;
             Col = 33;
            }
            else if (PlayerRank[0] == VicePresident && Col == 59)
            {
             Row += 3;
             Col = 31;
            }
           }
           else continue;
          }
          break;

     case Asol:
          Row = (PlayerRank[0] == President || PlayerRank[0] == VicePresident)?
                14 : 11;
          Col = 1;
          for (loop = 41; loop <= 53; loop++)
          {
           if (Card_List[loop] <= 53)
           {
            mvwprintw(MainSubWin, Row,   Col, "+-+");
            mvwprintw(MainSubWin, Row+1, Col, "| |");
            mvwprintw(MainSubWin, Row+2, Col, "+-+");

            Col += 4;
            if (Col == 29)
            {
             Row += 3;
             Col = 1;
            }
           }
           else continue;
          }
          break;

     default:
              break;
    }



 //Print previous player's name and his cards played
 mvwprintw(MainSubWin,
    ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))? 10 : 7,
    1,
    "Cards played by: ");
 mvwprintw(MainSubWin,
    ((PlayerRank[0] == President) || (PlayerRank[0] == VicePresident))? 11 : 8,
    1,
    "Cards played: ");



 wrefresh(MainSubWin);
}


//FUNCTION: Load_Game
void Load_Game(void)
{

}


//FUNCTION: Save_Game
void Save_Game(void)
{

}


//FUNCTION: High_Score
void High_Score(void)
{

}


//FUNCTION: Game_Settings
void Game_Settings(void)
{

}


//FUNCTION: Delete_SubWindows
void Delete_SubWindows(void)
{
 delwin(MainSubWin);
 delwin(MenuSubWin);
 delwin(InfoSubWin);
 delwin(StatusSubWin);
}


//FUNCTION: End
void Ncurses_End(void)
{
 clear();
 refresh();
 Delete_SubWindows();
 endwin();
}
