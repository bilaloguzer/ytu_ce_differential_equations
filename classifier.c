#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>		//for random number generator
#include <ctype.h>		//used only to make text lowercase

#define MAX_LENGTH 511
#define MAX_QUOTE 100
#define DICT_SIZE 2500

//TODO: 1 means Hayyam, -1 means Shakespeare.

int compute_number_of_words(char *text);
void text_to_vector(char **dictionary, char **quotes, int **vectors, int wordcount) ;
void remove_punctuation(char *text);
void remove_multiple_spaces(char *text, int tlen);
void organize_text(char *text);
void slide_text(char *text, int tlen, int start, int step, int direction);
int fill_dictionary(char **wordsdic, char **quotes);
int check_occurence(char **dictionary, char *word, int check_until);
void make_lowercase(char **quotes);
void organize_add(char **quotes);

double dot_product(int *vector, double *parameters, int wordcount);
double f_x(int *vector, double *parameters, int wordcount);
void initiate_param(double *parameters, int wordcount);

void gradiend_descent(int **vectors, double *parameters, int wordcount, int *labels, double stepsize, int maxiter, double error);
void stoc_grad_desc(int **vectors, double *parameters, int wordcount, int *labels, double stepsize, int maxiter, double error);
void adam();

void compute_gradient(int **vectors, double *parameters, int wordcount, int *labels, double *gradients);
double compute_loss(int *vector, double *parameters, int wordcount, int *labels);

int main(){
	int i=0, j;
	FILE *fptr;
	
	fptr = fopen("hayyam.txt", "r");
	if( fptr == NULL ){
		printf("File couldn't be opened. Exiting.");
		return 1;
	}

	char **quotes, **wordsdic;
	int **wordvectors, *labels;			//qoutes[i]'s label is labels[i]. (either 1 or -1)
	double *parameters;
	

	quotes = (char**) malloc(MAX_QUOTE*sizeof(char*));				//Allocating memory for quotes, dictionary and vectors.
	if(quotes == NULL){
		printf("Memory allocation failed. Exiting.");
		return 1;
	}
	for( i=0; i<MAX_QUOTE; i++ ){
		quotes[i] = (char*) malloc(MAX_LENGTH*sizeof(char));
	}
	
	wordsdic = (char**)malloc(DICT_SIZE*sizeof(char*));
	if(wordsdic == NULL){
		printf("Memory allocation failed. Exiting.");
		return 1;
	}
	for( i=0; i<DICT_SIZE; i++ ){
		wordsdic[i] = (char*) malloc(MAX_LENGTH*sizeof(char));
	}
	
	wordvectors = (int**) malloc(MAX_QUOTE*sizeof(int*));
	if(wordvectors == NULL){
		printf("Memory allocation failed. Exiting.");
		return 1;
	}
	for( i=0; i<MAX_QUOTE; i++ ){
		wordvectors[i] = (int*) malloc(DICT_SIZE*sizeof(int));
	}
	
	for( i=0; i<MAX_QUOTE; i++ ){
		fgets(quotes[i], MAX_LENGTH, fptr);
	}
	fclose(fptr);
	
	labels = (int*) malloc(MAX_QUOTE*sizeof(int));
	if(labels == NULL){
		printf("Memory allocation failed. Exiting.");
		return 1;
	}
	

	for( i=0; i<MAX_QUOTE; i++ ){
		remove_punctuation(quotes[i]);
		remove_multiple_spaces(quotes[i], strlen(quotes[i]));
	}
	make_lowercase(quotes);
	organize_add(quotes);
	
	/*printf("-------------------PUNCTUATION REMOVED--------------------------\n\n");
	for( i=0; i<MAX_QUOTE; i++ ){
		printf("\ncount test --> %d\n", compute_number_of_words(quotes[i]));
		printf("len = %d\n", strlen(quotes[i]));
		printf("%s\n", quotes[i]);
	}*/
	
	int wordcount = fill_dictionary(wordsdic, quotes); 
	
	/*
	printf("\n-------------------DICTIONARY FORMED--------------------------\n\n");
	printf("wordcount = %d\n", wordcount);
	
	for( i=0; i<MAX_QUOTE; i++ ){
		printf("\ncount test --> %d\n", compute_number_of_words(quotes[i]));
		printf("len = %d\n", strlen(quotes[i]));
		printf("%s|||\n", quotes[i]);
	}
	
    printf("\nUnique words in the dictionary:");
    for (i = 0; i < wordcount; i++) {
        printf("\n%d: %s", i + 1, wordsdic[i]);
    }*/
    
	text_to_vector(wordsdic, quotes, wordvectors, wordcount);
	
	/*for( i=0; i<MAX_QUOTE; i++ ){
    	printf("\n%d. METIN: %s\n%d. METNIN VEKTOR HALI:\n", i+1, quotes[i] ,i+1);
    	for (j = 0; j < wordcount; j++) {
        	printf("%s: %d\n", wordsdic[j], wordvectors[i][j]);
    	}
	}*/
	
	
	
	parameters = (double*) malloc(wordcount*sizeof(double));
	if( parameters == NULL ){
		printf("Memory allocation failed. Exiting."); 
		return 1;
	}
	initiate_param(parameters, wordcount);

	for( i=0; i<MAX_QUOTE; i++ ){
		labels[i] = 1;
	}
	
		
	/*
	for(i=0;i<wordcount;i++) {
		printf("%d.parametre = %f\n",i,parameters[i]);
	}
	*/
	
	
	stoc_grad_desc(wordvectors, parameters, wordcount, labels, 0.01, 1000, 0.01);
	//gradiend_descent(wordvectors, parameters, wordcount, labels,  0.01, 200, 1.0);
	
	
	
	
	
	
	for( i=0; i<MAX_QUOTE; i++ ){
		free(quotes[i]);
	}
	free(quotes);
	for( i=0; i<DICT_SIZE; i++ ){				//FREEING MEMORY.
		free(wordsdic[i]);
	}
	free(wordsdic);
	for( i=0; i<MAX_QUOTE; i++ ){
		free(wordvectors[i]);
	}
	free(wordvectors);
	
	return 0;
}

void gradiend_descent(int **vectors, double *parameters, int wordcount, int *labels, double stepsize, int maxiter, double error) {
    int i=0, t, z;
    double y_hat_std, y_std, gradient, total_loss=0, last_loss;
	
	do{
		last_loss = total_loss;
		total_loss = 0;
        for (t = 0; t < wordcount; t++) {
            gradient = 0;

            for (z = 0; z < MAX_QUOTE; z++) {
                y_std = (labels[z] + 1) / 2;
                y_hat_std = (f_x(vectors[z], parameters, wordcount) + 1) / 2;

                gradient += (y_hat_std - y_std) * vectors[z][t];
            }

            gradient /= MAX_QUOTE;
            parameters[t] -= stepsize * gradient;
        }

        for (t = 0; t < MAX_QUOTE; t++) {
            total_loss += compute_loss(vectors[t], parameters, wordcount, labels);
        }
        total_loss /= MAX_QUOTE;

        printf("Iteration %d: Loss: %lf\n", i + 1, total_loss);
		
		i++;
	} while( ( fabs(total_loss - last_loss) > error || i==1 ) && i<maxiter );
	
	if( i < maxiter ){
		printf("\nModel succesfully converged at %d. iteration.", i);
	}
	else{
		printf("\nModel training lasted for a full %d iterations.", maxiter);
	}

}

void stoc_grad_desc(int **vectors, double *parameters, int wordcount, int *labels, double stepsize, int maxiter, double error){
	int i=0, t, z, chosen, j;
	double y_hat_std, y_std, gradient, total_loss=0, last_loss;
	srand(time(NULL));
	
	do{
		last_loss = total_loss;
		total_loss = 0;
		
		chosen = rand()%MAX_QUOTE;
		y_std = (labels[chosen]+1) / 2;
		y_hat_std = ( f_x(vectors[chosen], parameters, wordcount)+1 ) / 2;
		gradient = 0;
		
		for( j=0; j<wordcount; j++ ){
			gradient = ( y_hat_std - y_std ) * vectors[chosen][j];
			parameters[j] -= stepsize*gradient;
		}
		
		total_loss = 0;
		for( t=0; t<MAX_QUOTE; t++ ){
			total_loss += compute_loss(vectors[t], parameters, wordcount, labels);
		}
		total_loss /= MAX_QUOTE;
		printf("\nIteration %d: 	Loss: %lf", i+1, total_loss);
		i++;
	} while( ( fabs(total_loss - last_loss) > error || i==1 ) && i<maxiter );
	
	if( i < maxiter ){
		printf("\nModel succesfully converged at %d. iteration.", i);
	}
	else{
		printf("\nModel training lasted for a full %d iterations.", maxiter);
	}
	
}





void compute_gradient(int **vectors, double *parameters, int wordcount, int *labels, double *gradients){				//COMPUTES GRADIENT FOR ONE DIMENSION
	int i, j;
	double f, err;
	for( i=0; i<MAX_QUOTE; i++ ){
		f = f_x( vectors[i], parameters, wordcount );
		err = f - labels[i];
		for( j=0; j<wordcount; j++ ){
			gradients[j] += err * vectors[i][j];
		}
		gradients[i] /= MAX_QUOTE;
	}
}

double compute_loss(int *vector, double *parameters, int wordcount, int *labels){		//TEK BIR ORNEK IÇIN HATA HESAPLAR.
	int i;
	double loss = 0, fx = f_x(vector, parameters, wordcount);
	double fx_std = (fx+1.0) / 2.0;
	
	if( fx_std == 0 ) fx_std += 0.00001;
	if( fx_std == 1 ) fx_std -= 0.00001;
	for( i=0; i<MAX_QUOTE; i++ ){
		//printf("\n1: %d		2: %lf		3: %lf", labels[i], log (sigm), log( 1-sigm ));
		loss += labels[i]*log (fx_std) + ( 1-labels[i] )*log( 1-fx_std );
		//printf("\ni = %d	loss = %lf", i, loss);
	}
	return -loss;
}

double dot_product(int *vector, double *parameters, int wordcount){
	int i;
	double result = 0;
	for( i=0; i<wordcount; i++ ){
		result += vector[i]*parameters[i];
	}
	return result;
}

double f_x(int *vector, double *parameters, int wordcount){			//Sygmoid function.
	double dot = dot_product(vector, parameters, wordcount);
	return tanh(dot);
}

void initiate_param(double *parameters, int wordcount){				//TODO: BU SADECE INT YAPIYOR. DOUBLE YAPMANIN YOLUNU BUL
	int i;
	double random_value;
	srand(time(NULL));
	for( i=0; i<wordcount; i++ ){
		random_value = (double)rand()/RAND_MAX*2.0-1.0;
		random_value *= 10;				//float in range -1 to 1
		parameters[i] = 0.0;
	}
}

int fill_dictionary(char **dictionary, char **quotes) {
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
    }
    return count;
}

int check_occurence(char **dictionary, char *word, int check_until){
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
	const char *punctuations = "!()-[]{};:??,<>./?@#$%^&*\"_~?-";
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

void make_lowercase(char **quotes){
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

void text_to_vector(char **dictionary, char **quotes, int **vectors, int wordcount) {
	int i, j;
	for( i=0; i<MAX_QUOTE; i++ ){
		for( j=0; j<wordcount; j++ ){
			if( strstr( quotes[i], dictionary[j] )){			//PLAIN GECYNCE IN'E DE 1 DEDY.
				vectors[i][j] = 1;
			}
			else{
				vectors[i][j] = 0;
			}
		}
	}
}


void slide_text(char *text, int tlen, int start, int step, int direction){	//1 ise sola do?ru kapa, 0 ise sa?a do?ru aç.
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

void organize_add(char **quotes){
	int i, j, len;
	for( i=0; i<MAX_QUOTE; i++ ){
		len = strlen(quotes[i]);
		j=0;
		while( j<len && quotes[i][j] != '\0' ){
			if( quotes[i][j] == '\'' ){			//KESME YSARETY BULDUK. 'VE YA DA N'T MI DYYE BAK.
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
