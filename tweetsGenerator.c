#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define  FALSE 0
#define TRUE 1

typedef struct WordStruct {
    char *word;
    struct WordProbability *prob_list;
    int occurrence;
    int size_of_probabilities;
    //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;
    int occurrence;
    //... Add your own fields here
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return 1;
    }
    *new_node = (Node){data, NULL};

    if (link_list->first == NULL)
    {
        link_list->first = new_node;
        link_list->last = new_node;
    }
    else
    {
        link_list->last->next = new_node;
        link_list->last = new_node;
    }

    link_list->size++;
    return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand()%max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary)
{
    int i;
    Node *temp = dictionary->first;
    int chosen_number = get_random_number(dictionary->size);

    for(i = 0; i < chosen_number; i++){
        temp = temp->next;
    }
    if(temp->data->word[strlen(temp->data->word)-1] == '.')
    {
        return get_first_random_word(dictionary);
    }
    else{
        return temp->data;
    }
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
    int i;
    int chosen_number = get_random_number(word_struct_ptr->occurrence);
    for(i = 0; i < word_struct_ptr->size_of_probabilities; i++){
        if(word_struct_ptr->prob_list[i].occurrence >= chosen_number) {
            return word_struct_ptr->prob_list[i].word_struct_ptr;
        }
        else {
            chosen_number = chosen_number - (word_struct_ptr->prob_list[i].occurrence);
        }
    }
    return word_struct_ptr->prob_list[0].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
    int counter = 1;
    WordStruct * word1;
    WordStruct * next_word;
    word1 = get_first_random_word(dictionary);
    printf("%s ", word1->word);
    next_word=word1;
    while(1)
    {
        if(counter >= MAX_WORDS_IN_SENTENCE_GENERATION || next_word->size_of_probabilities == 0 )
        {
            return counter;
        }
        next_word = get_next_random_word(next_word);
        printf("%s " , next_word->word);
        counter++;

    }
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word, WordStruct *second_word)//check empty list
{
    int i;
    if(first_word->prob_list == NULL)
    { // create a new probability list
        first_word->prob_list = (WordProbability*) malloc(sizeof(WordProbability));
        if(first_word->prob_list == NULL)
        {
            printf("Allocation Failure: can not allocate probability list\n");
        }
        first_word->size_of_probabilities = 1;
        first_word->prob_list[0].word_struct_ptr = second_word;
        first_word->prob_list[0].occurrence = 1;
        return 1;
    }
    for (i = 0; i < first_word->size_of_probabilities; i++)
    {
        if(strcmp(first_word->prob_list[i].word_struct_ptr->word, second_word->word) == 0)
        {
            ++(first_word->prob_list[i].occurrence);
            return 0;
        }
    }
    first_word->prob_list = (WordProbability *) realloc(first_word->prob_list, (1+(first_word->size_of_probabilities)) * sizeof(WordProbability));
    if(first_word->prob_list == NULL)
    {
        printf("Allocation failure: can not reallocate a new word");
        exit(EXIT_FAILURE);
    }
    first_word->size_of_probabilities++;
    first_word->prob_list[first_word->size_of_probabilities-1].occurrence = 1;
    first_word->prob_list[first_word->size_of_probabilities-1].word_struct_ptr = second_word;
    return 1;
}

/* the function looks for a word in a dictionary.
 * if the word has been founded, then the occurrences will be increase and the pointer to that word will be return
 * otherwise- create a new word and add to the dictionary, return the pointer to the new word
 * */
WordStruct* check_if_word_exists(char* word, LinkList * dictionary) {
    Node *pointer = dictionary->first;
    WordStruct* new_word;
    while (pointer != NULL)
    {
        if(strcmp(pointer->data->word, word) == 0)
        {
            ++(pointer->data->occurrence);
            free(word);
            return pointer->data;
        }
        pointer = pointer->next;
    }
    new_word = (WordStruct*)malloc(sizeof(WordStruct));
    if(new_word == NULL)
    {
        printf("Allocation failure: can not allocate a new word struct");
        exit(EXIT_FAILURE);
    }
    new_word->occurrence = 1;
    new_word->word = word;
    new_word->size_of_probabilities = 0;
    new_word->prob_list = NULL;
    if(add(dictionary, new_word) == 1)
    {
        printf("error: fail to add word");
        exit(EXIT_FAILURE);
    }
    return new_word;
}


/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{
    int counter = 1, flag=0;
    WordStruct* last_word, *current_word;
    char string[MAX_SENTENCE_LENGTH+1];
    char * token, *temp;
    while((fgets(string, MAX_SENTENCE_LENGTH+1, fp)) != NULL ){
        string[strlen(string) - 1] = '\0';
        token = strtok(string, " ");
        while(token != NULL )
        {
            ++counter;
            temp = (char*)malloc(strlen(token) + 1);
            if(temp == NULL)
            {
                printf("Allocation failure: can not allocate char word");
                exit(EXIT_FAILURE);
            }
            strcpy(temp, token);
            last_word = current_word;
            current_word = check_if_word_exists(temp, dictionary);
            if((dictionary->first!=NULL) && (flag==1) && (last_word->word[strlen(last_word->word)-1] != '.')) { //the last word in a sentence does not have a problist
                add_word_to_probability_list(last_word, current_word);
            }
            flag=1;
            if(words_to_read < counter && words_to_read!=-1) {
                fclose(fp);
                return;
            }
            token = strtok(NULL, " ");
        }
        flag = 0;
    }
    fclose(fp);
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
    Node * pointer ;
    while (dictionary->first != NULL)
    {
        pointer=dictionary->first;
        free(pointer->data->word);
        if(pointer->data->prob_list != NULL)
            free(pointer->data->prob_list);
        free(pointer->data);
        dictionary->first = dictionary->first->next;
        free(pointer);
    }
    free(dictionary);
}


int check_validity(int args, char *argv[], FILE ** fp)
{
    if(!(args == 5 || args == 4)) {
        printf("usage: <seed> <tweets> <path> <option: num of words>\n");
        exit(EXIT_FAILURE);
    }
    *fp = fopen(argv[3], "r");
    if(*fp == NULL) {
        perror("Error: ");
        exit(EXIT_FAILURE);
    }
    return (args == 4)? -1 :atoi(argv[4]);
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[])
{
    FILE *fp;
    int i;
    int ret = check_validity(argc, argv, &fp);
    srand(atoi(argv[1]));
    LinkList * dictionary = (LinkList*)malloc(sizeof(LinkList));
    if(dictionary == NULL)
    {
        perror("allocation failure: can not allocate dictionary");
        exit(EXIT_FAILURE);
    }
    *dictionary = (LinkList){NULL, NULL, 0};
    fill_dictionary(fp, ret, dictionary);
    for(i = 1; i <= atoi(argv[2]); i++)
    {
        printf("Tweet %d: ", i);
        generate_sentence(dictionary);
        printf("\n");
    }
    free_dictionary(dictionary);
    return 0;
}