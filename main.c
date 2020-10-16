/* TODO: Allow user to select section to print. This was my idea.*/
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forced to redefine PACKAGE and PACKAGE_VERSION to pass check */
#define PACKAGE 1
#define PACKAGE_VERSION 1
#include <bfd.h>
#undef PACKAGE
#undef PACKAGE_VERSION

#include <ncurses.h>

char **getSectionNames(bfd *file, unsigned int num);
void wPrintSections(char **names, WINDOW *win, unsigned int num);
WINDOW *create_newwin(int height, int width, int starty, int startx);

int main(int argc, char **argv){

    initscr();
    cbreak();
    refresh();

    WINDOW *output = create_newwin(25, 60, 5, 1);
    WINDOW *input = create_newwin(3, 6, 31, 1);
    WINDOW *instruct = create_newwin(3, 60, 1, 1);

    bfd_init();
    bfd *binary;
    const bfd_arch_info_type *archInfo;
    char **sectionNames;
    int i;
    unsigned int c;
    unsigned int nos;

    keypad(stdscr, true);
    // Checking arguments
    // Need at least 3 for real output. 
    if(argc != 2){
	printf("Usage: %s <binary file> -s -S -P <section> -a\n", argv[0]);
	exit(-1);
    }

    // Open our binary file in read mode
    if((binary = bfd_openr(argv[1], NULL)) == NULL){
	printf("Failed to open binary!\n");
	exit(-1);
    }

    // Checking to see if the file is a binary_object
    // bfd_object may contain data, symbols, relocations, and debug info.	
    if(!bfd_check_format(binary, bfd_object)){
	printf("Failed to open binary file!\n");
	exit(-1);
    }

    // Display name of current file.
    wprintw(stdscr, "[loaded binary] -> %s\n", argv[1]);
    wrefresh(stdscr);

    // Initial design for instruct
    wmove(instruct, 0, 0);
    wprintw(instruct, "Help:");
    wmove(instruct, 1, 1);
    wprintw(instruct, "s = List Sections\tq = Quit");
    wrefresh(instruct);

    // Initial design for output
    wmove(output, 0, 0);
    wprintw(output, "Output:");
    wrefresh(output);

    // Initial design for input
    wmove(input, 0, 0);
    wprintw(input, "Input:");
    wmove(input, 1, 1);
    wrefresh(input);



    // Parse Arguments
    
    /*  
	We loop through each argument and execute their specific
	functionality when we come across the arguments. 
    */
    while(c != 'q'){
	wmove(stdscr, 32, 3);
	wrefresh(input);
	c = getch();
	switch(c){
	    case 's':
		nos = bfd_count_sections(binary);
		sectionNames = getSectionNames(binary, nos);
		//wprint_section_names(binary, output);
		//wrefresh(output);
		//wrefresh(input);
		wPrintSections(sectionNames, output, nos);
		break;
	}
    } 

    // Close the binary, be nice to the computer :) 
    bfd_close(binary);
    endwin();
    return 0;
}
char **getSectionNames(bfd *file, unsigned int num){

    /* Return an array of the section names */

    int i = 0;
    char **tmpList = malloc(sizeof(char*) * num);;
    asection *sectionParse = file->sections;
    while(sectionParse != NULL){
	tmpList[i] = malloc(sizeof(char) * strlen(sectionParse->name));
	strncpy(tmpList[i], sectionParse->name, strlen(sectionParse->name));
	tmpList[i][strlen(sectionParse->name)] = '\0';
	sectionParse = sectionParse->next;
	i++;
    }

    return tmpList;
}

void wPrintSections(char **names, WINDOW *win, unsigned int num){

    /* Display Sections in a given window, and allow scrolling */
    /* Tristan pretty much made this function */

    int start = 0;
    int end = 20;
    int inputChar = '0';
    int i;
    int j = 2; 
    if(end > num){
	end = num - 1;
    }

    while(inputChar != 'q'){
	/* display sections */
	j = 2;
	wclear(win);	
	box(win, 0,0);
	wmove(win, 0,0);
	wprintw(win, "Sections:");
	for(i = start; i <= end; i++){
	    wmove(win, j, 2); 
	    wprintw(win, "%s", names[i]);
	    j++;
	}
	wrefresh(win);
	inputChar = getch();
	switch(inputChar){
	    case KEY_DOWN:	// Scroll down
		if(end != num - 1){
		    start++;
		    end++;
		}
		break;

	    case KEY_UP:	// Scroll up
		if(start != 0){
		    start--;
		    end--;
		}
		break;
	}
    }

    return;

}

WINDOW *create_newwin(int height, int width, int starty, int startx){
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}
