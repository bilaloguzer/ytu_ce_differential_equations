#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>		//used only to make text lowercase

#define MAX_LENGTH 511
#define MAX_QUOTE 100

//TODO: REMOVE SPACES AT THE END OF THE SENTENCE AND DATA CLEARING IS DONE.

int compute_number_of_words(char *text);
void text_to_vector(char dictionary[][MAX_LENGTH], char quotes[][MAX_LENGTH], int vectors[][MAX_LENGTH], int wordcount) ;
void remove_punctuation(char *text);
void remove_multiple_spaces(char *text, int tlen);
void organize_text(char *text);
void slide_text(char *text, int tlen, int start, int step, int direction);
int fill_dictionary(char wordsdic[][MAX_LENGTH], char quotes[][MAX_LENGTH]);
int check_occurence(char dictionary[][MAX_LENGTH], char *word, int check_until);
void make_lowercase(char quotes[][MAX_LENGTH]);
void organize_add(char quotes[][MAX_LENGTH]);


int main(){
	int i=0, j;
	FILE *fptr;
	
	fptr = fopen("hayyam.txt", "r");
	if( fptr == NULL ){
		printf("File couldn't be opened. Exiting.");
		return 1;
	}

	char quotes[MAX_QUOTE][MAX_LENGTH], wordsdic[2000][MAX_LENGTH];
	int wordvectors[200][MAX_LENGTH];
	
	for( i=0; i<MAX_QUOTE; i++ ){
		fgets(quotes[i], MAX_LENGTH, fptr);
	}
	fclose(fptr);
	for( i=0; i<MAX_QUOTE; i++ ){
		printf("\nMETIN %d\ncount test --> %d\n", i+1, compute_number_of_words(quotes[i]));
		printf("len = %d\n", strlen(quotes[i]));
		printf("%s\n", quotes[i]);
	}
	for( i=0; i<MAX_QUOTE; i++ ){
		remove_punctuation(quotes[i]);
		remove_multiple_spaces(quotes[i], strlen(quotes[i]));
	}
	make_lowercase(quotes);
	organize_add(quotes);
	
	printf("-------------------PUNCTUATION REMOVED--------------------------\n\n");
	for( i=0; i<MAX_QUOTE; i++ ){
		printf("\ncount test --> %d\n", compute_number_of_words(quotes[i]));
		printf("len = %d\n", strlen(quotes[i]));
		printf("%s\n", quotes[i]);
	}
	
	int wordcount = fill_dictionary(wordsdic, quotes);  	//FILL_DICTIONARY'DE SORUN VAR. 10 YA DA 15'TE HATA ALIP ÇIKIYOR.
	
	
	printf("\n-------------------DICTIONARY FORMED--------------------------\n\n");
	printf("wordcount = %d\n", wordcount);
	
	for( i=0; i<MAX_QUOTE; i++ ){
		printf("\ncount test --> %d\n", compute_number_of_words(quotes[i]));
		printf("len = %d\n", strlen(quotes[i]));
		printf("%s\n", quotes[i]);
	}
	
    printf("\nUnique words in the dictionary:");
    for (i = 0; i < wordcount; i++) {
        printf("\n%d: %s", i + 1, wordsdic[i]);
    }
	
	text_to_vector(wordsdic, quotes, wordvectors, wordcount);
	
	for( i=0; i<MAX_QUOTE; i++ ){
    	printf("\n%d. METIN: %s\n%d. METNIN VEKTOR HALI:\n", i+1, quotes[i] ,i+1);
    	for (j = 0; j < wordcount; j++) {
        	printf("%s: %d\n", wordsdic[j], wordvectors[i][j]);
    	}
	}
	
	
	return 0;
}

int fill_dictionary(char dictionary[][MAX_LENGTH], char quotes[][MAX_LENGTH]) {
    int i, j, count = 0, tmp = 0;
    char word[MAX_LENGTH];

    for (i = 0; i < MAX_QUOTE; i++) {
        j = 0;
        while (quotes[i][j] != '\0' && quotes[i][j] != '\n' ) {
            if (quotes[i][j] != ' ') {
                word[tmp] = quotes[i][j];
                tmp++;
                j++;
            } else {
                word[tmp] = '\0';
                if( check_occurence(dictionary, word, count) != 1 ){
                	strncpy(dictionary[count], word, tmp);
                	count++;
				}
                tmp = 0;
                j++;
            }
        }
        if (tmp > 0) {
            word[tmp] = '\0';
            if( check_occurence(dictionary, word, count) != 1 ){
               	strncpy(dictionary[count], word, tmp);
               	count++;
			}
			tmp = 0;
        }

        printf("%d. yaziyla birlikte kelime sayisi: %d\n", i + 1, count);
    }
    return count;
}

int check_occurence(char dictionary[][MAX_LENGTH], char *word, int check_until){
	int i=0;
	while( i<check_until){
		if( strcmp(dictionary[i], word) == 0 ){
			return 1;
		}
		i++;
	}
	return 0;
}

int compute_number_of_words(char *text){		//HOW CAN WE?
	int count = 0, len = strlen(text), i;
	for( i=0; i<len; i++ ){
		if( text[i] == ' ' ){
			count++;
		}
	}
	
	return count+1;
}

void remove_punctuation(char *text){
	const char *punctuations = "!()-[]{};:“”,<>./?@#$%^&*\"_~—-";
	int i=0, len = strlen(text);
	while( i<len && text[i] != '\0' ){
		if( strchr(punctuations, text[i]) ){
			text[i] = ' ';
		}
		i++;
	}
}

void remove_multiple_spaces(char *text, int tlen){
	int i=0, j, tmp;
	while( text[i++] != '\0' ){
		if( text[i] == ' ' ){
			tmp = 0;
			j = i;
			while( text[j++] == ' ' ){
				tmp++;
			}
			if( tmp > 1 ){
				slide_text(text, tlen, i, tmp-1, 1);
			}			
		}
	}
}

void make_lowercase(char quotes[][MAX_LENGTH]){
	int i, j, len;
	for( i=0; i<MAX_QUOTE; i++ ){
		len = strlen(quotes[i]);
		j=0;
		while( j<len && quotes[i][j] != '\0' ){
			quotes[i][j] = tolower(quotes[i][j]);
			j++;
		}
	}
}

void text_to_vector(char dictionary[][MAX_LENGTH], char quotes[][MAX_LENGTH], int vectors[][MAX_LENGTH], int wordcount) {
	int i, j;
	for( i=0; i<MAX_QUOTE; i++ ){
		//Eðer sözlükteki karþýlýk gelen kelime metinde var ise vektörün orasýný 1 yap, yoksa 0 yap.
		for( j=0; j<wordcount; j++ ){
			if( strstr( quotes[i], dictionary[j] )  ){			//PLAIN GECÝNCE IN'E DE 1 DEDÝ.
				vectors[i][j] = 1;
			}
			else{
				vectors[i][j] = 0;
			}
		}
	}
}


void slide_text(char *text, int tlen, int start, int step, int direction){	//1 ise sola doðru kapa, 0 ise saða doðru aç.
	int i, end = strlen(text);	
	if( direction == 1 ){
		for( i=start; i<end; i++ ){
			text[i] = text[i+step];
		}
		text[i] = '\0';
	}
	else if( direction == 0 ){
		for( i=tlen; i>start; i-- ){
			text[i+step] = text[i];
		}
		text[tlen+step-1] = '\0';
	}
}

void organize_add(char quotes[][MAX_LENGTH]){
	int i, j, len;
	for( i=0; i<MAX_QUOTE; i++ ){
		len = strlen(quotes[i]);
		j=0;
		while( j<len && quotes[i][j] != '\0' ){
			if( quotes[i][j] == '\'' ){			//KESME ÝSARETÝ BULDUK. 'VE YA DA N'T MI DÝYE BAK.
				if( quotes[i][j-1] == 'n' && quotes[i][j+1] == 't' ){		//NOT
					slide_text(quotes[i], strlen(quotes[i]), j, 1, 0);
					quotes[i][j-1] = ' '; quotes[i][j] = 'n'; quotes[i][j+1] = 'o'; quotes[i][j+2] = 't'; quotes[i][j+3] = ' ';
				}
				else if( quotes[i][j+1] == 'v' && quotes[i][j+2] == 'e' ){		//HAVE
					slide_text(quotes[i], strlen(quotes[i]), j, 2, 0);
					quotes[i][j] = ' ';  quotes[i][j+1] = 'h';  quotes[i][j+2] = 'a';  quotes[i][j+3] = 'v';  quotes[i][j+4] = 'e';  quotes[i][j+5] = ' ';  
				}
			}
			j++;
		}
	}
}
